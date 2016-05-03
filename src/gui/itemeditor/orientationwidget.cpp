/***************************************************************************
 *   Copyright (C) 2003-2006 David Saxton <david@bluehaze.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cnitem.h"
#include "cnitemgroup.h"
#include "component.h"
#include "flowpart.h"
#include "iteminterface.h"
#include "itemlibrary.h"
#include "orientationwidget.h"
#include "node.h"

#include <kdebug.h>
#include <kstandarddirs.h>

#include <Qt/qbitmap.h>
#include <Qt/qlayout.h>
#include <Qt/qimage.h>
#include <Qt/qpixmap.h>
#include <Qt/qpainter.h>
#include <Qt/qpushbutton.h>

#include <cassert>

const int _size = 44;

//BEGIN class DrawingPushButton
struct DrawingPushButton : public QPushButton {
    DrawingPushButton(QWidget *parent) : QPushButton(parent) { }


    virtual void paintEvent(QPaintEvent *ev) {
        QPushButton::paintEvent(ev);
        //QPainter painter(this);// 2016.05.03 - explicitly initialize painter
        QPainter painter;
        const bool isSuccess = painter.begin(this);
        if (!isSuccess) {
            qWarning() << Q_FUNC_INFO << " painter not active";
        }
        painter.drawPixmap( 0, 0, toDiplayPixmap);
    }

    void setToDisplayPixmap(const QPixmap &pixmap) {
        toDiplayPixmap = pixmap.copy(); // need to explicitly copy, or all the buttons will look the same
    }

    QPixmap toDiplayPixmap;
};

//END class DrawingPushButton

//BEGIN class OrientationWidget
OrientationWidget::OrientationWidget(QWidget *parent, const char *name)
	: QWidget( parent, name )
{
	QGridLayout * layout = new QGridLayout( this, 2, 4, 0, 4 );
	
	for ( int row=0; row<2; ++row )
	{
		for ( int col=0; col<4; ++col )
		{
			DrawingPushButton *btn = new DrawingPushButton(this);
			m_toolBtn[row][col] = btn;
			layout->addWidget( btn, row, col );
			btn->setFixedSize( _size+6, _size+6 );
// 			btn->setFlat(true);
			btn->setToggleButton( true );
			btn->setEnabled(false);
			
			connect( btn, SIGNAL(clicked()), this, SLOT(slotButtonClicked()) );
		}
	}
}


OrientationWidget::~OrientationWidget()
{
}


void OrientationWidget::slotUpdate( CNItemGroup * itemGroup )
{
	if ( m_pCNItem )
		m_pCNItem->disconnect( this );
	
	m_pCNItem = dynamic_cast<CNItem*>( itemGroup->activeItem() );
	if ( m_pCNItem )
		connect( m_pCNItem, SIGNAL(orientationChanged()), this, SLOT(updateShownOrientation()) );
	
	bool haveSameOrientation = itemGroup->haveSameOrientation();
	
	if ( FlowPart * flowPart = dynamic_cast<FlowPart*>((CNItem*)m_pCNItem) )
	{
		// Do we actually need to udpate the interface?
		if ( m_pFlowPart && (m_bHaveSameOrientation == haveSameOrientation) )
			return;
		
		m_pComponent = 0l;
		m_pFlowPart = flowPart;
		m_bHaveSameOrientation = haveSameOrientation;
		
		initFromFlowPart( m_pFlowPart );
	}
	
	else if ( Component * component = dynamic_cast<Component*>((CNItem*)m_pCNItem) )
	{
		// Do we actually need to udpate the interface?
		if ( m_pComponent && (m_bHaveSameOrientation == haveSameOrientation) )
			return;
		
		m_pFlowPart = 0l;
		m_pComponent = component;
		m_bHaveSameOrientation = haveSameOrientation;
		
		initFromComponent( m_pComponent );
	}
	
	else
		slotClear();
}


void OrientationWidget::initFromFlowPart( FlowPart * flowPart )
{
	if (!flowPart)
		return;
	
	uint valid = flowPart->allowedOrientations();
	
	for ( uint i=0; i<2; ++i )
	{
		for ( uint j=0; j<4; ++j )
		{
			uint o = j + 4*i;
			if ( valid & (1<<o) )
			{
				m_toolBtn[i][j]->setEnabled(true);
				QPixmap pm( 50, 50 );
				flowPart->orientationPixmap( o, pm );
				//m_toolBtn[i][j]->setPixmap(pm);
                m_toolBtn[i][j]->setToDisplayPixmap(pm);
                m_toolBtn[i][j]->update();
			}
		}
	}
	
	updateShownOrientation();
}


void OrientationWidget::initFromComponent( Component * component )
{
	const QImage im = itemLibrary()->componentImage( component );
	
	QRect bound = component->boundingRect();
	
	// We want a nice square bounding rect
	const int dy = bound.width() - bound.height();
	if ( dy > 0 )
	{
		bound.setTop( bound.top()-(dy/2) );
		bound.setBottom( bound.bottom()+(dy/2) );
	}
	else if ( dy < 0 )
	{
		bound.setLeft( bound.left()+(dy/2) );
		bound.setRight( bound.right()-(dy/2) );
	}
	

	QPixmap tbPm;
	
	for ( unsigned col = 0; col < 4; ++col )
	{
		for ( unsigned row = 0; row < 2; ++row )
		{
			bool flipped = bool(row);
			int angle = 90 * col;
			
			if ( col || row ) {
				tbPm.convertFromImage( im.transformed(
                        Component::transMatrix( angle, flipped, bound.width()/2, bound.height()/2 ) ) );
            } else {
				tbPm.convertFromImage( im );
            }
			
			//m_toolBtn[row][col]->setPixmap(tbPm);
            m_toolBtn[row][col]->setToDisplayPixmap(tbPm);
            m_toolBtn[row][col]->update();
			m_toolBtn[row][col]->setEnabled(true);
		}
	}
	
	updateShownOrientation();
}


void OrientationWidget::slotClear()
{
	if ( m_pCNItem )
		m_pCNItem->disconnect( this );
	
	m_pComponent = 0l;
	m_pFlowPart = 0l;
	m_pCNItem = 0l;
	
	for ( int row=0; row<2; ++row )
	{
		for ( int col=0; col<4; ++col )
		{
			// Hmm...this line has crashed before
			//m_toolBtn[row][col]->setPixmap( QPixmap() );
            m_toolBtn[row][col]->setToDisplayPixmap(QPixmap());
            m_toolBtn[row][col]->update();
			m_toolBtn[row][col]->setEnabled(false);
		}
	}
}


void OrientationWidget::slotButtonClicked()
{
	QPushButton * btn = const_cast<QPushButton *>( dynamic_cast<const QPushButton *>(sender()) );
	assert( btn );
	
	for ( int row=0; row<2; ++row )
	{
		for ( int col=0; col<4; ++col )
		{
			bool isButton = (m_toolBtn[row][col] == btn);
			m_toolBtn[row][col]->setOn( isButton );
			if ( !isButton )
				continue;
			
			if ( m_pFlowPart )
				ItemInterface::self()->setFlowPartOrientation( (4*row) + col );
			else
				ItemInterface::self()->setComponentOrientation( 90*col, bool(row) );
		}
	}
}


void OrientationWidget::updateShownOrientation()
{
	if ( !m_pFlowPart && !m_pComponent )
		return;
	
	CNItemGroup * ig = dynamic_cast<CNItemGroup *>( ItemInterface::self()->itemGroup() );
	if ( !ig )
		return;
	
	bool haveSameOrientation = ig->haveSameOrientation();
	
	
	for ( unsigned col = 0; col < 4; ++col )
	{
		for ( unsigned row = 0; row < 2; ++row )
		{
			if ( m_pFlowPart )
			{
				bool setOn = haveSameOrientation && (m_pFlowPart->orientation() == (4*row + col));
				m_toolBtn[row][col]->setOn( setOn );
			}
			else
			{
				bool flipped = bool(row);
				int angle = 90 * col;
			
				bool setOn = haveSameOrientation && (m_pComponent->angleDegrees() == angle) && (m_pComponent->flipped() == flipped);
				m_toolBtn[row][col]->setOn( setOn );
			}
		}
	}
}
//END class OrientationWidget

#include "orientationwidget.moc"

