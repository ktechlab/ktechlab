/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ITEMDOCUMENT_H
#define ITEMDOCUMENT_H

#include <set>
#include <document.h>
#include <canvas.h>
#include "canvasitems.h"

#include <qmap.h>
#include <qstack.h>
// #include <q3valuevector.h>

class Canvas;
class CanvasTip;
class Connector;
class CMManager;
class ECNode;
class Item;
class ItemDocumentData;
class ItemGroup;
class KTechlab;
class Operation;

class KActionMenu;
class KtlQCanvasItem;

typedef QStack<ItemDocumentData*> IDDStack;
typedef QPointer<Item> GuardedItem;
typedef QMap< int, GuardedItem > IntItemMap;
typedef QMap< QString, Item* > ItemMap;
typedef QList<GuardedItem> ItemList;
typedef QList<QPoint> QPointList;

/**
@author David Saxton
*/
class ItemDocument : public Document
{
	Q_OBJECT

	friend class KtlTestsAppFixture;

	public:
		ItemDocument( const QString &caption, const char *name = nullptr );
		~ItemDocument() override;

		class Z
		{
			public:
				enum
				{
					Select			= 10000000,
					Connector		= 20000000,
					Item			= 30000000,
					RaisedItem		= 40000000,
					ResizeHandle 		= 50000000,
					Tip			= 60000000,
					ConnectorCreateLine	= 70000000,

					// How much "Z" separates items stacked on each other
					DeltaItem		= 10000
				};
		};

		/**
		 * Some things (such as the canvas getting resized, connectors being
		 * invalidated, need to be done after editing operations have finished,
		 * etc, and they also need to be done in the order given in the
		 * enumeration below.
		 */
		class ItemDocumentEvent
		{
			public: enum type
			{
				ResizeCanvasToItems		= 1 << 0,
				UpdateNodeGroups		= 1 << 1,
				RerouteInvalidatedConnectors	= 1 << 2,
				UpdateZOrdering			= 1 << 3
			};
		};

		void fileSave() override;
		void fileSaveAs() override;
		void print() override;
		bool openURL( const KUrl &url ) override;
		/**
		 * Attempt to register the item, returning true iff successful
		 */
		virtual bool registerItem( KtlQCanvasItem *qcanvasItem );
		/**
		 * Will attempt to create an item with the given id at position p. Some item
		 * (such as PIC/START) have restrictions, and can only have one instance of
		 * themselves on the canvas, and adds the operation to the undo list
		 */
		virtual Item* addItem( const QString &id, const QPoint &p, bool newItem ) = 0;
		/**
		 * @returns A pointer to the canvas
		 */
		Canvas *canvas() const { return m_canvas; }
		/**
		 * Attemtps to register a unique id for the canvas view of an item on the
		 * canvas. If the id does not already exist, will return true; otherwise
		 * the function will return false.
		 */
		bool registerUID( const QString & uid );
		/**
		 * Generates a unique id based on a possibly unique component name.
		 */
		QString generateUID( QString name );
		/**
		 * Unlists the given id as one that is used.
		 * @see registerUID
		 */
		virtual void unregisterUID( const QString & uid );
		/**
		 * @return Whether or not the item is valid; i.e. is appropriate to the
		 * document being edited, and does not have other special restrictions
		 * on it (such as only allowing one instance of the Start part in
		 * FlowCode).
		 */
		virtual bool isValidItem( Item *item ) = 0;
		/**
		 * @return Whether or not the item is valid; i.e. is appropriate to the
		 * document being edited, and does not have other special restrictions
		 * on it (such as only allowing one instance of the Start part in
		 * FlowCode).
		 */
		virtual bool isValidItem( const QString &itemId ) = 0;
		/**
		 * Increases the "height" of the given list of items by "one".
		 */
		void raiseZ( const ItemList & itemList );
		/**
		 * Decreases the "height" of the given list of items by "one".
		 */
		void lowerZ( const ItemList & itemList );
		/**
		 * @return ItemGroup that is used as the select list for this document.
		 */
		virtual ItemGroup *selectList() const = 0;
		/**
		 * Deselects any currently selected items
		 */
		void unselectAll();
		/**
		 * Select a list of KtlQCanvasItem's
		 */
		void select( const KtlQCanvasItemList & list );
		/**
		 * Select a KtlQCanvasItem
		 */
		void select( KtlQCanvasItem * item );
		/**
		 * Unselects the item
		 */
		void unselect( KtlQCanvasItem *qcanvasItem );
		/**
		 * Deletes anything waiting to be deleted.
		 */
		virtual void flushDeleteList() = 0;
		/**
		 * Returns a rubber-band rectangle that contains all of the items on the
		 * canvas, padded out by a small border.
		 */
		QRect canvasBoundingRect() const;
		/**
		 * Returns a pointer to a Item on the canvas with the given id,
		 * or nullptr if no such Item exists.
		 */
		Item* itemWithID( const QString & );
		/**
		 * Returns true if the user can perform an undo action
		 * (i.e. the undo stack is not empty)
		 */
		bool isUndoAvailable() const override;
		/**
		 * Returns true if the user can perform an redo action
		 * (i.e. the redo stack is not empty)
		 */
		bool isRedoAvailable() const override;
		/**
		 * Returns the top item at point (x, y), or nullptr if there is no item there
		 */
		KtlQCanvasItem* itemAtTop( const QPoint &pos ) const ;
		/**
		 * Called when the canvas is clicked on with the right mouse button.
		 * Popups up a menu for editing operations
		 */
		virtual void canvasRightClick( const QPoint &pos, KtlQCanvasItem* item );
		/**
		 * List of items in the ItemDocument
		 */
		ItemList itemList() const;
		/**
		 * Set the given KtlQCanvasItem (which will attempt to be casted to known
		 * items to be deleted.
		 */
		virtual void appendDeleteList( KtlQCanvasItem * ) = 0;
		/**
		 * Save the current state of the document to the undo/redo history.
		 * @param actionTicket if this is non-negative, and the last state save
		 * also had the same actionTicket, then the next state save will
		 * overwrite the previous state save.
		 * @see getActionTicket
		 */
		void requestStateSave( int actionTicket = -1 );

		/**
		 * Clears the undo / redo history
		 */
		void clearHistory();
		/**
		 * Requests an event to be done after other stuff (editing, etc) is finished.
		 */
		void requestEvent( ItemDocumentEvent::type type );
		/**
		 * Called from Canvas (when KtlQCanvas::advance is called).
		 */
		virtual void update();

	/**
	 * Returns a unique id, for use in requestStateSave
	 */
	int getActionTicket() const { return m_nextActionTicket++; }

public slots:
		void undo() override;
		void redo() override;
		void cut() override;
		void paste() override;
		/**
		 * Ask the canvas to be resized to the current items on the canvas.
		 */
		void requestCanvasResize();
		/**
		 * Selects everything in the view.
		 */
		void selectAll() override = 0;
		/**
		 * Increases the "height" of the selected items.
		 */
		void raiseZ();
		/**
		 * Decreases the "height" of the selected items.
		 */
		void lowerZ();
		/**
		 * Brings up a file dialog requesting the location of the file to export
		 * to, and then exports an image of the canvas.
		 */
		void exportToImage();
    protected:
        void exportToImageDraw( const QRect & saveArea, QPaintDevice &pDev);
    public slots:
		/**
		 * Deletes whatever is selected.
		 */
		virtual void deleteSelection() {};
		/**
		 * Called when the user presses Escape (or similar)
		 */
		void cancelCurrentOperation();
		/**
		 * Sets the y-positions of the selected items to the average of the
		 * initial y-positions.
		 */

// TODO: decide whether these should be moved to ICNdocument...
		void alignHorizontally();
		/**
		 * Sets the x-positions of the selected items to the average of the
		 * initial x-positions.
		 */
		void alignVertically();
		/**
		 * Averages out the horizontal spacing between the selected items.
		 */
		void distributeHorizontally();
		/**
		 * Averages out the vertical spacing between the selected items.
		 */
		void distributeVertically();
		/**
		 * Adds an items not in the Z ordering to the ordering, and removes any
		 * items from the Z ordering if they have parents. Then, calls all items
		 * found in the ordering to tell them their Z position.
		 */
// ##################


		void slotUpdateZOrdering();
		/**
		 * Call this with ItemDocument::DrawAction to start drawing the given thing
		 */
		void slotSetDrawAction( QAction *selected );
		/**
		 * Sets the editing mode to repeatedly creating a CNItem
		 * with the given id. Usually called when the user double-clicks on
		 * the component box.
		 */
		void slotSetRepeatedItemId( const QString &id );
		/**
		 * Unsets the editing mode from repeatedly creating a CNItem
		 */
		void slotUnsetRepeatedItemId();
		/**
		 * Called when the user changes the configuration.
		 * This, for example, will tell the CNItems on the canvas to update
		 * their configuration.
		 */
		void slotUpdateConfiguration() override;
		/**
		 * Enables / disables / selects various actions depending on
		 * what is selected or not.
		 */
		virtual void slotInitItemActions();
		/**
		 * Process queued events (see ItemDocument::ItemDocumentEvent).
		 */
		void processItemDocumentEvents();

	signals:
		/**
		 * Emitted when the selection changes.
		 */
		void selectionChanged();

	protected slots:
		/**
		 * Called after the canvas is resized to set the scrollbars of the
		 * ItemViews to either always show or always hidden.
		 */
		void updateItemViewScrollbars();

	protected:
		/**
		 * Called from registerItem when a new item is added.
		 */
		virtual void itemAdded( Item * item );
		void handleNewView( View *view ) override;
		/**
		 * Set to true to remove buttons and grid and so on from the canvas, set false to put them back
		 */
		void setSVGExport( bool svgExport );
		void writeFile();
		/**
		 * Reinherit this if you want to add any options to the right-click context
		 */
		virtual void fillContextMenu( const QPoint & pos );
		/**
		 * Reads the background settings (grid-colour, underlying colour) from the Config settings,
		 * and generates the background pixmap from those settings
		 */
		void updateBackground();
		/**
		 * Sets the canvas size to both (a) containing all items present on the
		 * canvas, and (b) no smaller than the smallest view of the canvas. This
		 * function should only be called by processItemDocumentEvents - a resize
		 * request must be made with requestEvent.
		 */
		void resizeCanvasToItems();

		Canvas		*m_canvas;

		CMManager	*m_cmManager;
		CanvasTip	*m_canvasTip;

		ItemList	 m_itemDeleteList;
		ItemMap		 m_itemList;

		QString		 m_fileExtensionInfo; // For displaying in the save file dialog



private:
	/**
	 * This clears a given stack and deletes all pointers, but the one to m_currentState.
	 */
	void cleanClearStack( IDDStack &stack );

	static int	  m_nextActionTicket;

	unsigned	  m_queuedEvents; // OR'ed together list of ItemDocumentEvent::type
	unsigned	  m_nextIdNum;
	int		  m_currentActionTicket;
	bool		  m_bIsLoading;

	ItemDocumentData *m_currentState;
	ItemDocumentData *m_savedState; // Pointer to the document data that holds the state when it saved

	KActionMenu	 *m_pAlignmentAction;

	IntItemMap	  m_zOrder;

	std::set<QString> m_idList; // used to ensure unique IDs to try to make sure save files are valid.

	QTimer		*m_pEventTimer;
	QTimer		*m_pUpdateItemViewScrollbarsTimer;

	IDDStack	 m_undoStack;
	IDDStack	 m_redoStack;

	friend class ICNView;
	friend class ItemView;
};


/**
@author David Saxton
*/
class Canvas : public KtlQCanvas
{
	Q_OBJECT
	public:
		Canvas( ItemDocument *itemDocument, const char * name = nullptr );

		/**
		 * Sets a message to be displayed on the canvas for a brief period of
		 * time. If this is called with an empty message, then any existing
		 * message will be removed.
		 */
		void setMessage( const QString & message );
		void update() override;
		void resize( const QRect & size ) override;

	signals:
		/**
		 * Emitted when the canvas rectangle-size changes.
		 */
		void resized( const QRect & oldSize, const QRect & newSize );

	public slots:
		void slotSetAllChanged() { setAllChanged(); }

	protected:
		void drawBackground ( QPainter & painter, const QRect & clip ) override;
		void drawForeground ( QPainter & painter, const QRect & clip ) override;

		ItemDocument *p_itemDocument;

		QString m_message;
		QTimer * m_pMessageTimeout;
};


/**
@author David Saxton
*/
class CanvasTip : public KtlQCanvasRectangle
{
	public:
		CanvasTip( ItemDocument *itemDocument, KtlQCanvas *qcanvas );
		~CanvasTip() override;

		void displayVI( ECNode *node, const QPoint &pos );
		void displayVI( Connector *connector, const QPoint &pos );

	protected:
		void draw( QPainter &p ) override;
		void setText( const QString & text );
		bool updateVI();
		void display( const QPoint &pos );
		QString displayText( unsigned num ) const;

		QVector<double> m_v;
		QVector<double> m_i;
		ItemDocument *p_itemDocument;
		QString m_text;
};


#endif
