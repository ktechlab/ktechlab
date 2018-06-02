/***************************************************************************
 *   Copyright (C) 2006 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dpimage.h"
#include "itemdocument.h"
#include "libraryitem.h"
#include "resizeoverlay.h"

#include <kdebug.h>
#include <kiconloader.h>
#include <klocalizedstring.h>

#include <qpainter.h>
#include <qtimer.h>


//BEGIN class ImageScaleThread
ImageScaleThread::ImageScaleThread()
{
	// Start with a blank (grey) image
	QPixmap pm( 1, 1 );
	pm.fill( Qt::gray );
	m_image = pm.convertToImage();
	
	m_width = -1;
	m_height = -1;
	m_bDoneNormalScale = false;
	m_bDoneSmoothScale = false;
	m_bSettingsChanged = false;
}


bool ImageScaleThread::updateSettings( const QString & imageURL, int width, int height )
{
	if ( running() )
	{
		kWarning() << k_funcinfo << "Cannot update settings while running.\n";
		return false;
	}
	
	bool changed = false;
	
	if ( m_width != width )
	{
		m_width = width;
		changed = true;
	}
	
	if ( m_height != height )
	{
		m_height = height;
		changed = true;
	}
	
	if ( m_imageURL != imageURL )
	{
		m_imageURL = imageURL;
		m_image.load( m_imageURL );
		if ( m_image.isNull() )
		{
			QPixmap pm( 1, 1 );
			pm.fill( Qt::gray );
			m_image = pm.convertToImage();
		}
		changed = true;
	}
	
	if ( changed )
	{
		m_bSettingsChanged = true;
		m_bDoneNormalScale = false;
		m_bDoneSmoothScale = false;
	}
	
	return changed;
}



QImage ImageScaleThread::bestScaling( BestScaling * scaling ) const
{
	BestScaling temp;
	if ( !scaling )
		scaling = & temp;
	
	if ( m_bDoneSmoothScale )
	{
		*scaling = SmoothScaled;
		return m_smoothScaled;
	}
	
	else if ( m_bDoneNormalScale )
	{
		*scaling = NormalScaled;
		return m_normalScaled;
	}
	
	else
	{
		*scaling = Unscaled;
		return m_image;
	}
}


void ImageScaleThread::run()
{
	do
	{
		m_bSettingsChanged = false;
		if ( !m_bDoneNormalScale )
		{
			m_normalScaled = m_image.scaled( m_width, m_height );
			m_bDoneNormalScale = true;
		}
	}
	while ( m_bSettingsChanged );
	
	// If m_bSettingsChanged is true, then another thread called updateSettings
	// while we were doing normal scaling, so don't both doing smooth scaling
	// just yet.
	
	if ( !m_bDoneSmoothScale )
	{
		m_smoothScaled = m_image.smoothScale( m_width, m_height );
		m_bDoneSmoothScale = true;
	}
}
//END class ImageScaleThread



//BEGIN class DPImage
Item* DPImage::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new DPImage( itemDocument, newItem, id );
}


LibraryItem* DPImage::libraryItem()
{
	return new LibraryItem(
			QStringList(QString("dp/image")),
	i18n("Image"),
	i18n("Other"),
	KIconLoader::global()->loadIcon( "text", KIconLoader::Small ),
	LibraryItem::lit_drawpart,
	DPImage::construct );
}


DPImage::DPImage( ItemDocument *itemDocument, bool newItem, const char *id )
	: DrawPart( itemDocument, newItem, id ? id : "image" )
{
	m_bSettingsChanged = false;
	m_bResizeToImage = newItem;
	m_imageScaling = ImageScaleThread::Unscaled;
	
	m_pRectangularOverlay = new RectangularOverlay( this );
	
	m_pCheckImageScalingTimer = new QTimer( this );
	connect( m_pCheckImageScalingTimer, SIGNAL(timeout()), SLOT(checkImageScaling()) );
	m_pCheckImageScalingTimer->start( 100 );
	
	m_name = i18n("Image");
	
	Variant * v = createProperty( "image", Variant::Type::FileName );
	v->setCaption( i18n("Image File") );
	dataChanged();
}


DPImage::~DPImage()
{
	m_imageScaleThread.wait();
}


void DPImage::setSelected( bool yes )
{
	if ( yes == isSelected() )
		return;
	
	DrawPart::setSelected(yes);
	m_pRectangularOverlay->showResizeHandles(yes);
}


void DPImage::postResize()
{
	setItemPoints( QPolygon(m_sizeRect), false );
	m_bSettingsChanged = true;
}


void DPImage::dataChanged()
{
	m_imageURL = dataString( "image" );
	m_image.load( m_imageURL );
	
	if ( m_image.isNull() )
	{
		// Make a grey image
		m_image.resize( width(), height() );
		m_image.fill( Qt::gray );
		
		m_imageScaling = ImageScaleThread::SmoothScaled;
	}
	else
	{
		if ( m_bResizeToImage )
		{
			int w = m_image.width();
			int h = m_image.height();
			setSize( 0, 0, w, h );
			m_imageScaling = ImageScaleThread::SmoothScaled;
		}
		else
		{
			m_bResizeToImage = true;
			m_bSettingsChanged = true;
		}
	}
}


void DPImage::checkImageScaling()
{
	if ( !m_bSettingsChanged && (m_imageScaling == ImageScaleThread::SmoothScaled) )
	{
		// Image scaling is already at its best, so return
		return;
	}
	
	ImageScaleThread::BestScaling bs;
	QImage im = m_imageScaleThread.bestScaling( & bs );
	if ( bs > m_imageScaling )
	{
		m_imageScaling = bs;
		m_image = im;
		setChanged();
	}
	
	if ( !m_imageScaleThread.running() )
	{
		if ( m_imageScaleThread.updateSettings( m_imageURL, width(), height() ) )
		{
			m_bSettingsChanged = false;
			m_imageScaling = ImageScaleThread::Unscaled;
			m_imageScaleThread.start();
		}
	}
}


void DPImage::drawShape( QPainter & p )
{
	p.drawPixmap( int(x()+offsetX()), int(y()+offsetY()), m_image, 0, 0, width(), height() );
}
//END class DPImage

#include "dpimage.moc"

