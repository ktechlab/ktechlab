/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cnitem.h"
#include "canvasitemparts.h"
#include "eventinfo.h"

#include <qevent.h>

// #include <q3button.h>

#include <qdebug.h>

CIWidgetMgr::CIWidgetMgr( KtlQCanvas *canvas, CNItem *item )
{
	p_cnItem = item;
	p_canvas = canvas;
}


CIWidgetMgr::~CIWidgetMgr()
{
	// KtlQCanvas deletes our items for us. Actually, it pretty much insists on deleting them,
	// despite me telling it not to, so if I delete them then it gets confused and crashes.
	// Naughty KtlQCanvas!
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		delete it.value();
	}
	m_widgetMap.clear();
}


void CIWidgetMgr::setWidgetsPos( const QPoint &pos )
{
	m_pos = pos;
}


void CIWidgetMgr::setDrawWidgets( bool draw )
{
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		draw ? it.value()->show() : it.value()->hide();
	}
}


Widget *CIWidgetMgr::widgetWithID( const QString &id ) const
{
	WidgetMap::const_iterator it = m_widgetMap.find(id);
	if ( it == m_widgetMap.end() )
		return 0l;
	else return it.value();
}


Button *CIWidgetMgr::button( const QString &id ) const
{
	return dynamic_cast<Button*>(widgetWithID(id));
}

Slider *CIWidgetMgr::slider( const QString &id ) const
{
	return dynamic_cast<Slider*>(widgetWithID(id));
}



void CIWidgetMgr::setButtonState( const QString &id, int state )
{
	Button *b = button(id);
	if (!b)
		return;
	
	// Actually, we don't want to check to see if we are already down; this way,
	// we get toggle events when loading from file
// 	bool oldState = b->isDown();
// 	if ( oldState == state )
// 		return;
	
	b->setState(state);
}


void CIWidgetMgr::drawWidgets( QPainter &p )
{
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		it.value()->drawShape(p);
	}
}


void CIWidgetMgr::removeWidget( const QString & id )
{
	if ( !m_widgetMap.contains(id) )
		return;
	
	delete m_widgetMap[id];
	m_widgetMap.remove(id);
}


Button* CIWidgetMgr::addButton( const QString &id, const QRect & pos, const QString &display, bool toggle )
{
	WidgetMap::iterator it;
	
	Button *button = new Button( id, p_cnItem, toggle, pos, p_canvas );
	(dynamic_cast<QToolButton*>(button->widget()))->setText(display);
	
	it = m_widgetMap.find(id);
	if ( it == m_widgetMap.end() )
	{
		m_widgetMap[id] = button;
	}
	else
	{
		qWarning() << "CIWidgetMgr::addButton: Attempting to re-add button with same id as previous"<<endl;
		delete it.value();
		it.value() = button;
	}
	
	p_cnItem->updateAttachedPositioning();
	return button;
}


Button* CIWidgetMgr::addButton( const QString &id, const QRect & pos, const QIcon &icon, bool toggle )
{
	WidgetMap::iterator it;
	
	Button *button = new Button( id, p_cnItem, toggle, pos, p_canvas );
	button->setIcon( icon );
	
	it = m_widgetMap.find(id);
	if ( it == m_widgetMap.end() )
	{
		m_widgetMap[id] = button;
	}
	else
	{
		qWarning() << "CIWidgetMgr::addButton: Attempting to re-add button with same id as previous"<<endl;
		delete it.value();
		it.value() = button;
	}
	
	p_cnItem->updateAttachedPositioning();
	return button;
}


Slider* CIWidgetMgr::addSlider( const QString &id, int minValue, int maxValue, int pageStep, int value, Qt::Orientation orientation, const QRect & pos )
{
	Slider *slider = new Slider( id, p_cnItem, pos, p_canvas );
	QSlider *qslider = dynamic_cast<QSlider*>(slider->widget());
	
	qslider->setMinimum(minValue);
	qslider->setMaximum(maxValue);
	qslider->setPageStep(pageStep);
	qslider->setValue(value);
	slider->setOrientation(orientation);
	
	WidgetMap::iterator it = m_widgetMap.find(id);
	if ( it == m_widgetMap.end() )
	{
		m_widgetMap[id] = slider;
	}
	else
	{
		qWarning() << "CIWidgetMgr::addSlider: Attempting to re-add slider with same id as previous"<<endl;
		delete slider;
		return 0l;
	}
	
	p_cnItem->updateAttachedPositioning();
	return slider;
}


bool CIWidgetMgr::mousePressEvent( const EventInfo &info )
{
	QMouseEvent *e = info.mousePressEvent( 0, 0 );
	
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		if ( it.value()->rect().contains(info.pos) )
		{
			it.value()->mousePressEvent(e);
			if (e->isAccepted())
			{
				delete e;
				return true;
			}
		}
	}
	delete e;
	return false;
}


bool CIWidgetMgr::mouseReleaseEvent( const EventInfo &info )
{
	QMouseEvent *e = info.mouseReleaseEvent( 0, 0 );

    qDebug() << Q_FUNC_INFO << " button=" << e->button() << " buttons=" << e->buttons();
	
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		it.value()->mouseReleaseEvent(e);
	}
	
	bool accepted = e->isAccepted();
	delete e;
	return accepted;
}


bool CIWidgetMgr::mouseDoubleClickEvent( const EventInfo &info )
{
	QMouseEvent *e = info.mouseDoubleClickEvent( 0, 0 );
	
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		if ( it.value()->rect().contains(info.pos) )
		{
			it.value()->mouseDoubleClickEvent(e);
			if (e->isAccepted())
			{
				delete e;
				return true;
			}
		}
	}
	delete e;
	return false;
}


bool CIWidgetMgr::mouseMoveEvent( const EventInfo &info )
{
	QMouseEvent *e = info.mouseMoveEvent( 0, 0 );
	
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		it.value()->mouseMoveEvent(e);
		if (e->isAccepted())
		{
			delete e;
			return true;
		}
	}
	delete e;
	return false;
}


bool CIWidgetMgr::wheelEvent( const EventInfo &info )
{
	QWheelEvent *e = info.wheelEvent( 0, 0 );
	
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		if ( it.value()->rect().contains(info.pos) )
		{
			it.value()->wheelEvent(e);
			if (e->isAccepted())
			{
				delete e;
				return true;
			}
		}
	}
	
	delete e;
	return false;
}


void CIWidgetMgr::enterEvent(QEvent *)
{
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		it.value()->enterEvent(0);
	}
}


void CIWidgetMgr::leaveEvent(QEvent *)
{
	const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
	for ( WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it )
	{
		it.value()->leaveEvent(0);
	}
}


