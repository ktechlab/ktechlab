/***************************************************************************
 *   Copyright (C) 2005-2006 David Saxton                                  *
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

#include <canvas.h>
#include <qpointer.h>

class Canvas;
class CVBEditor;
class Item;
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
		/**
		 * When the user drags from an item selector into the item view, the
		 * item will be created and shown to the user. This function returns
		 * that item.
		 */
		Item * dragItem() const { return m_pDragItem; }
		/**
		 * Zoom in. The point center will remain fixed.
		 */
		void zoomIn( const QPoint & center );
		/**
		 * Zoom in. The point center will remain fixed.
		 */
		void zoomOut( const QPoint & center );
		/**
		 * Converts a mouse click position (in the contents coordinates) to the
		 * associated position on the canvas.
		 */
		QPoint mousePosToCanvasPos( const QPoint & contentsClick ) const;

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
		/**
		 * If the user drags an acceptable item in (e.g. a component in a
		 * circuit), then call this function to create the item and have it
		 * moved when the user moves his mouse while dragging.
		 */
		void createDragItem( QDragEnterEvent * event );
		void removeDragItem();
		void updateZoomActions();

    public:
		/**
		 * Attempts to create a new CNItem if one was dragged onto the canvas
		 */
		void dropEvent( QDropEvent * event );
		/**
		 * Reinherit to allow different types of items to be dragged in.
		 */
		virtual void dragEnterEvent( QDragEnterEvent * event );
		void dragLeaveEvent( QDragLeaveEvent * event );
		void dragMoveEvent( QDragMoveEvent * event );
		void contentsMousePressEvent( QMouseEvent *e );
		void contentsMouseReleaseEvent( QMouseEvent *e );
		void contentsMouseDoubleClickEvent( QMouseEvent *e );
		void contentsMouseMoveEvent( QMouseEvent *e );
		void contentsWheelEvent( QWheelEvent *e );
		void enterEvent( QEvent * e );
		void leaveEvent( QEvent * e );

        void requestDocumentResizeToCanvasItems();

    protected:
		QPointer<ItemDocument> p_itemDocument;
		CVBEditor *m_CVBEditor;
		double m_zoomLevel;
		QTimer * m_pUpdateStatusTmr;
		Item * m_pDragItem;
	
		//friend class CVBEditor; // 2018.09.26
};


/**
@author David Saxton
*/
class CVBEditor : public KtlQCanvasView
{
	Q_OBJECT
	public:
		CVBEditor( Canvas * canvas, ItemView *itemView, const char *name );
		
		void setPassEventsToView( bool pass ) { b_passEventsToView = pass; }
		virtual bool event( QEvent * e );
		virtual void contentsWheelEvent( QWheelEvent * e );
		/**
		 * Updates the world matrix from ItmeView's zoom level and from Canvas'
		 * offset.
		 */
		void updateWorldMatrix();
		
	protected slots:
		void canvasResized( const QRect & oldSize, const QRect & newSize );
		
	protected:
		virtual void viewportResizeEvent( QResizeEvent * );
		ItemView *p_itemView;
		bool b_passEventsToView;
		bool b_ignoreEvents;
		Canvas * m_pCanvas;
};



#endif
