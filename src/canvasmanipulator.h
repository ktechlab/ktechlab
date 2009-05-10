/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CANVAsmANIPULATOR_H
#define CANVAsmANIPULATOR_H

#include "eventinfo.h"

#include <canvas.h>
#include <qguardedptr.h>

class CanvasManipulator;
class Connector;
class CMManager;
class CNItem;
class CNItemGroup;
class EventInfo;
class FlowContainer;
class ICNDocument;
class Item;
class ItemDocument;
class ItemGroup;
class ItemView;
class ManipulatorInfo;
class ManualConnectorDraw;
class MechanicsItem;
class MechanicsGroup;
class MechanicsDocument;
class Node;
class NodeGroup;
class ResizeHandle;

class QCanvas;
class QCanvasItem;
class QCanvasLine;
class QCanvasRectangle;
class QMouseEvent;
class QTimer;
class QWheelEvent;

typedef CanvasManipulator*(*CreateManipulatorPtr)( ItemDocument *, CMManager * );
typedef bool(*AcceptManipulationPtr)( uint eventState, uint cmState, uint itemType, uint cnItemType );
typedef QValueList<NodeGroup*> NodeGroupList;
typedef QValueList<Connector *> ConnectorList;
typedef QValueList<QPoint> QPointList;


class ManipulatorInfo
{
public:
	ManipulatorInfo();
	AcceptManipulationPtr m_acceptManipulationPtr;
	CreateManipulatorPtr m_createManipulatorPtr;
};
typedef QValueList<ManipulatorInfo*> ManipulatorInfoList;


/**
Handles canvas manipulation, such as moving an item or resizing the canvas
@author David Saxton
*/
class CMManager : public QObject
{
Q_OBJECT
public:
	enum EventState
	{
		es_right_click =	1 << 0,
		es_ctrl_pressed =	1 << 1
	};
	enum CMState
	{
		cms_repeated_add =	1 << 0,
		cms_manual_route =	1 << 1,
		cms_draw =		1 << 2
	};
	enum ItemType
	{
		it_none =		1 << 0,
		it_node =		1 << 1,
		it_connector =		1 << 2,
		it_pin =		1 << 3,
		it_canvas_item =	1 << 4,
		it_mechanics_item =	1 << 5,
		it_resize_handle =	1 << 6,
		it_drawpart =		1 << 7
	};

	enum ItemStateInfo
	{
		isi_isMovable = 0x2
	};
	CMManager( ItemDocument *itemDocument );
	~CMManager();
	/**
	 * Called when the user single-clicks the mouse
	 */
	void mousePressEvent( EventInfo eventInfo );
	/**
	 * Called when the user releases the mouse
	 */
	void mouseReleaseEvent( const EventInfo &eventInfo );
	/**
	 * Called when the user double clicks the mouse
	 */
	void mouseDoubleClickEvent( const EventInfo &eventInfo );
	/**
	 * Called when the user moves the mouse
	 */
	void mouseMoveEvent( const EventInfo &eventInfo );
	/**
	 * Called when the user scrolls the mouse
	 */
	void wheelEvent( const EventInfo &eventInfo );
	/**
	 * Set a current CMState to true or false
	 */
	void setCMState( CMState type, bool state );
	/**
	 * Cancels the current manipulation (if there is one)
	 */
	void cancelCurrentManipulation();
	CanvasManipulator * currentManipulator() const { return m_canvasManipulator; }
	void setRepeatedAddId( const QString & repeatedId = QString::null );
	uint cmState() const { return m_cmState; }
	void addManipulatorInfo( ManipulatorInfo *info );
	QString repeatedItemId() const { return m_repeatedItemId; }
	void setDrawAction( int drawAction );
	int drawAction() const { return m_drawAction; }
	
public slots:
	void slotSetManualRoute( bool manualRoute );
	
signals:
	void manualRoutingChanged( bool manualRouting );
	
protected:
	/**
	 * Called when the mouse is moved or released, with the ResizeHandle that
	 * the mouse is currently over (which can be null). Updates which handle is
	 * selected, etc.
	 */
	void updateCurrentResizeHandle( ResizeHandle * mouseOver );
	CanvasManipulator *m_canvasManipulator;
	uint m_cmState;
	QString m_repeatedItemId;
	ItemDocument *p_itemDocument;
	ManipulatorInfoList m_manipulatorInfoList;
	QGuardedPtr<Item> p_lastMouseOverItem; // Pointer to the item where the mouse was last over - this is used to determine when mouse
	QGuardedPtr<ResizeHandle> p_lastMouseOverResizeHandle;
	QGuardedPtr<Item> p_lastItemClicked;
	QTimer *m_allowItemScrollTmr; // When a user scrolls on the canvas, we don't want to stop scrolling when the user gets to (e.g.) a scrollable widget. So this timer prevents scrolling a widget for a few hundred milliseconds after a scroll event if it was initiated over the canvas
	bool b_allowItemScroll; // See above.
	int m_drawAction;
	
private slots:
	void slotAllowItemScroll() { b_allowItemScroll = true; }
};


/**
Abstract class for a "editing operation" on the ICNDocument, such as moving an item or resizing the canvas
@author David Saxton
*/
class CanvasManipulator : public QObject
{
	Q_OBJECT
	
public:
	CanvasManipulator( ItemDocument *itemDocument, CMManager *cmManager );
	virtual ~CanvasManipulator();
	enum Type
	{
		RepeatedItemAdd,
		RightClick,
		AutoConnector,
		ManualConnector,
		ItemMove,
		ItemResize,
		MechItemMove,
		Select,
		CanvasResize,
		ItemDrag,
		Draw
	};
	virtual Type type() const = 0;
	/**
	 * Called when the user single-clicks the mouse
	 * @returns true if the manipulation operation has finished
	 */
	virtual bool mousePressedInitial( const EventInfo &/*info*/ ) { return false; }
	/**
	 * Called when the user single-clicks the mouse after the first time (only
	 * applicable for those operations who are not oneClick
	 * @returns true if the manipulation operation has finished
	 */
	virtual bool mousePressedRepeat( const EventInfo &/*info*/ ) { return false; };
	/**
	 * Called when the user moves the mouse
	 * @returns true if the manipulation operation has finished
	 */
	virtual bool mouseMoved( const EventInfo &/*info*/ ) { return false; };
	/**
	 * Called when the user releases the mouse
	 * @returns true if the manipulation operation has finished
	 */
	virtual bool mouseReleased( const EventInfo &/*info*/ ) { return true; }
	/**
	 * Snaps the point to the 8-sized canvas grid.
	 */
	static QPoint snapPoint( QPoint point );
	
protected slots:
	/**
	 * Called when the working canvas emits a resized signal.
	 */
	virtual void canvasResized( const QRect & oldSize, const QRect & newSize ) { (void)oldSize; (void)newSize; }
	
protected:
	Type m_type;
	EventInfo m_eventInfo;
	QPoint m_prevPos;
	ItemDocument *p_itemDocument;
	ICNDocument *p_icnDocument;
	MechanicsDocument *p_mechanicsDocument;
	QCanvas *p_canvas;
	ItemGroup *p_selectList;
	CNItemGroup *p_cnItemSelectList;
	MechanicsGroup *p_mechItemSelectList;
	CNItem *p_cnItemClickedOn;
	MechanicsItem *p_mechanicsItemClickedOn;
	CMManager *p_cmManager;
};


/**
@author David Saxton
*/
class CMRepeatedItemAdd : public CanvasManipulator
{
public:
	CMRepeatedItemAdd( ItemDocument *itemDocument, CMManager *cmManager );
	virtual ~CMRepeatedItemAdd();
	virtual Type type() const { return RepeatedItemAdd; }
	
	static CanvasManipulator* construct( ItemDocument *itemDocument, CMManager *cmManager );
	static ManipulatorInfo *manipulatorInfo();
	static bool acceptManipulation( uint eventState, uint cmState, uint itemType, uint cnItemType );
	
	virtual bool mousePressedInitial( const EventInfo &info );
	virtual bool mousePressedRepeat( const EventInfo &info );
	virtual bool mouseMoved( const EventInfo &info );
	virtual bool mouseReleased( const EventInfo &info );
	
protected:
};



/**
@author David Saxton
*/
class CMRightClick : public CanvasManipulator
{
public:
	CMRightClick( ItemDocument *itemDocument, CMManager *cmManager );
	virtual ~CMRightClick();
	virtual Type type() const { return RightClick; }
	
	static CanvasManipulator* construct( ItemDocument *itemDocument, CMManager *cmManager );
	static ManipulatorInfo *manipulatorInfo();
	static bool acceptManipulation( uint eventState, uint cmState, uint itemType, uint cnItemType );
	
	virtual bool mousePressedInitial( const EventInfo &info );
	virtual bool mouseMoved( const EventInfo &info );
	virtual bool mouseReleased( const EventInfo &info );
	
protected:
};


/**
@author David Saxton
*/
class ConnectorDraw : public CanvasManipulator
{
	public:
		ConnectorDraw( ItemDocument *itemDocument, CMManager *cmManager );
		virtual ~ConnectorDraw();
		
		/**
		 * Returns the colour used to indicate that the current connection
		 * being drawn is valid. Invalid colour is black.
		 */
		static QColor validConnectionColor();
		
	protected:
		/**
		 * If the node has more than 2 connections, return one of the
		 * connectors
		*/
		Connector * toConnector( Node * node );
		/**
		 * Converts the given qcanvasitem to an appropriate node or connector.
		 * @param posIsExact if true, then only gets an appropriate node or
		 * connector when the to-be end-point of the new connector will coincide
		 * with pos (i.e. auto-connector will call this with posIsExact = false,
		 * and manual-connector will call this with posIsExact = true).
		 */
		void grabEndStuff( QCanvasItem * endItem, const QPoint & pos, bool posIsExact );
		/**
		 * Returns the closest point to the clickPos that is on the given
		 * connector.
		 */
		QPoint toValidPos( const QPoint & clickPos, Connector * clickedConnector ) const;
		
		QGuardedPtr<Node> p_startNode;
		QGuardedPtr<Connector> p_startConnector;
		Node * p_endNode;
		Connector * p_endConnector;
		QPoint startConnectorPoint;
};


/**
@author David Saxton
*/
class CMAutoConnector : public ConnectorDraw
{
public:
	CMAutoConnector( ItemDocument *itemDocument, CMManager *cmManager );
	virtual ~CMAutoConnector();
	virtual Type type() const { return AutoConnector; }
	
	static CanvasManipulator* construct( ItemDocument *itemDocument, CMManager *cmManager );
	static ManipulatorInfo *manipulatorInfo();
	static bool acceptManipulation( uint eventState, uint cmState, uint itemType, uint cnItemType );
	
	virtual bool mousePressedInitial( const EventInfo &info );
	virtual bool mouseMoved( const EventInfo &info );
	virtual bool mouseReleased( const EventInfo &info );
	
protected:
	QCanvasLine *m_connectorLine;
};


/**
@author David Saxton
*/
class CMManualConnector : public ConnectorDraw
{
public:
	CMManualConnector( ItemDocument *itemDocument, CMManager *cmManager );
	virtual ~CMManualConnector();
	virtual Type type() const { return ManualConnector; }
	
	static CanvasManipulator* construct( ItemDocument *itemDocument, CMManager *cmManager );
	static ManipulatorInfo *manipulatorInfo();
	static bool acceptManipulation( uint eventState, uint cmState, uint itemType, uint cnItemType );
	
	virtual bool mousePressedInitial( const EventInfo &info );
	virtual bool mousePressedRepeat( const EventInfo &info );
	virtual bool mouseMoved( const EventInfo &info );
	virtual bool mouseReleased( const EventInfo &info );
	
protected:
	ConnectorList m_fixedRouteConnectors;
	ManualConnectorDraw *m_manualConnectorDraw;
};


/**
@author David Saxton
*/
class CMItemMove : public CanvasManipulator
{
public:
	CMItemMove( ItemDocument *itemDocument, CMManager *cmManager );
	virtual ~CMItemMove();
	virtual Type type() const { return ItemMove; }
	
	static CanvasManipulator* construct( ItemDocument *itemDocument, CMManager *cmManager );
	static ManipulatorInfo *manipulatorInfo();
	static bool acceptManipulation( uint eventState, uint cmState, uint itemType, uint cnItemType );
	
	virtual bool mousePressedInitial( const EventInfo &info );
	virtual bool mouseMoved( const EventInfo &info );
	virtual bool mouseReleased( const EventInfo &info );
	virtual bool mousePressedRepeat( const EventInfo & info );
	
protected:
	virtual void canvasResized( const QRect & oldSize, const QRect & newSize );
	void scrollCanvasToSelection();
	
	QPoint m_prevSnapPoint;
	bool m_bItemsSnapToGrid; ///< true iff selection contains CNItems
	int m_dx;
	int m_dy;
	ConnectorList m_translatableConnectors;
	NodeGroupList m_translatableNodeGroups;
	FlowContainer *p_flowContainerCandidate;
};


/**
@author David Saxton
*/
class CMItemResize : public CanvasManipulator
{
public:
	CMItemResize( ItemDocument *itemDocument, CMManager *cmManager );
	virtual ~CMItemResize();
	virtual Type type() const { return ItemResize; }
	
	static CanvasManipulator* construct( ItemDocument *itemDocument, CMManager *cmManager );
	static ManipulatorInfo *manipulatorInfo();
	static bool acceptManipulation( uint eventState, uint cmState, uint itemType, uint cnItemType );
	
	virtual bool mousePressedInitial( const EventInfo &info );
	virtual bool mouseMoved( const EventInfo &info );
	virtual bool mouseReleased( const EventInfo &info );
	
protected:
	ResizeHandle *p_resizeHandle;
	double m_rh_dx;
	double m_rh_dy;
};


/**
@author David Saxton
*/
class CMMechItemMove : public CanvasManipulator
{
public:
	CMMechItemMove( ItemDocument *itemDocument, CMManager *cmManager );
	virtual ~CMMechItemMove();
	virtual Type type() const { return MechItemMove; }
	
	static CanvasManipulator* construct( ItemDocument *itemDocument, CMManager *cmManager );
	static ManipulatorInfo *manipulatorInfo();
	static bool acceptManipulation( uint eventState, uint cmState, uint itemType, uint cnItemType );
	
	virtual bool mousePressedInitial( const EventInfo &info );
	virtual bool mouseMoved( const EventInfo &info );
	virtual bool mouseReleased( const EventInfo &info );
	
protected:
	uint m_prevClickedOnSM; // Previous select mode of the item that was clicked on
};


/**
@author David Saxton
*/
class SelectRectangle
{
	public:
		SelectRectangle( int x, int y, int w, int h, QCanvas *qcanvas );
		~SelectRectangle();
		
		void setSize( int w, int h );
		QCanvasItemList collisions();
		
	protected:
		QCanvasLine *m_topLine;
		QCanvasLine *m_rightLine;
		QCanvasLine *m_bottomLine;
		QCanvasLine *m_leftLine;
		const int m_x;
		const int m_y;
		int m_w;
		int m_h;
		int m_prevCollisions_w;
		int m_prevCollisions_h;
		QCanvasItemList m_prevCollisions;
};


/**
@author David Saxton
*/
class CMSelect : public CanvasManipulator
{
public:
	CMSelect( ItemDocument *itemDocument, CMManager *cmManager );
	virtual ~CMSelect();
	virtual Type type() const { return Select; }
	
	static CanvasManipulator* construct( ItemDocument *itemDocument, CMManager *cmManager );
	static ManipulatorInfo *manipulatorInfo();
	static bool acceptManipulation( uint eventState, uint cmState, uint itemType, uint cnItemType );
	
	virtual bool mousePressedInitial( const EventInfo &info );
	virtual bool mouseMoved( const EventInfo &info );
	virtual bool mouseReleased( const EventInfo &info );
	
protected:
	SelectRectangle *m_selectRectangle;
};


/**
@author David Saxton
*/
class CMItemDrag : public CanvasManipulator
{
public:
	CMItemDrag( ItemDocument *itemDocument, CMManager *cmManager );
	virtual ~CMItemDrag();
	virtual Type type() const { return ItemDrag; }
	
	static CanvasManipulator* construct( ItemDocument *itemDocument, CMManager *cmManager );
	static ManipulatorInfo *manipulatorInfo();
	static bool acceptManipulation( uint eventState, uint cmState, uint itemType, uint cnItemType );
	
	virtual bool mousePressedInitial( const EventInfo &info );
	virtual bool mouseMoved( const EventInfo &info );
	virtual bool mouseReleased( const EventInfo &info );
	
protected:
	bool b_dragged;
};



/**
@author David Saxton
A QCanvasEllipse that uses a pen (not a brush) to paint
*/
class CanvasEllipseDraw : public QCanvasEllipse
{
	public:
		CanvasEllipseDraw( int x, int y, QCanvas * canvas );
		
	protected:
		virtual void drawShape( QPainter & p );
};


/**
@author David Saxton
*/
class CMDraw : public CanvasManipulator
{
	public:
		CMDraw( ItemDocument *itemDocument, CMManager *cmManager );
		virtual ~CMDraw();
		virtual Type type() const { return Draw; }
	
		static CanvasManipulator* construct( ItemDocument *itemDocument, CMManager *cmManager );
		static ManipulatorInfo *manipulatorInfo();
		static bool acceptManipulation( uint eventState, uint cmState, uint itemType, uint cnItemType );
	
		virtual bool mousePressedInitial( const EventInfo &info );
		virtual bool mouseMoved( const EventInfo &info );
		virtual bool mouseReleased( const EventInfo &info );
	
	protected:
		QCanvasRectangle * m_pDrawRectangle;
		CanvasEllipseDraw * m_pDrawEllipse;
		QCanvasLine * m_pDrawLine;
};


/**
@author David Saxton
*/
class ManualConnectorDraw
{
	public:
		ManualConnectorDraw( ICNDocument *_icnDocument, const QPoint &initialPos );
		virtual ~ManualConnectorDraw();
		
		/**
		 * Called when the mouse is moved.
		 * Normally will do something like updating the connector route
		 */
		void mouseMoved( const QPoint &pos );
		/**
		 * Called when the user clicks the mouse. If the connector finishes on a
		 * valid QCanvasItem (Node or Connetor), then this is returned. Otherwise,
		 * null is returned.
		 */
		QCanvasItem * mouseClicked( const QPoint &pos );
		/**
		 * Returns the list of points that define the manual connection route
		 */
		QPointList pointList();
		/**
		 * Sets the colour used to draw the connection lines.
		 */
		void setColor( const QColor & color );

	protected:
		void updateConnectorEnds();
	
		QValueList<QCanvasLine*> m_connectorLines;
		ICNDocument *icnDocument;
	
		bool b_currentVertical;
		bool b_orientationDefined;
	
		QPoint m_initialPos;
		QPoint m_previousPos;
		QPoint m_currentPos;
	
		QCanvasLine *m_currentCon;
		QCanvasLine *m_previousCon;
	
		// The first item that we clicked on
		QCanvasItem *p_initialItem;
	
		QColor m_color;
};


#endif
