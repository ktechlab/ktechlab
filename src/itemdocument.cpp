/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktlcanvas.h"

#include "canvasitemparts.h"
#include "canvasmanipulator.h"
#include "cells.h"
#include "connector.h"
#include "cnitem.h"
#include "drawpart.h"
#include "node.h"
#include "flowcodedocument.h"
#include "icnview.h"
#include "itemdocumentdata.h"
#include "itemgroup.h"
#include "itemselector.h"
#include "ktechlab.h"
#include "core/ktlconfig.h"
#include "resizeoverlay.h"
#include "simulator.h"

#include <kapplication.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kprinter.h>

#include <qcheckbox.h>
#include <qclipboard.h>
#include <qcursor.h>
#include <qimage.h>
#include <qpaintdevicemetrics.h>
#include <qpicture.h>
#include <qregexp.h> 
#include <qtimer.h>

#include <cassert>

#include "canvastip.h"
#include "connectorline.h"

//BEGIN class ItemDocument
int ItemDocument::m_nextActionTicket = 0;

ItemDocument::ItemDocument( const QString &caption, const char *name)
	: Document( caption, name )
{
	m_queuedEvents = 0;
	m_nextIdNum = 1;
	m_savedState = 0;
	m_currentState = 0;
	m_bIsLoading = false;
	
	m_canvas = new Canvas( this, "canvas" );
	m_canvasTip = new CanvasTip(this,m_canvas);
	m_cmManager = new CMManager(this);
	
	updateBackground();
	
	m_pUpdateItemViewScrollbarsTimer = new QTimer(this);
	connect( m_pUpdateItemViewScrollbarsTimer, SIGNAL(timeout()), this, SLOT(updateItemViewScrollbars()) );
	
	m_pEventTimer = new QTimer(this);
	connect( m_pEventTimer, SIGNAL(timeout()), this, SLOT(processItemDocumentEvents()) );
	
	connect( this, SIGNAL(selectionChanged()), this, SLOT(slotInitItemActions()) );
	
	connect( ComponentSelector::self(),	SIGNAL(itemClicked(const QString& )),	this, SLOT(slotUnsetRepeatedItemId()) );
	connect( FlowPartSelector::self(),	SIGNAL(itemClicked(const QString& )),	this, SLOT(slotUnsetRepeatedItemId()) );
#ifdef MECHANICS
	connect( MechanicsSelector::self(),	SIGNAL(itemClicked(const QString& )),	this, SLOT(slotUnsetRepeatedItemId()) );
#endif

	m_pAlignmentAction = new KActionMenu( i18n("Alignment"), "rightjust", this );
	
	slotUpdateConfiguration();
}

ItemDocument::~ItemDocument()
{
	m_bDeleted = true;
	
//	ItemMap toDelete = m_itemList;

	const ItemMap::iterator end = m_itemList.end();
	for ( ItemMap::iterator it = m_itemList.begin(); it != end; ++it )
		delete *it;
	m_itemList.clear();

	cleanClearStack( m_undoStack );
	cleanClearStack( m_redoStack );

	delete m_cmManager;
	delete m_currentState;
	delete m_canvasTip;
}

void ItemDocument::handleNewView( View * view )
{
	Document::handleNewView(view);
	requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
}

bool ItemDocument::registerItem(QCanvasItem *qcanvasItem)
{
	if (!qcanvasItem) return false;
	
	requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
	
	if(Item *item = dynamic_cast<Item*>(qcanvasItem) )
	{
		m_itemList[ item->id() ] = item;
		connect( item, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()) );
		itemAdded(item);
		return true;
	}
	
	return false;
}

void ItemDocument::slotSetDrawAction( int drawAction )
{
	m_cmManager->setDrawAction(drawAction);
}

void ItemDocument::cancelCurrentOperation()
{
	m_cmManager->cancelCurrentManipulation();
}

void ItemDocument::slotSetRepeatedItemId( const QString &id )
{
	m_cmManager->setCMState( CMManager::cms_repeated_add, true );
	m_cmManager->setRepeatedAddId(id);
}

void ItemDocument::slotUnsetRepeatedItemId()
{
	m_cmManager->setCMState( CMManager::cms_repeated_add, false );
}

void ItemDocument::fileSave()
{
	if ( url().isEmpty() && !getURL(m_fileExtensionInfo) ) return;
	writeFile();
}

void ItemDocument::fileSaveAs()
{
	if ( !getURL(m_fileExtensionInfo) ) return;
	writeFile();
	
	// Our modified state may not have changed, but we emit this to force the
	// main window to update our caption.
	emit modifiedStateChanged();
}

void ItemDocument::writeFile()
{
	ItemDocumentData data( type() );
	data.saveDocumentState(this);
	
	if ( data.saveData(url()) )
	{
		m_savedState = m_currentState;
		setModified(false);
	}
}


bool ItemDocument::openURL( const KURL &url )
{
	ItemDocumentData data( type() );
	
	if ( !data.loadData(url) )
		return false;
	
	// Why do we stop simulating while loading a document?
	// Crash possible when loading a circuit document, and the Qt event loop is
	// reentered (such as when a PIC component pops-up a message box), which
	// will then call the Simulator::step function, which might use components
	// that have not fully initialized themselves.
	
	m_bIsLoading = true;
	bool wasSimulating = Simulator::self()->isSimulating();
	Simulator::self()->slotSetSimulating( false );
	data.restoreDocument(this);
	Simulator::self()->slotSetSimulating( wasSimulating );
	m_bIsLoading = false;
	
	setURL(url);
	clearHistory();
	m_savedState = m_currentState;
	setModified(false);
	
	if ( FlowCodeDocument *fcd = dynamic_cast<FlowCodeDocument*>(this) )
	{
		// We need to tell all pic-depedent components about what pic type is in use
		emit fcd->picTypeChanged();
	}
	
	requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems );
	
	// Load Z-position info
	m_zOrder.clear();
	ItemMap::iterator end = m_itemList.end();
	for ( ItemMap::iterator it = m_itemList.begin(); it != end; ++it )
	{
		if ( !*it || (*it)->parentItem() )
			continue;
		
		m_zOrder[(*it)->baseZ()] = *it;
	}
	slotUpdateZOrdering();
	
	return true;
}

void ItemDocument::print()
{
	static KPrinter * printer = new KPrinter;
	
	if ( ! printer->setup( KTechlab::self() ) )
		return;
	
	// setup the printer.  with Qt, you always "print" to a
	// QPainter.. whether the output medium is a pixmap, a screen,
	// or paper
	QPainter p;
	p.begin( printer );
	
	// we let our view do the actual printing
	QPaintDeviceMetrics metrics( printer );
	
	// Round to 16 so that we cut in the middle of squares
	int w = metrics.width();
	w = (w & 0xFFFFFFF0) + ((w << 1) & 0x10);

	int h = metrics.height();
	h = (h & 0xFFFFFFF0) + ((h << 1) & 0x10);

	p.setClipping( true );
	p.setClipRect( 0, 0, w, h, QPainter::CoordPainter );
	
	// Send off the painter for drawing
	m_canvas->setBackgroundPixmap( 0 );
	
	QRect bounding = canvasBoundingRect();
	unsigned int rows = (unsigned) std::ceil( double( bounding.height() ) / double( h ) );
	unsigned int cols = (unsigned) std::ceil( double( bounding.width() ) / double( w ) );
	int offset_x = bounding.x();
	int offset_y = bounding.y();
	
	for ( unsigned row = 0; row < rows; ++row )
	{
		for ( unsigned col = 0; col < cols; ++col )
		{
			if ( row != 0 || col != 0 )
				printer->newPage();
			
			QRect drawArea( offset_x + (col * w), offset_y + (row * h), w, h );
			m_canvas->drawArea( drawArea, & p );
			
			p.translate( -w, 0 );
		}
		p.translate( w * cols, -h );
	}
	
	updateBackground();
	
	// and send the result to the printer
	p.end();
}

void ItemDocument::requestStateSave( int actionTicket )
{
	if ( m_bIsLoading ) return;
	
	cleanClearStack( m_redoStack );

	if ( (actionTicket >= 0) && (actionTicket == m_currentActionTicket) )
	{
		delete m_currentState;
		m_currentState = 0;
	}
	
	m_currentActionTicket = actionTicket;
	
	//FIXME: it is possible, that we push something here, also nothing has changed, yet.
	// to reproduce do:
	// 1. select an item -> something is pushed onto undoStack, but nothing changed
	// 2. select Undo -> pushed on redoStack, pop from undoStack
	// 3. deselect item -> there is still something on the redoStack
	//
	// this way you can fill up the redoStack, as you like :-/
	if (m_currentState)
		m_undoStack.push(m_currentState);
	
	m_currentState = new ItemDocumentData( type() );
	m_currentState->saveDocumentState(this);
	
	if (!m_savedState)
		m_savedState = m_currentState;
	
	setModified( m_savedState != m_currentState );
	
	emit undoRedoStateChanged();
	
	//FIXME To resize undo queue, have to pop and push everything
	// In Qt4 QStack is used and QStack inherits QVector, that should
	// make it a bit more easy
	int maxUndo = KTLConfig::maxUndo();
	if ( maxUndo <= 0 || m_undoStack.count() < (unsigned)maxUndo )
		return;
	IDDStack tempStack;
	int pushed = 0;
	while ( !m_undoStack.isEmpty() && pushed < maxUndo ) {
		tempStack.push( m_undoStack.pop() );
		pushed++;
	}
	cleanClearStack( m_undoStack );
	while ( !tempStack.isEmpty() )
		m_undoStack.push( tempStack.pop() );
}

void ItemDocument::cleanClearStack( IDDStack &stack )
{
	while ( !stack.isEmpty() )
	{
		ItemDocumentData * idd = stack.pop();
		if ( m_currentState != idd )
			delete idd;
	}
}

void ItemDocument::clearHistory()
{
	cleanClearStack( m_undoStack );
	cleanClearStack( m_redoStack );
	delete m_currentState;
	m_currentState = 0;
	requestStateSave();
}

bool ItemDocument::isUndoAvailable() const
{
	return !m_undoStack.isEmpty();
}

bool ItemDocument::isRedoAvailable() const
{
	return !m_redoStack.isEmpty();
}

void ItemDocument::undo()
{
	ItemDocumentData *idd = m_undoStack.pop();
	if (!idd) return;

	if (m_currentState) m_redoStack.push(m_currentState);

	idd->restoreDocument(this);
	m_currentState = idd;

	setModified( m_savedState != m_currentState );
	emit undoRedoStateChanged();
}

void ItemDocument::redo()
{
	ItemDocumentData *idd = m_redoStack.pop();
	if (!idd) return;
	
	if (m_currentState)
		m_undoStack.push(m_currentState);
	
	idd->restoreDocument(this);
	m_currentState = idd;
	
	setModified( m_savedState != m_currentState );
	emit undoRedoStateChanged();
}

void ItemDocument::cut()
{
    copy();
    deleteSelection();
}

void ItemDocument::paste()
{
	QString xml = KApplication::clipboard()->text( QClipboard::Clipboard );
	if ( xml.isEmpty() )
		return;
	
	unselectAll();
	
	ItemDocumentData data( type() );
	
	if ( !data.fromXML(xml) )
		return;
	
	data.generateUniqueIDs(this);
//	data.translateContents( 64, 64 );
	data.mergeWithDocument( this, true );
	
	// Get rid of any garbage that shouldn't be around / merge connectors / etc
	flushDeleteList();

	requestStateSave();
}


Item *ItemDocument::itemWithID( const QString &id )
{
	if ( m_itemList.contains( id ) )
		return m_itemList[id];
	else	return 0;
}


void ItemDocument::unselectAll()
{
	selectList()->removeAllItems();
}


void ItemDocument::select( QCanvasItem * item )
{
	if (!item) return;

	item->setSelected( selectList()->contains( item ) || selectList()->addQCanvasItem( item ) );
}


void ItemDocument::select( const QCanvasItemList & list )
{
	const QCanvasItemList::const_iterator end = list.end();
	for ( QCanvasItemList::const_iterator it = list.begin(); it != end; ++it )
		selectList()->addQCanvasItem(*it);
	
	selectList()->setSelected(true);
}


void ItemDocument::unselect( QCanvasItem *qcanvasItem )
{
	selectList()->removeQCanvasItem(qcanvasItem);
	qcanvasItem->setSelected(false);
}


void ItemDocument::slotUpdateConfiguration()
{
	updateBackground();
	m_canvas->setUpdatePeriod( int(1000./KTLConfig::refreshRate()) );
}


QCanvasItem* ItemDocument::itemAtTop( const QPoint &pos ) const
{
	QCanvasItemList list = m_canvas->collisions( QRect( pos.x()-1, pos.y()-1, 3, 3 ) );
	QCanvasItemList::const_iterator it = list.begin();
	const QCanvasItemList::const_iterator end = list.end();

	while ( it != end ) {
		QCanvasItem *item = *it;
		if(	!dynamic_cast<Item*>(item) &&
			!dynamic_cast<ConnectorLine*>(item) &&
			!dynamic_cast<Node*>(item) &&
			!dynamic_cast<Widget*>(item) &&
			!dynamic_cast<ResizeHandle*>(item) )
		{
			++it;
		} else {
			if ( ConnectorLine * l = dynamic_cast<ConnectorLine*>(item) )
				return l->parent();
			
			return item;
		}
	}

	return 0;
}


// these look dangerous., see todo in header file.
void ItemDocument::alignHorizontally( )
{
	selectList()->slotAlignHorizontally();
	if ( ICNDocument *icnd = dynamic_cast<ICNDocument*>(this) )
		icnd->requestRerouteInvalidatedConnectors();
}

void ItemDocument::alignVertically( )
{
	selectList()->slotAlignVertically();
	if ( ICNDocument *icnd = dynamic_cast<ICNDocument*>(this) )
		icnd->requestRerouteInvalidatedConnectors();
}

void ItemDocument::distributeHorizontally( )
{
	selectList()->slotDistributeHorizontally();
	if ( ICNDocument *icnd = dynamic_cast<ICNDocument*>(this) )
		icnd->requestRerouteInvalidatedConnectors();
}

void ItemDocument::distributeVertically( )
{
	selectList()->slotDistributeVertically();
	if ( ICNDocument *icnd = dynamic_cast<ICNDocument*>(this) )
		icnd->requestRerouteInvalidatedConnectors();
}
// ###########################



bool ItemDocument::registerUID( const QString &uid )
{
	return m_idList.insert( uid ).second;
}


void ItemDocument::unregisterUID( const QString & uid )
{
	m_idList.erase(uid);
	m_itemList.remove(uid);
}


QString ItemDocument::generateUID( QString name )
{
	name.remove( QRegExp("__.*") ); // Change 'node__13' to 'node', for example
	QString idAttempt = name;
	
	while ( !registerUID(idAttempt) )
		idAttempt = name + "__" + QString::number(m_nextIdNum++);
	
	return idAttempt;
}

// FIXME: popup menu doesn't seem to work these days. =( 
void ItemDocument::canvasRightClick( const QPoint &pos, QCanvasItem* item )
{
	if (item) {
		if ( dynamic_cast<CNItem*>(item) &&
			!item->isSelected() )
		{
			unselectAll();
			select(item);
		}
	}

	KTechlab::self()->unplugActionList("alignment_actionlist");
	KTechlab::self()->unplugActionList("orientation_actionlist");
	fillContextMenu(pos);

	QPopupMenu *pop = static_cast<QPopupMenu*>(KTechlab::self()->factory()->container("item_popup", KTechlab::self() ));

	if (!pop) return;

	pop->popup(pos);
}


void ItemDocument::fillContextMenu( const QPoint & pos )
{
	Q_UNUSED(pos);
	
	ItemView * activeItemView = dynamic_cast<ItemView*>(activeView());
	if ( !KTechlab::self() || !activeItemView )
		return;
	
	KAction * align_actions[] = { 
		activeItemView->action("align_horizontally"),
		activeItemView->action("align_vertically"),
		activeItemView->action("distribute_horizontally"),
		activeItemView->action("distribute_vertically") };
	
	bool enableAlignment = selectList()->itemCount() > 1;

	if ( !enableAlignment ) return;
	
	for ( unsigned i = 0; i < 4; ++i )
	{
		align_actions[i]->setEnabled(true);
		m_pAlignmentAction->remove( align_actions[i] );
		m_pAlignmentAction->insert( align_actions[i] );
	}
	QPtrList<KAction> alignment_actions;
	alignment_actions.append( m_pAlignmentAction );
	KTechlab::self()->plugActionList( "alignment_actionlist", alignment_actions );
}


void ItemDocument::slotInitItemActions()
{
	ItemView * activeItemView = dynamic_cast<ItemView*>(activeView());
	if ( !KTechlab::self() || !activeItemView )
		return;
	
	KAction * align_actions[] = { 
		activeItemView->action("align_horizontally"),
		activeItemView->action("align_vertically"),
		activeItemView->action("distribute_horizontally"),
		activeItemView->action("distribute_vertically") };
	
	bool enableAlignment = selectList()->itemCount() > 1;
	for ( unsigned i = 0; i < 4; ++i )
		align_actions[i]->setEnabled(enableAlignment);
}


void ItemDocument::updateBackground()
{
	// Also used in the constructor to make the background initially.
	
	// Thoughts.
	// ~The pixmap could be done somehow with 1bpp. It might save some waste
	// I expect it won't hurt for now.
	// ~This is all rather static, only works with square etc... should be no prob. for most uses. IMO. 
	// ~If you want, decide what maximum and minimum spacing should be, then enforce them
	// in the Config (I suppose you can use <max></max> tags?)
	// ~Defaults based on the existing grid background png. It should produce identical results, to your
	// original png.
	
	// **** Below where it says "interval * 10", that decides how big the pixmap will be (always square)
	// Originally I set this to 32, which give 256x256 with 8 spacing, as that was the size of your pixmap
	// Are there any good reasons to make the a certain size? (i.e. big or small ?).
	
	int interval = 8;
	int bigness = interval * 10;
	QPixmap pm( bigness, bigness );
// 	pm.fill( KTLConfig::bgColor() ); // first fill the background colour in
	pm.fill( Qt::white );
	
	if( KTLConfig::showGrid() ){
		QPainter p(&pm); // setup painter to draw on pixmap
		p.setPen( KTLConfig::gridColor() ); // set forecolour
		// note: anything other than 8 borks this
		for( int i = (interval / 2); i < bigness; i+=interval ){
			p.drawLine( 0, i, bigness, i ); // horizontal
			p.drawLine( i, 0, i, bigness ); // vertical
		}
		p.end(); // all done
	}

	pm.setDefaultOptimization( QPixmap::BestOptim );
	m_canvas->setBackgroundPixmap(pm); // and the finale.
}


void ItemDocument::requestCanvasResize()
{
	requestEvent( ItemDocumentEvent::ResizeCanvasToItems );
}


void ItemDocument::requestEvent( ItemDocumentEvent::type type )
{
	m_queuedEvents |= type;
	m_pEventTimer->stop();
	m_pEventTimer->start( 0, true );
}


void ItemDocument::processItemDocumentEvents()
{
	// Copy it incase we have new events requested while doing this...
	unsigned queuedEvents = m_queuedEvents;
	m_queuedEvents = 0;
	
	if ( queuedEvents & ItemDocumentEvent::ResizeCanvasToItems )
		resizeCanvasToItems();
	
	if ( queuedEvents & ItemDocumentEvent::UpdateZOrdering )
		slotUpdateZOrdering();
	
	ICNDocument * icnd = dynamic_cast<ICNDocument*>(this);
	
	if ( icnd && (queuedEvents & ItemDocumentEvent::UpdateNodeGroups) )
		icnd->slotAssignNodeGroups();
	
	if ( icnd && (queuedEvents & ItemDocumentEvent::RerouteInvalidatedConnectors) )
		icnd->rerouteInvalidatedConnectors();
}

void ItemDocument::resizeCanvasToItems()
{
	QRect bound = canvasBoundingRect();
	
	m_viewList.remove((View*)0);
	const ViewList::iterator end = m_viewList.end();
	for ( ViewList::iterator it = m_viewList.begin(); it != end; ++it ) {
		ItemView * iv = static_cast<ItemView*>((View*)*it);
		CVBEditor * cvbEditor = iv->cvbEditor();
		
		QPoint topLeft = iv->mousePosToCanvasPos( QPoint( 0, 0 ) );
		int width = int( cvbEditor->visibleWidth() / iv->zoomLevel() );
		int height = int( cvbEditor->visibleHeight() / iv->zoomLevel() );
		QRect r( topLeft, QSize( width, height ) );
		
		bound |= r;
		
// 		kdDebug() << "r="<<r<<endl;
// 		kdDebug() << "bound="<<bound<<endl;
	}
	
	// Make it so that the rectangular offset is a multiple of 8
	bound.setLeft( bound.left() - (bound.left()%8) );
	bound.setTop( bound.top() - (bound.top()%8) );
	
	m_pUpdateItemViewScrollbarsTimer->start( 10, true );
	
	bool changedSize = canvas()->rect() != bound;
	if ( changedSize ) {
		canvas()->resize( bound );
		requestEvent( ItemDocumentEvent::ResizeCanvasToItems );
	} else if ( ICNDocument * icnd = dynamic_cast<ICNDocument*>(this) ) {
		icnd->createCellMap();
	}
}

void ItemDocument::updateItemViewScrollbars()
{
	int w = canvas()->width();
	int h = canvas()->height();
	
	const ViewList::iterator end = m_viewList.end();
	for ( ViewList::iterator it = m_viewList.begin(); it != end; ++it )
	{
		ItemView * itemView = static_cast<ItemView*>((View*)*it);
		CVBEditor * cvbEditor = itemView->cvbEditor();
		
		cvbEditor->setVScrollBarMode( ((h*itemView->zoomLevel()) > cvbEditor->visibleHeight()) ? QScrollView::AlwaysOn : QScrollView::AlwaysOff );
		cvbEditor->setHScrollBarMode( ((w*itemView->zoomLevel()) > cvbEditor->visibleWidth()) ? QScrollView::AlwaysOn : QScrollView::AlwaysOff );
	}
}

QRect ItemDocument::canvasBoundingRect() const
{
	QRect bound;
	
	// Don't include items used for dragging
	Item *dragItem = 0;
	const ViewList::const_iterator viewsEnd = m_viewList.end();
	for ( ViewList::const_iterator it = m_viewList.begin(); it != viewsEnd; ++it )
	{
		dragItem = (static_cast<ItemView*>((View*)*it))->dragItem();
		if ( dragItem ) break;
	}
	
	const QCanvasItemList allItems = canvas()->allItems();
	const QCanvasItemList::const_iterator end = allItems.end();

	for ( QCanvasItemList::const_iterator it = allItems.begin(); it != end; ++it )
	{
		if( !(*it)->isVisible() ) continue;
		
		if(dragItem ) {
			if(*it == dragItem ) continue;
			
			if(Node *n = dynamic_cast<Node*>(*it) ) {
				if ( n->parentItem() == dragItem )
					continue;
			}
		
			if(GuiPart *gp = dynamic_cast<GuiPart*>(*it) ) {
				if ( gp->parent() == dragItem )
					continue;
			}
		}
		
		bound |= (*it)->boundingRect();
	}
	
	if ( !bound.isNull() )
	{
		bound.setLeft( bound.left() - 16 );
		bound.setTop( bound.top() - 16 );
		bound.setRight( bound.right() + 16 );
		bound.setBottom( bound.bottom() + 16 );
	}
	
	return bound;
}

void ItemDocument::exportToImage()
{
	// scaralously copied from print.
	// this slot is called whenever the File->Export menu is selected,
	// the Export shortcut is pressed or the Export toolbar
	// button is clicked
	
	// widget for the kfiledialog
	// It is the bit that says "Crop circuit?"
	// Okay need to think of something way better to say here.
	// gotme here, KFileDialog makes itself parent so tries to destroy cropCheck when it is deleted.
	// therefore we use a pointer.
	QString cropMessage;

	cropMessage = i18n("Crop image");
	
	QCheckBox *cropCheck = new QCheckBox( cropMessage, KTechlab::self(), "cropCheck" );
	cropCheck->setChecked(true); // yes by default?
	
	// we need an object so we can retrieve which image type was selected by the user
	// so setup the filedialog.
	QString f;
	f = QString("*.png|%1\n*.bmp|%2\n*.svg|%3").arg( i18n("PNG Image") ).arg( i18n("BMP Image") ).arg( i18n("SVG Image") );
	KFileDialog exportDialog(QString::null, f, KTechlab::self(), i18n("Export As Image"), true, cropCheck);
	
	exportDialog.setOperationMode( KFileDialog::Saving );
	// now actually show it
	if ( exportDialog.exec() == QDialog::Rejected )
		return;
	KURL url = exportDialog.selectedURL();

	if ( url.isEmpty() ) return;
	
	if ( QFile::exists( url.path() ) )
	{
		int query = KMessageBox::warningYesNo( KTechlab::self(), i18n( "A file named \"%1\" already exists. " "Are you sure you want to overwrite it?" ).arg( url.fileName() ), i18n( "Overwrite File?" ), i18n( "Overwrite" ), KStdGuiItem::cancel() );
		if ( query == KMessageBox::No ) return;
	}
	
	// with Qt, you always "print" to a
	// QPainter.. whether the output medium is a pixmap, a screen,
	// or paper
	
	// needs to be something like QPicture to do SVG etc...
	
	QRect saveArea;
	QString type;
	QRect cropArea;
	QPaintDevice *outputImage;
	QString filter = exportDialog.currentFilter();
	filter = filter.lower(); // gently soften the appearance of the letters.
	
	// did have a switch here but seems you can't use that on strings
	if ( filter == "*.png") 	type = "PNG";
	else if ( filter == "*.bmp")	type = "BMP";
	else if ( filter == "*.svg" ) {
		KMessageBox::information( NULL, i18n("SVG export is sub-functional"), i18n("Export As Image") );
		type = "SVG";
	}
	// I don't like forcing people to use the right extension (personally)
	// but it is the easiest way to decide image type.
	else {
		KMessageBox::sorry( NULL, i18n("Unknown extension, please select one from the filter list."), i18n("Export As Image") );
		return;
	}

	if ( cropCheck->isChecked() ) {
		cropArea = canvasBoundingRect();
		if ( cropArea.isNull() ) {
			KMessageBox::sorry( 0l, i18n("There is nothing to crop"), i18n("Export As Image") );
			return;
		} else {
			cropArea &= canvas()->rect();
		}
	}

	saveArea = m_canvas->rect();

	if ( type == "PNG" || type == "BMP" )
		outputImage = new QPixmap( saveArea.size() );
	else if ( type == "SVG" ) {
		setSVGExport(true);
		outputImage = new QPicture();
		// svg can't be cropped using the qimage method.
		saveArea = cropArea;
	} else {
		kdWarning() << "Unknown type!" << endl;
		return;
	}
	
	QPainter p(outputImage);
	
	m_canvas->setBackgroundPixmap(QPixmap());
	m_canvas->drawArea( saveArea, &p );
	updateBackground();
	
	p.end();

	bool saveResult;
	
	// if cropping we need to convert to an image,
	// crop, then save.
	if ( cropCheck->isChecked() )
	{
		if( type == "SVG" )
			saveResult = dynamic_cast<QPicture*>(outputImage)->save( url.path(), type);
		else {
			QImage img = dynamic_cast<QPixmap*>(outputImage)->convertToImage();
			img = img.copy(cropArea);
			saveResult = img.save(url.path(),type);
		}
	} else {
		if ( type=="SVG" )
			saveResult = dynamic_cast<QPicture*>(outputImage)->save( url.path(), type );
		else	saveResult = dynamic_cast<QPixmap*>(outputImage)->save( url.path(), type );
	}
	
	//if(saveResult == true)	KMessageBox::information( this, i18n("Sucessfully exported to \"%1\"").arg( url.filename() ), i18n("Image Export") );
	//else KMessageBox::information( this, i18n("Export failed"), i18n("Image Export") );
	
	if ( type == "SVG" ) setSVGExport(false);

	if (saveResult == false)
		KMessageBox::information( KTechlab::self(), i18n("Export failed"), i18n("Image Export") );

	delete outputImage;
}

void ItemDocument::setSVGExport( bool svgExport )
{
	// Find any items and tell them not to draw buttons or sliders
	QCanvasItemList items = m_canvas->allItems();
	const QCanvasItemList::iterator end = items.end();
	for ( QCanvasItemList::Iterator it = items.begin(); it != end; ++it )
	{
		if ( CNItem * cnItem = dynamic_cast<CNItem*>(*it) )
			cnItem->setDrawWidgets(!svgExport);
	}
}

void ItemDocument::raiseZ()
{
	raiseZ( selectList()->items(true) );
}

void ItemDocument::raiseZ( const ItemList & itemList )
{
	if ( m_zOrder.isEmpty() ) slotUpdateZOrdering();
	
	if ( m_zOrder.isEmpty() ) return;
	
	IntItemMap::iterator begin = m_zOrder.begin();
	IntItemMap::iterator previous = m_zOrder.end();
	IntItemMap::iterator it = --m_zOrder.end();
	do {
		Item * previousData = (previous == m_zOrder.end()) ? 0l : previous.data();
		Item * currentData = it.data();
		
		if ( currentData && previousData && itemList.contains(currentData) && !itemList.contains(previousData) )
		{
			previous.data() = currentData;
			it.data() = previousData;
		}
		
		previous = it;
		--it;
	} while ( previous != begin );
	
	slotUpdateZOrdering();
}


void ItemDocument::lowerZ()
{
	lowerZ( selectList()->items(true) );
}

void ItemDocument::lowerZ( const ItemList &itemList )
{
	if ( m_zOrder.isEmpty() ) slotUpdateZOrdering();
	
	if ( m_zOrder.isEmpty() ) return;
	
	IntItemMap::iterator previous = m_zOrder.begin();
	IntItemMap::iterator end = m_zOrder.end();
	for ( IntItemMap::iterator it = m_zOrder.begin(); it != end; ++it )
	{
		Item * previousData = previous.data();
		Item * currentData = it.data();
		
		if ( currentData && previousData && itemList.contains(currentData) && !itemList.contains(previousData) )
		{
			previous.data() = currentData;
			it.data() = previousData;
		}
		
		previous = it;
	}
	
	slotUpdateZOrdering();
}


void ItemDocument::itemAdded( Item * )
{
	requestEvent( ItemDocument::ItemDocumentEvent::UpdateZOrdering );
}

void ItemDocument::slotUpdateZOrdering()
{
	ItemMap toAdd = m_itemList;
	
	IntItemMap newZOrder;
	int atLevel = 0;
	
	IntItemMap::iterator zEnd = m_zOrder.end();
	for ( IntItemMap::iterator it = m_zOrder.begin(); it != zEnd; ++it )
	{	
		Item * item = it.data();
		if (!item) continue;
		
		toAdd.remove( item->id() );
		
		if ( !item->parentItem() && item->isMovable() )
			newZOrder[atLevel++] = item;
	}
	
	ItemMap::iterator addEnd = toAdd.end();
	for ( ItemMap::iterator it = toAdd.begin(); it != addEnd; ++it )
	{
		Item * item = *it;
		if ( item->parentItem() || !item->isMovable() )
			continue;
		
		newZOrder[atLevel++] = item;
	}
	
	m_zOrder = newZOrder;
	
	zEnd = m_zOrder.end();
	for ( IntItemMap::iterator it = m_zOrder.begin(); it != zEnd; ++it )
		it.data()->updateZ( it.key() );
}

void ItemDocument::update( )
{
	ItemMap::iterator end = m_itemList.end();
	for ( ItemMap::iterator it = m_itemList.begin(); it != end; ++it )
	{
		if ( (*it)->contentChanged() )
			(*it)->setChanged();
	}
}

ItemList ItemDocument::itemList( ) const
{
	ItemList l;
	
	ItemMap::const_iterator end = m_itemList.end();
	for ( ItemMap::const_iterator it = m_itemList.begin(); it != end; ++it )
		l << it.data();

	return l;
}
//END class ItemDocument

#include "itemdocument.moc"

