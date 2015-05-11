/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CIWIDGETMGR_H
#define CIWIDGETMGR_H

#include <Qt/qmap.h>
#include <Qt/qstring.h>

class Button;
class CNItem;
class Slider;
class KtlQCanvas;
class Widget;

typedef QMap<QString, Widget*> WidgetMap;

/**
This class handles the widgets (those things associated with CNItems that use QWidgets.
This class is pretty much to maintain a tidy interface: the functions could just as well be
all shoved in CNItem, but that gets messy.
@author David Saxton
*/
class CIWidgetMgr
{
public:
	CIWidgetMgr( KtlQCanvas *canvas, CNItem *item );
	virtual ~CIWidgetMgr();
	
	/**
	 * Set the top-left position from which mouse events are interpreted and the
	 * widgets are drawn from.
	 */
	void setWidgetsPos( const QPoint &pos );
	/**
	 * Returns a pointer to the widget with the given id, or NULL if no such
	 * widgets are found.
	 */
	Widget *widgetWithID( const QString &id ) const;
	Button *button( const QString &id ) const;
	Slider *slider( const QString &id ) const;
	void setButtonState( const QString &id, int state );
	/**
	 * Adds a slider with the given id and values to the position
	 */
	Slider* addSlider( const QString &id, int minValue, int maxValue, int pageStep, int value, Qt::Orientation orientation, const QRect & pos );
	/**
	 * Essentially the same as addDisplayText, but displays a button with
	 * text on it. The virtual functions buttonPressed( const QString &id ) and
	 * buttonReleased( const QString &id ) are called as appropriate with button id
	 */
	Button* addButton( const QString &id, const QRect & pos, const QString &display, bool toggle = false );
	/**
	 * Adds a button with a QPixmap pixmap on it instead of text
	 * @see void addButton( const QString &id, QRect pos, const QString &display )
	 */
	Button* addButton( const QString &id, const QRect & pos, QPixmap pixmap, bool toggle = false );
	/**
	 * Removes the widget with the given id.
	 */
	void removeWidget( const QString & id );
	/**
	 * Sets whether or not to draw the widgets (drawing widgets mucks up SVG
	 * export). This function just calls either hide() or show() in each widget.
	 */
	void setDrawWidgets( bool draw );
	
	bool mousePressEvent( const EventInfo &info );
	bool mouseReleaseEvent( const EventInfo &info );
	bool mouseDoubleClickEvent ( const EventInfo &info );
	bool mouseMoveEvent( const EventInfo &info );
	bool wheelEvent( const EventInfo &info );
	void enterEvent();
	void leaveEvent();
	
	virtual void buttonStateChanged( const QString &/*id*/, bool /*on*/ ) {};
	virtual void sliderValueChanged( const QString &/*id*/, int /*value*/ ) {};
	
	int mgrX() const { return m_pos.x(); }
	int mgrY() const { return m_pos.y(); }
	/**
	 * Draw the widgets using the given painter. This function isn't actually
	 * used to draw the widgets on the canvas, as they are QCanvasItems
	 * themselves, but allows other classes (e.g. ItemLibrary) to draw them
	 * using a special painter.
	 */
	void drawWidgets( QPainter &p );
	
protected:
	WidgetMap m_widgetMap;
	QPoint m_pos;
	KtlQCanvas *p_canvas;
	CNItem *p_cnItem;
};

#endif
