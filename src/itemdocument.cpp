/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasmanipulator.h"
#include "circuitdocument.h"
#include "connector.h"
#include "cnitem.h"
#include "drawpart.h"
#include "ecnode.h"
#include "flowcodedocument.h"
#include "icnview.h"
#include "itemdocumentdata.h"
#include "itemgroup.h"
#include "itemselector.h"
#include "ktechlab.h"
#include "core/ktlconfig.h"
#include "pin.h"
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
#include <qpainter.h>
#include <qpicture.h>
#include <qregexp.h> 
#include <qsimplerichtext.h>
#include <qtimer.h>

#include <cmath>

//BEGIN class ItemDocument
int ItemDocument::m_nextActionTicket = 0;

ItemDocument::ItemDocument( const QString &caption, KTechlab *ktechlab, const char *name)
	: Document( caption, ktechlab, name)
{
	m_queuedEvents = 0;
	p_ktechlab = ktechlab;
	m_nextIdNum = 1;
	m_savedState = 0;
	m_currentState = 0;
	m_bIsLoading = false;
	
	m_canvas = new Canvas( this, "canvas");
	m_canvasTip = new CanvasTip(this,m_canvas);
	m_cmManager = new CMManager(this);
	m_undoStack.setAutoDelete(true);
	m_redoStack.setAutoDelete(true);
	
	updateBackground();
	m_canvas->resize( 0, 0);
	m_canvas->setDoubleBuffering(true);
	
	m_pEventTimer = new QTimer(this);
	connect( m_pEventTimer, SIGNAL(timeout()), this, SLOT(processItemDocumentEvents()));
	
	connect( this, SIGNAL(itemSelected(Item*)), this, SLOT(slotInitItemActions(Item*)));
	connect( this, SIGNAL(itemUnselected(Item*)), this, SLOT(slotInitItemActions(Item*)));
	
	connect( ComponentSelector::self(),	SIGNAL(itemClicked(const QString&)),		this, SLOT(slotUnsetRepeatedItemId()));
	connect( FlowPartSelector::self(),	SIGNAL(itemClicked(const QString&)),		this, SLOT(slotUnsetRepeatedItemId()));
#ifdef MECHANICS
	connect( MechanicsSelector::self(),	SIGNAL(itemClicked(const QString&)),		this, SLOT(slotUnsetRepeatedItemId()));
#endif

	m_pAlignmentAction = new KActionMenu( i18n("Alignment"), "rightjust", this);
	
	slotUpdateConfiguration();
}

ItemDocument::~ItemDocument()
{
	m_bDeleted = true;
	
	ItemList toDelete = m_itemList;
	const ItemList::iterator end = toDelete.end();
	for( ItemList::iterator it = toDelete.begin(); it != end; ++it)
		delete *it;
	
	delete m_cmManager;
	delete m_currentState;
	delete m_canvasTip;
}

void ItemDocument::handleNewView( View * view)
{
	Document::handleNewView(view);
	requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
}

bool ItemDocument::registerItem( QCanvasItem *qcanvasItem)
{
	if(!qcanvasItem)
		return false;
	
	requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
	
	switch (qcanvasItem->rtti())
	{
		case ItemDocument::RTTI::DrawPart:
		case ItemDocument::RTTI::CNItem:
		{
			Item *item = dynamic_cast<Item*>(qcanvasItem);
			m_itemList.append(item);
			connect( item, SIGNAL(removed(Item*)), this, SLOT(requestRerouteInvalidatedConnectors()));
			connect( item, SIGNAL(selected(Item*,bool)), this, SIGNAL(itemSelected(Item*)));
			connect( item, SIGNAL(unselected(Item*,bool)), this, SIGNAL(itemUnselected(Item*)));
			itemAdded(item);
			return true;
		}
		default:
			return false;
	}
}

void ItemDocument::slotSetDrawAction( int drawAction)
{
	m_cmManager->setDrawAction(drawAction);
}

void ItemDocument::cancelCurrentOperation()
{
	m_cmManager->cancelCurrentManipulation();
}

void ItemDocument::slotSetRepeatedItemId( const QString &id)
{
	m_cmManager->setCMState( CMManager::cms_repeated_add, true);
	m_cmManager->setRepeatedAddId(id);
}

void ItemDocument::slotUnsetRepeatedItemId()
{
	m_cmManager->setCMState( CMManager::cms_repeated_add, false);
}

void ItemDocument::fileSave()
{
	if( url().isEmpty() && !getURL(m_fileExtensionInfo)) return;
	writeFile();
}

void ItemDocument::fileSaveAs()
{
	if( !getURL(m_fileExtensionInfo)) return;
	writeFile();
	
	// Our modified state may not have changed, but we emit this to force the
	// main window to update our caption.
	emit modifiedStateChanged();
}

void ItemDocument::writeFile()
{
	ItemDocumentData data( type());
	data.saveDocumentState(this);
	
	if( data.saveData(url()))
	{
		m_savedState = m_currentState;
		setModified(false);
	}
}

bool ItemDocument::openURL( const KURL &url)
{
	ItemDocumentData data( type());

	if(!data.loadData(url)) return false;

	// Why do we stop simulating while loading a document?
	// Crash possible when loading a circuit document, and the Qt event loop is
	// reentered (such as when a PIC component pops-up a message box), which
	// will then call the Simulator::step function, which might use components
	// that have not fully initialized themselves.

	m_bIsLoading = true;
	bool wasSimulating = Simulator::self()->isSimulating();
	Simulator::self()->slotSetSimulating( false);
	data.restoreDocument(this);
	Simulator::self()->slotSetSimulating( wasSimulating);
	m_bIsLoading = false;

	setURL(url);
	clearHistory();
	m_savedState = m_currentState;
	setModified(false);

	if( FlowCodeDocument *fcd = dynamic_cast<FlowCodeDocument*>(this))
	{
		// We need to tell all pic-depedent components about what pic type is in use
		emit fcd->picTypeChanged();
	}
	
	requestEvent( ItemDocument::ItemDocumentEvent::ResizeCanvasToItems);
	
	// Load Z-position info
	m_zOrder.clear();
	ItemList::iterator end = m_itemList.end();
	for( ItemList::iterator it = m_itemList.begin(); it != end; ++it)
	{
		if( !*it || (*it)->parentItem())
			continue;
		
		m_zOrder[(*it)->baseZ()] = *it;
	}
	slotUpdateZOrdering();

	return true;
}

void ItemDocument::print()
{
	static KPrinter * printer = new KPrinter;
	
	if(! printer->setup( p_ktechlab)) return;

	// setup the printer.  with Qt, you always "print" to a
	// QPainter.. whether the output medium is a pixmap, a screen,
	// or paper
	QPainter p;
	p.begin( printer);

	// we let our view do the actual printing
	QPaintDeviceMetrics metrics( printer);

	// Round to 16 (= 2 * 8) so that we cut in the middle of squares
	int w = 16*int(metrics.width()/16);
	int h = 16*int(metrics.height()/16);

	p.setClipping( true);
	p.setClipRect( 0, 0, w, h, QPainter::CoordPainter);

	// Send off the painter for drawing
	m_canvas->setBackgroundPixmap( 0);

	QRect bounding = canvasBoundingRect();
	unsigned int rows = (unsigned) std::ceil( double( bounding.height()) / double( h));
	unsigned int cols = (unsigned) std::ceil( double( bounding.width()) / double( w));
	int offset_x = bounding.x();
	int offset_y = bounding.y();

	for( unsigned row = 0; row < rows; ++row)
	{
		for( unsigned col = 0; col < cols; ++col)
		{
			if( row != 0 || col != 0)
				printer->newPage();
			
			QRect drawArea( offset_x + (col * w), offset_y + (row * h), w, h);
			m_canvas->drawArea( drawArea, & p);
			
			p.translate( -w, 0);
		}
		p.translate( w * cols, -h);
	}

	updateBackground();

	// and send the result to the printer
	p.end();
}

void ItemDocument::requestStateSave( int actionTicket)
{
	if( m_bIsLoading) return;

	m_redoStack.clear();

	if( (actionTicket >= 0) && (actionTicket == m_currentActionTicket)) {
		delete m_currentState;
		m_currentState = 0;
	}

	m_currentActionTicket = actionTicket;

	if(m_currentState)
		m_undoStack.push(m_currentState);

	m_currentState = new ItemDocumentData( type());
	m_currentState->saveDocumentState(this);

	if(!m_savedState) m_savedState = m_currentState;

	setModified( m_savedState != m_currentState);

	emit undoRedoStateChanged();

	//FIXME To resize undo queue, have to pop and push everything
	int maxUndo = KTLConfig::maxUndo();
	if( maxUndo <= 0 || m_undoStack.count() < (unsigned)maxUndo)
		return;
	IDDStack tempStack;
	int pushed = 0;
	while(!m_undoStack.isEmpty() && pushed < maxUndo) {
		tempStack.push( m_undoStack.pop());
		pushed++;
	}

	m_undoStack.clear();
	while(!tempStack.isEmpty()) m_undoStack.push( tempStack.pop());
}

void ItemDocument::clearHistory()
{
	m_undoStack.clear();
	m_redoStack.clear();
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
	if(!idd) return;

	if(m_currentState) m_redoStack.push(m_currentState);

	idd->restoreDocument(this);
	m_currentState = idd;

	setModified( m_savedState != m_currentState);
	emit undoRedoStateChanged();
}

void ItemDocument::redo()
{
	ItemDocumentData *idd = m_redoStack.pop();

	if(!idd) return;

	if(m_currentState) m_undoStack.push(m_currentState);

	idd->restoreDocument(this);
	m_currentState = idd;

	setModified( m_savedState != m_currentState);
	emit undoRedoStateChanged();
}

void ItemDocument::cut()
{
    copy();
    deleteSelection();
}

void ItemDocument::paste()
{
	QString xml = KApplication::clipboard()->text( QClipboard::Clipboard);
	if( xml.isEmpty()) return;

	unselectAll();

	ItemDocumentData data( type());
	data.fromXML(xml);
	data.generateUniqueIDs(this);
	data.translateContents( 64, 64);
	data.mergeWithDocument( this, true);

	// Get rid of any garbage that shouldn't be around / merge connectors / etc
	flushDeleteList();

	requestStateSave();
}

Item *ItemDocument::itemWithID( const QString &id)
{
	const ItemList::iterator end = m_itemList.end();
	for( ItemList::iterator it = m_itemList.begin(); it != end; ++it)
	{
		if( (*it)->id() == id) return *it;
	}
	return 0;
}

void ItemDocument::unselectAll()
{
	selectList()->removeAllItems();
}

void ItemDocument::select( QCanvasItem * item)
{
	if(!item) return;
	item->setSelected( selectList()->contains( item) || selectList()->addQCanvasItem( item));
}

void ItemDocument::select( const QCanvasItemList & list)
{
	const QCanvasItemList::const_iterator end = list.end();
	for( QCanvasItemList::const_iterator it = list.begin(); it != end; ++it)
		selectList()->addQCanvasItem(*it);
	
	selectList()->setSelected(true);
}

void ItemDocument::unselect( QCanvasItem *qcanvasItem)
{
	selectList()->removeQCanvasItem(qcanvasItem);
	qcanvasItem->setSelected(false);
}

void ItemDocument::slotUpdateConfiguration()
{
	updateBackground();
	m_canvas->setUpdatePeriod( int(1000./KTLConfig::refreshRate()));
}

QCanvasItem* ItemDocument::itemAtTop( const QPoint &pos) const
{
	QCanvasItemList list = m_canvas->collisions( QRect( pos.x()-1, pos.y()-1, 3, 3));

	QCanvasItemList::const_iterator it = list.begin();
	const QCanvasItemList::const_iterator end = list.end();
	while ( it != end) {
		QCanvasItem *item = *it;
		if( item == m_canvasTip ||
				   item->rtti() == QCanvasItem::Rtti_Line ||
				   item->rtti() == QCanvasItem::Rtti_Text ||
				   item->rtti() == QCanvasItem::Rtti_Rectangle) 
		{
			++it;
		} else {
			if( item->rtti() == ItemDocument::RTTI::ConnectorLine)
				return (static_cast<ConnectorLine*>(item))->parent();

			return item;
		}
	}

	return 0;
}

void ItemDocument::alignHorizontally()
{
	selectList()->slotAlignHorizontally();
	if( ICNDocument *icnd = dynamic_cast<ICNDocument*>(this))
		icnd->requestRerouteInvalidatedConnectors();
}

void ItemDocument::alignVertically()
{
	selectList()->slotAlignVertically();
	if( ICNDocument *icnd = dynamic_cast<ICNDocument*>(this))
		icnd->requestRerouteInvalidatedConnectors();
}

void ItemDocument::distributeHorizontally()
{
	selectList()->slotDistributeHorizontally();
	if( ICNDocument *icnd = dynamic_cast<ICNDocument*>(this))
		icnd->requestRerouteInvalidatedConnectors();
}

void ItemDocument::distributeVertically()
{
	selectList()->slotDistributeVertically();
	if( ICNDocument *icnd = dynamic_cast<ICNDocument*>(this))
		icnd->requestRerouteInvalidatedConnectors();
}

bool ItemDocument::registerUID( const QString &UID)
{
	if( m_idList.findIndex(UID) == -1)
	{
		m_idList.append(UID);
		return true;
	}
	
	return false;
}

void ItemDocument::unregisterUID( const QString & uid)
{
	m_idList.remove(uid);
}

QString ItemDocument::generateUID( QString name)
{
	name.remove( QRegExp("__.*")); // Change 'node__13' to 'node', for example
	QString idAttempt = name;
// 	if( idAttempt.find("__") != -1) idAttempt.truncate( idAttempt.find("__"));
	while ( !registerUID(idAttempt)) { idAttempt = name + "__" + QString::number(m_nextIdNum++); }
	
	return idAttempt;
}

void ItemDocument::canvasRightClick( const QPoint &pos, QCanvasItem* item)
{
	if(item) {
		if( item->rtti() == ItemDocument::RTTI::CNItem &&
			!item->isSelected())
		{
			unselectAll();
			select(item);
		}
	}

	p_ktechlab->unplugActionList("alignment_actionlist");
	p_ktechlab->unplugActionList("orientation_actionlist");
	p_ktechlab->unplugActionList("component_actionlist");
	fillContextMenu(pos);

	QPopupMenu *pop = static_cast<QPopupMenu*>(p_ktechlab->factory()->container("item_popup", p_ktechlab));

	if(!pop) return;

	pop->popup(pos);
}


void ItemDocument::fillContextMenu( const QPoint & pos)
{
	Q_UNUSED(pos);
	
	ItemView * activeItemView = dynamic_cast<ItemView*>(activeView());
	if( !p_ktechlab || !activeItemView)
		return;
	
	KAction * align_actions[] = { 
		activeItemView->action("align_horizontally"),
		activeItemView->action("align_vertically"),
		activeItemView->action("distribute_horizontally"),
		activeItemView->action("distribute_vertically") };
	
	bool enableAlignment = selectList()->itemCount() > 1;
	
	if( !enableAlignment)
		return;
	
	for( unsigned i = 0; i < 4; ++i)
	{
		align_actions[i]->setEnabled(true);
		m_pAlignmentAction->remove( align_actions[i]);
		m_pAlignmentAction->insert( align_actions[i]);
	}
	QPtrList<KAction> alignment_actions;
	alignment_actions.append( m_pAlignmentAction);
	p_ktechlab->plugActionList( "alignment_actionlist", alignment_actions);
}


void ItemDocument::slotInitItemActions( Item *item)
{
	Q_UNUSED(item);
	
	ItemView * activeItemView = dynamic_cast<ItemView*>(activeView());
	if( !p_ktechlab || !activeItemView)
		return;
	
	KAction * align_actions[] = { 
		activeItemView->action("align_horizontally"),
		activeItemView->action("align_vertically"),
		activeItemView->action("distribute_horizontally"),
		activeItemView->action("distribute_vertically") };
	
	bool enableAlignment = selectList()->itemCount() > 1;
	for( unsigned i = 0; i < 4; ++i)
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
	QPixmap pm( bigness, bigness);
// 	pm.fill( KTLConfig::bgColor()); // first fill the background colour in
	pm.fill( Qt::white);
	
	if( KTLConfig::showGrid()){
		QPainter p(&pm); // setup painter to draw on pixmap
		p.setPen( KTLConfig::gridColor()); // set forecolour
		// note: anything other than 8 borks this
		for( int i = (interval / 2); i < bigness; i+=interval){
			p.drawLine( 0, i, bigness, i); // horizontal
			p.drawLine( i, 0, i, bigness); // vertical
		}
		p.end(); // all done
	}

	pm.setDefaultOptimization( QPixmap::BestOptim);
	m_canvas->setBackgroundPixmap(pm); // and the finale.
}


void ItemDocument::requestEvent( ItemDocumentEvent::type type)
{
	m_queuedEvents |= type;
	m_pEventTimer->stop();
	m_pEventTimer->start( 0, true);
}


void ItemDocument::processItemDocumentEvents()
{
	// Copy it incase we have new events requested while doing this...
	unsigned queuedEvents = m_queuedEvents;
	m_queuedEvents = 0;
	
	if( queuedEvents & ItemDocumentEvent::ResizeCanvasToItems)
		resizeCanvasToItems();
	
	if( queuedEvents & ItemDocumentEvent::UpdateZOrdering)
		slotUpdateZOrdering();
	
	ICNDocument * icnd = dynamic_cast<ICNDocument*>(this);
	
	if( icnd && (queuedEvents & ItemDocumentEvent::UpdateNodeGroups))
		icnd->slotAssignNodeGroups();
	
	if( icnd && (queuedEvents & ItemDocumentEvent::RerouteInvalidatedConnectors))
		icnd->rerouteInvalidatedConnectors();
}


void ItemDocument::resizeCanvasToItems()
{
	const ViewList::iterator end = m_viewList.end();
	
	QRect bound = canvasBoundingRect();
	QSize size( bound.right(), bound.bottom());
	
	m_viewList.remove((View*)0);
	
	for( ViewList::iterator it = m_viewList.begin(); it != end; ++it)
	{
		CVBEditor * cvbEditor = (static_cast<ItemView*>((View*)*it))->cvbEditor();
		
		int contentsX, contentsY;
		int contentsWMX, contentsWMY;
		
 		cvbEditor->viewportToContents( cvbEditor->viewport()->width(), cvbEditor->viewport()->height(), contentsX, contentsY);
		cvbEditor->inverseWorldMatrix().map( contentsX, contentsY, &contentsWMX, &contentsWMY);
		
		// Hack to fix a bug whereby when scrolled, but emoty gap before scrollbars,
		// size slowly decreases one pixel at a time
		if( (contentsX - contentsWMX) == 1)
			contentsWMX = contentsX;
		if( (contentsY - contentsWMY) == 1)
			contentsWMY = contentsY;
		
		size = size.expandedTo( QSize( contentsWMX, contentsWMY));
	}
	
	// We want to avoid flicker....
	for( ViewList::iterator it = m_viewList.begin(); it != end; ++it)
	{
		ItemView * itemView = static_cast<ItemView*>((View*)*it);
		CVBEditor * cvbEditor = itemView->cvbEditor();
		
		cvbEditor->setVScrollBarMode( ((size.height()*itemView->zoomLevel()) > cvbEditor->visibleHeight()) ? QScrollView::AlwaysOn : QScrollView::AlwaysOff);
		cvbEditor->setHScrollBarMode( ((size.width()*itemView->zoomLevel()) > cvbEditor->visibleWidth()) ? QScrollView::AlwaysOn : QScrollView::AlwaysOff);
	}

	bool changedSize = canvas()->size() != size;
	canvas()->resize( size.width(), size.height());

	if(changedSize) requestEvent( ItemDocumentEvent::ResizeCanvasToItems);
	else if(ICNDocument * icnd = dynamic_cast<ICNDocument*>(this))
		icnd->createCellMap();
}

QRect ItemDocument::canvasBoundingRect() const
{
	QRect bound;
	
	const QCanvasItemList allItems = canvas()->allItems();
	const QCanvasItemList::const_iterator end = allItems.end();
	for( QCanvasItemList::const_iterator it = allItems.begin(); it != end; ++it)
	{
		if( !(*it)->isVisible()) continue;
		bound |= (*it)->boundingRect();
	}

	if( !bound.isNull())
	{
		bound.setLeft( bound.left() - 16);
		bound.setTop( bound.top() - 16);
		bound.setRight( bound.right() + 16);
		bound.setBottom( bound.bottom() + 16);
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
	if(type() == Document::dt_flowcode)
		cropMessage = i18n("Crop image to program parts");

	else if( type() == Document::dt_circuit)
		cropMessage = i18n("Crop image to circuit components");

	else cropMessage = i18n("Crop image");

	QCheckBox *cropCheck = new QCheckBox( cropMessage, p_ktechlab, "cropCheck");
	cropCheck->setChecked(true); // yes by default?

	// we need an object so we can retrieve which image type was selected by the user
	// so setup the filedialog.
	KFileDialog exportDialog(QString::null, "*.png|PNG Image\n*.bmp|BMP Image\n*.svg|SVG Image" , p_ktechlab, i18n("Export As Image"), true, cropCheck);

	exportDialog.setOperationMode( KFileDialog::Saving);
	// now actually show it
	if( exportDialog.exec() == QDialog::Rejected) return;

	KURL url = exportDialog.selectedURL();

	if(url.isEmpty()) return;
	
	if( QFile::exists( url.path())) {
		int query = KMessageBox::warningYesNo( p_ktechlab, i18n( "A file named \"%1\" already exists. " "Are you sure you want to overwrite it?").arg( url.fileName()), i18n( "Overwrite File?"), i18n( "Overwrite"), KStdGuiItem::cancel());
		if( query == KMessageBox::No) return;
	}
	
	// with Qt, you always "print" to a
	// QPainter.. whether the output medium is a pixmap, a screen,
	// or paper
	
	// needs to be something like QPicture to do SVG etc...
	// at the moment the pixmap is just as big as the canvas,
	// intend to make some kind of cropping thing so it just 
	// takes the bit with the circuit on.
	
	QRect saveArea;
	QString type;
	QRect cropArea;
	QPaintDevice *outputImage;
	QString filter = exportDialog.currentFilter();
	filter = filter.lower(); // gently soften the appearance of the letters.
	
	// did have a switch here but seems you can't use that on strings
	if( filter == "*.png") type = "PNG";
	else if( filter == "*.bmp") type = "BMP";
	else if( filter == "*.svg") {
		KMessageBox::information( NULL, i18n("SVG export is sub-functional"), i18n("Export As Image"));
		type = "SVG";
	}
	// I don't like forcing people to use the right extension (personally)
	// but it is the easiest way to decide image type.
	else {
		KMessageBox::sorry( NULL, i18n("Unknown extension, please select one from the filter list."), i18n("Export As Image"));
		return;
	}

	if( cropCheck->isChecked()) {
		cropArea = canvasBoundingRect();
		if( cropArea.isNull())
		{  
			KMessageBox::sorry( 0, i18n("There is nothing to crop"), i18n("Export As Image"));
			return;
		} else cropArea &= canvas()->rect();

	}

	saveArea = m_canvas->rect();

	if( type == "PNG" || type == "BMP")
		outputImage = new QPixmap( saveArea.size());
	else if( type == "SVG") {
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
	m_canvas->drawArea( saveArea, &p);
	updateBackground();

	p.end();
	bool saveResult;

	// if cropping we need to convert to an image,
	// crop, then save.
	if(cropCheck->isChecked()) {
		if( type == "SVG")
			saveResult = dynamic_cast<QPicture*>(outputImage)->save( url.path(), type);
		else {
			QImage img = dynamic_cast<QPixmap*>(outputImage)->convertToImage();
			img = img.copy(cropArea);
			saveResult = img.save(url.path(),type);
		}
	} else {
		if( type=="SVG")
			saveResult = dynamic_cast<QPicture*>(outputImage)->save( url.path(), type);
		else saveResult = dynamic_cast<QPixmap*>(outputImage)->save( url.path(), type);
	}

	//if(saveResult == true)	KMessageBox::information( this, i18n("Sucessfully exported to \"%1\"").arg( url.filename()), i18n("Image Export"));
	//else KMessageBox::information( this, i18n("Export failed"), i18n("Image Export"));

	if( type == "SVG") setSVGExport(false);
	
	if(saveResult == false)
		KMessageBox::information( p_ktechlab, i18n("Export failed"), i18n("Image Export"));

	delete outputImage;
}

void ItemDocument::setSVGExport( bool svgExport)
{
	// Find any items and tell them not to draw buttons or sliders
	QCanvasItemList items = m_canvas->allItems();
	const QCanvasItemList::iterator end = items.end();
	for( QCanvasItemList::Iterator it = items.begin(); it != end; ++it)
	{
		if( CNItem * cnItem = dynamic_cast<CNItem*>(*it))
			cnItem->setDrawWidgets(!svgExport);
	}
}

void ItemDocument::raiseZ()
{
	raiseZ( selectList()->items(true));
}

void ItemDocument::raiseZ( const ItemList & itemList)
{
	if( m_zOrder.isEmpty()) slotUpdateZOrdering();

	if( m_zOrder.isEmpty()) return;

	IntItemMap::iterator begin = m_zOrder.begin();
	IntItemMap::iterator previous = m_zOrder.end();
	IntItemMap::iterator it = --m_zOrder.end();

	do {
		Item * previousData = (previous == m_zOrder.end()) ? 0 : previous.data();
		Item * currentData = it.data();

		if( currentData && previousData && itemList.contains(currentData) && !itemList.contains(previousData)) {
			previous.data() = currentData;
			it.data() = previousData;
		}

		previous = it;
		--it;
	} while ( previous != begin);

	slotUpdateZOrdering();
}

void ItemDocument::lowerZ()
{
	lowerZ( selectList()->items(true));
}
void ItemDocument::lowerZ( const ItemList & itemList)
{
	if(m_zOrder.isEmpty()) slotUpdateZOrdering();
	
	if(m_zOrder.isEmpty()) return;
	
	IntItemMap::iterator previous = m_zOrder.begin();
	IntItemMap::iterator end = m_zOrder.end();
	for( IntItemMap::iterator it = m_zOrder.begin(); it != end; ++it)
	{
		Item * previousData = previous.data();
		Item * currentData = it.data();
		
		if( currentData && previousData && itemList.contains(currentData) && !itemList.contains(previousData))
		{
			previous.data() = currentData;
			it.data() = previousData;
		}
		
		previous = it;
	}
	
	slotUpdateZOrdering();
}

void ItemDocument::itemAdded( Item *)
{
	requestEvent( ItemDocument::ItemDocumentEvent::UpdateZOrdering);
}

void ItemDocument::slotUpdateZOrdering()
{
	ItemList toAdd = m_itemList;
	toAdd.remove((Item*)0);
	
	IntItemMap newZOrder;
	int atLevel = 0;
	
	IntItemMap::iterator zEnd = m_zOrder.end();
	for( IntItemMap::iterator it = m_zOrder.begin(); it != zEnd; ++it)
	{	
		Item * item = it.data();
		if(!item) continue;
		
		toAdd.remove(item);
		
		if( !item->parentItem() && item->isMovable())
			newZOrder[atLevel++] = item;
	}
	
	ItemList::iterator addEnd = toAdd.end();
	for( ItemList::iterator it = toAdd.begin(); it != addEnd; ++it) {
		Item * item = *it;
		if( item->parentItem() || !item->isMovable())
			continue;
		
		newZOrder[atLevel++] = item;
	}
	
	m_zOrder = newZOrder;
	
	zEnd = m_zOrder.end();
	for( IntItemMap::iterator it = m_zOrder.begin(); it != zEnd; ++it)
		it.data()->updateZ( it.key());
}

void ItemDocument::update()
{
	ItemList::iterator end = m_itemList.end();
	for(ItemList::iterator it = m_itemList.begin(); it != end; ++it) {
		if( (*it)->hasDynamicContent())
			(*it)->setChanged();
	}
}
//END class ItemDocument

//BEGIN class CanvasTip
CanvasTip::CanvasTip( ItemDocument *itemDocument, QCanvas *qcanvas)
	: QCanvasText(qcanvas)
{
	p_itemDocument = itemDocument;
	
	setColor( Qt::black);
	setZ( ICNDocument::Z::Tip);
}

CanvasTip::~CanvasTip()
{
}

void CanvasTip::displayVI( ECNode *node, const QPoint &pos)
{
	if( !node || !updateVI()) return;

	unsigned num = node->numPins();

	m_v.resize(num);
	m_i.resize(num);

	for( unsigned i = 0; i < num; i++) {
		if( Pin * pin = node->pin(i)) {
			m_v[i] = pin->voltage();
			m_i[i] = pin->current();
		}
	}

	display(pos);
}

void CanvasTip::displayVI( Connector *connector, const QPoint &pos)
{
	if( !connector || !updateVI()) return;

	unsigned num = connector->numWires();

	m_v.resize(num);
	m_i.resize(num);

	for( unsigned i = 0; i < num; i++) {
		if( Wire * wire = connector->wire(i)) {
			m_v[i] = wire->voltage();
			m_i[i] = std::abs(wire->current());
		}
	}

	display(pos);
}

bool CanvasTip::updateVI()
{
	CircuitDocument *circuitDocument = dynamic_cast<CircuitDocument*>(p_itemDocument);
	if( !circuitDocument || !Simulator::self()->isSimulating())
		return false;

	circuitDocument->calculateConnectorCurrents();
	return true;
}

void CanvasTip::display( const QPoint &pos)
{
	unsigned num = m_v.size();

	for( unsigned i = 0; i < num; i++) {
		if( !std::isfinite(m_v[i]) || std::abs(m_v[i]) < 1e-9)
			m_v[i] = 0.;
	
		if( !std::isfinite(m_i[i]) || std::abs(m_i[i]) < 1e-9)
			m_i[i] = 0.;
	}

	move( pos.x()+20, pos.y()+4);

	if(num == 0) return;

	if(num == 1) setText( displayText(0));
	else {
		QString text;
		for( unsigned i = 0; i < num; i++)
			text += QString(" %1: %2\n").arg( QString::number(i)).arg( displayText(i));
		setText(text);
	}
}

QString CanvasTip::displayText( unsigned num) const
{
	if( m_v.size() <= num) return QString::null;

	return QString(" %1%2V  %3%4A ")
			.arg( QString::number( m_v[num] / CNItem::getMultiplier(m_v[num]), 'g', 3))
			.arg( CNItem::getNumberMag( m_v[num]))
			.arg( QString::number( m_i[num] / CNItem::getMultiplier(m_i[num]), 'g', 3))
			.arg( CNItem::getNumberMag( m_i[num]));
}

void CanvasTip::draw( QPainter &p)
{
	CircuitDocument *circuitDocument = dynamic_cast<CircuitDocument*>(p_itemDocument);
	if( !circuitDocument || !Simulator::self()->isSimulating())
		return;

	p.setBrush( QColor( 0xff, 0xff, 0xdc));
	p.setPen( Qt::black);
	p.drawRect( boundingRect());
	QCanvasText::draw(p);
}
//END class CanvasTip

//BEGIN class Canvas
Canvas::Canvas( ItemDocument *itemDocument, const char * name)
	: QCanvas( itemDocument, name)
{
	p_itemDocument = itemDocument;
	m_pMessageTimeout = new QTimer(this);
	connect( m_pMessageTimeout, SIGNAL(timeout()), this, SLOT(slotSetAllChanged()));
}

void Canvas::setMessage( const QString & message)
{
	m_message = message;
	
	if( message.isEmpty())
		m_pMessageTimeout->stop();
	else m_pMessageTimeout->start( 2000, true);

	setAllChanged();
}

void Canvas::drawBackground ( QPainter &p, const QRect & clip)
{
	QCanvas::drawBackground( p, clip);
#if 0
	const int scx = (int)((clip.left()-4)/8);
	const int ecx = (int)((clip.right()+4)/8);
	const int scy = (int)((clip.top()-4)/8);
	const int ecy = (int)((clip.bottom()+4)/8);
	if( !((ICNDocument*)(p_itemDocument))->isValidCellReference( scx, scy) ||
			   !((ICNDocument*)(p_itemDocument))->isValidCellReference( ecx, ecy)) return;
	Cells *c = ((ICNDocument*)(p_itemDocument))->cells();
	for( int x=scx; x<=ecx; x++)
	{
		for( int y=scy; y<=ecy; y++)
		{
			const double score = (*c)[x][y].CIpenalty+(*c)[x][y].Cpenalty;
			int value = (int)std::log(score)*20;
			if( value>255) value=255;
			else if(value<0) value=0;
			p.setBrush( QColor( 255, (255-value), (255-value)));
			p.setPen( Qt::NoPen);
			p.drawRect( (x*8), (y*8), 8, 8);
		}
	}
#endif
}

void Canvas::drawForeground(QPainter &p, const QRect &clip)
{
	QCanvas::drawForeground( p, clip);
	
	if( !m_pMessageTimeout->isActive()) return;

	// Following code stolen and adapted from amarok/src/playlist.cpp :)

	// Find out width of smallest view
	QSize minSize;
	const ViewList viewList = p_itemDocument->viewList();
	ViewList::const_iterator end = viewList.end();
	View *firstView = 0;
	for( ViewList::const_iterator it = viewList.begin(); it != end; ++it)
	{
		if( !*it) continue;
		
		if(!firstView) {
			firstView = *it;
			minSize = (*it)->size();
		} else minSize = minSize.boundedTo( (*it)->size());
	}

	if(!firstView) return;

	QSimpleRichText * t = new QSimpleRichText( m_message, QApplication::font());

	int w = t->width();
	int h = t->height();
	int x = 15;
	int y = 15;
	int b = 10; // text padding

	if( w+2*b >= minSize.width() || h+2*b >= minSize.height())
	{
		delete t;
		return;
	}

	p.setBrush( firstView->colorGroup().background());
	p.drawRoundRect( x, y, w+2*b, h+2*b, (8*200)/(w+2*b), (8*200)/(h+2*b));
	t->draw( &p, x+b, y+b, QRect(), firstView->colorGroup());
	delete t;
}

void Canvas::update()
{
	p_itemDocument->update();
	QCanvas::update();
}
//END class Canvas

#include "itemdocument.moc"

