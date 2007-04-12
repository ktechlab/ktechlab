/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ITEMVIEW_H
#define ITEMVIEW_H

#include <view.h>

#include <qcanvas.h>
#include <qguardedptr.h>

class CVBEditor;
class ItemDocument;
class QTimer;

/**
@author David Saxton
*/
class ItemView : public View
{
	Q_OBJECT
	public:
		ItemView( ItemDocument *itemDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name = 0 );
		~ItemView();

		virtual bool canZoomIn() const;
		virtual bool canZoomOut() const;
		CVBEditor *cvbEditor() const { return m_CVBEditor; }
		/**
		 * @returns The zoom level
		 */
		double zoomLevel() const { return m_zoomLevel; }

	public slots:
		void actualSize();
		void zoomIn();
		void zoomOut();
		void scrollToMouse( const QPoint &pos );
		virtual void updateStatus();

	protected slots:
		/**
		 * Called when the user changes the configuration.
		 */
		virtual void slotUpdateConfiguration();
		void startUpdatingStatus();
		void stopUpdatingStatus();

	protected:
		void updateZoomActions();
		/**
		 * Attempts to create a new CNItem if one was dragged onto the canvas
		 */
		void dropEvent( QDropEvent* );
		/**
		 * Reinherit to allow different types of items to be dragged in.
		 */
		virtual void dragEnterEvent( QDragEnterEvent* );
		void contentsMousePressEvent( QMouseEvent *e );
		void contentsMouseReleaseEvent( QMouseEvent *e );
		void contentsMouseDoubleClickEvent( QMouseEvent *e );
		void contentsMouseMoveEvent( QMouseEvent *e );
		void contentsWheelEvent( QWheelEvent *e );
		void enterEvent( QEvent * e );
		void leaveEvent( QEvent * e );

		QGuardedPtr<ItemDocument> p_itemDocument;
		CVBEditor *m_CVBEditor;
		double m_zoomLevel;
		QTimer * m_pUpdateStatusTmr;
	
		friend class CVBEditor;
};

/**
@author David Saxton
*/
class CVBEditor : public QCanvasView
{
Q_OBJECT
public:
    CVBEditor( QCanvas *canvas, ItemView *itemView, const char *name );
	
	void setPassEventsToView( bool pass ) { b_passEventsToView = pass; }

	virtual void contentsMousePressEvent( QMouseEvent* e );
	virtual void contentsMouseReleaseEvent( QMouseEvent* e );
	virtual void contentsMouseDoubleClickEvent( QMouseEvent* e );
	virtual void contentsMouseMoveEvent( QMouseEvent* e );
	virtual void dragEnterEvent( QDragEnterEvent* e );
	virtual void dropEvent( QDropEvent* e );
	virtual void contentsWheelEvent( QWheelEvent *e );
	virtual void enterEvent( QEvent * e );
	virtual void leaveEvent( QEvent * e );

protected:
	virtual void viewportResizeEvent( QResizeEvent * );
	ItemView *p_itemView;
	bool b_passEventsToView;
	bool b_ignoreEvents;
};

#endif
