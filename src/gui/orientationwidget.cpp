/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
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

#include <kstandarddirs.h>
#include <qbitmap.h>
#include <qlayout.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>

const int _size = 44;

OrientationWidget::OrientationWidget(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	QGridLayout *layout = new QGridLayout( this, 2, 4, 0, 4);
	p_activeFlowPart = 0;
	
	for( int row=0; row<2; ++row)
	{
		for( int col=0; col<4; ++col)
		{
			QPushButton *btn = new QPushButton(this);
			m_toolBtn[row][col] = btn;
			layout->addWidget( btn, row, col);
			btn->setFixedSize( _size+6, _size+6);
// 			btn->setFlat(true);
			btn->setEnabled(false);
		}
	}
	
	connect( m_toolBtn[0][0], SIGNAL(clicked()), this, SLOT(set_cio_noflip_0()));
	connect( m_toolBtn[0][1], SIGNAL(clicked()), this, SLOT(set_cio_noflip_90()));
	connect( m_toolBtn[0][2], SIGNAL(clicked()), this, SLOT(set_cio_noflip_180()));
	connect( m_toolBtn[0][3], SIGNAL(clicked()), this, SLOT(set_cio_noflip_270()));
	connect( m_toolBtn[1][0], SIGNAL(clicked()), this, SLOT(set_cio_flip_0()));
	connect( m_toolBtn[1][1], SIGNAL(clicked()), this, SLOT(set_cio_flip_90()));
	connect( m_toolBtn[1][2], SIGNAL(clicked()), this, SLOT(set_cio_flip_180()));
	connect( m_toolBtn[1][3], SIGNAL(clicked()), this, SLOT(set_cio_flip_270()));
}


OrientationWidget::~OrientationWidget()
{
}

void OrientationWidget::slotUpdate( CNItem *activeCNItem)
{
	p_activeFlowPart = dynamic_cast<FlowPart*>(activeCNItem);
	if(p_activeFlowPart)
	{
		initFromFlowPart(p_activeFlowPart);
		return;
	}
	
	Component *activeComponent = dynamic_cast<Component*>(activeCNItem);
	
	if( activeComponent && (activeComponent->canRotate() || activeComponent->canFlip()))
	{
		initFromComponent(activeComponent);
		return;
	}
	
	slotClear();
}


void OrientationWidget::initFromFlowPart( FlowPart *flowPart)
{
	if(!flowPart)
		return;
	
	uint valid = flowPart->allowedOrientations();
	
// 	m_toolBtn[0][0]->setText("b00");
	for( uint i=0; i<2; ++i)
	{
		for( uint j=0; j<4; ++j)
		{
			uint o = j + 4*i;
			if( valid & (1<<o))
			{
				m_toolBtn[i][j]->setEnabled(true);
				QPixmap pm( 50, 50);
				flowPart->orientationPixmap( o, pm);
				m_toolBtn[i][j]->setPixmap(pm);
			}
		}
	}
}


void OrientationWidget::initFromComponent( Component *component)
{
	const QImage im = itemLibrary()->itemImage(component);
	
	QRect bound = component->boundingRect();
	
	// We want a nice square bounding rect
	const int dy = bound.width() - bound.height();
	if( dy > 0)
	{
		bound.setTop( bound.top()-(dy/2));
		bound.setBottom( bound.bottom()+(dy/2));
	}
	else if( dy < 0)
	{
		bound.setLeft( bound.left()+(dy/2));
		bound.setRight( bound.right()-(dy/2));
	}
	

	QPixmap tbPm;
	tbPm.convertFromImage(im);
	m_toolBtn[0][0]->setPixmap(tbPm);
	m_toolBtn[0][0]->setEnabled(true);
	
	if( component->canRotate())
	{
// 		QPixmap tbPm;
		tbPm.convertFromImage( im.xForm( Component::transMatrix( 90, false, bound.width()/2, bound.height()/2)));
		m_toolBtn[0][1]->setPixmap(tbPm);
		m_toolBtn[0][1]->setEnabled(true);
		
// 		QPixmap tbPm;
		tbPm.convertFromImage( im.xForm( Component::transMatrix( 180, false, bound.width()/2, bound.height()/2)));
		m_toolBtn[0][2]->setPixmap(tbPm);
		m_toolBtn[0][2]->setEnabled(true);
		
// 		QPixmap tbPm;
		tbPm.convertFromImage( im.xForm( Component::transMatrix( 270, false, bound.width()/2, bound.height()/2)));
		m_toolBtn[0][3]->setPixmap(tbPm);
		m_toolBtn[0][3]->setEnabled(true);
	}
	
	if( component->canFlip())
	{
// 		QPixmap tbPm;
		tbPm.convertFromImage( im.xForm( Component::transMatrix( 0, true, bound.width()/2, bound.height()/2)));
		m_toolBtn[1][0]->setPixmap(tbPm);
		m_toolBtn[1][0]->setEnabled(true);
		
		if( component->canRotate())
		{
// 			QPixmap tbPm;
			tbPm.convertFromImage( im.xForm( Component::transMatrix( 90, true, bound.width()/2, bound.height()/2)));
			m_toolBtn[1][1]->setPixmap(tbPm);
			m_toolBtn[1][1]->setEnabled(true);
			
// 			QPixmap tbPm;
			tbPm.convertFromImage( im.xForm( Component::transMatrix( 180, true, bound.width()/2, bound.height()/2)));
			m_toolBtn[1][2]->setPixmap(tbPm);
			m_toolBtn[1][2]->setEnabled(true);
			
// 			QPixmap tbPm;
			tbPm.convertFromImage( im.xForm( Component::transMatrix( 270, true, bound.width()/2, bound.height()/2)));
			m_toolBtn[1][3]->setPixmap(tbPm);
			m_toolBtn[1][3]->setEnabled(true);
		}
	}
}


void OrientationWidget::slotClear()
{
	for( int row=0; row<2; ++row)
	{
		for( int col=0; col<4; ++col)
		{
			// Hmm...this line has crashed before
			m_toolBtn[row][col]->setPixmap( QPixmap::QPixmap());
			m_toolBtn[row][col]->setEnabled(false);
		}
	}
}


void OrientationWidget::set_cio_noflip_0()
{
	if(p_activeFlowPart)
		ItemInterface::self()->setFlowPartOrientation(0);
	else
		ItemInterface::self()->setComponentOrientation( 0, false);
}
void OrientationWidget::set_cio_noflip_90()
{
	if(p_activeFlowPart)
		ItemInterface::self()->setFlowPartOrientation(1);
	else
		ItemInterface::self()->setComponentOrientation( 90, false);
}
void OrientationWidget::set_cio_noflip_180()
{
	if(p_activeFlowPart)
		ItemInterface::self()->setFlowPartOrientation(2);
	else
		ItemInterface::self()->setComponentOrientation( 180, false);
}
void OrientationWidget::set_cio_noflip_270()
{
	if(p_activeFlowPart)
		ItemInterface::self()->setFlowPartOrientation(3);
	else
		ItemInterface::self()->setComponentOrientation( 270, false);
}
void OrientationWidget::set_cio_flip_0()
{
	if(p_activeFlowPart)
		ItemInterface::self()->setFlowPartOrientation(4);
	else
		ItemInterface::self()->setComponentOrientation( 0, true);
}
void OrientationWidget::set_cio_flip_90()
{
	if(p_activeFlowPart)
		ItemInterface::self()->setFlowPartOrientation(5);
	else
		ItemInterface::self()->setComponentOrientation( 90, true);
}
void OrientationWidget::set_cio_flip_180()
{
	if(p_activeFlowPart)
		ItemInterface::self()->setFlowPartOrientation(6);
	else
		ItemInterface::self()->setComponentOrientation( 180, true);
}
void OrientationWidget::set_cio_flip_270()
{
	if(p_activeFlowPart)
		ItemInterface::self()->setFlowPartOrientation(7);
	else
		ItemInterface::self()->setComponentOrientation( 270, true);
}

#include "orientationwidget.moc"



