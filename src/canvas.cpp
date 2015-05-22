/***************************************************************************
 *   Copyright (C) 1999-2005 Trolltech AS                                  *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This file may be distributed and/or modified under the terms of the   *
 *   GNU General Public License version 2 as published by the Free         *
 *   Software Foundation                                                   *
 ***************************************************************************/

#include "utils.h"
#include "canvas.h"
#include "canvas_private.h"

#include <kdebug.h>

#include "Qt/qapplication.h"
#include "Qt/qbitmap.h"
//#include "Qt/q3ptrdict.h"
#include "Qt/qpainter.h"
#include "Qt/q3polygonscanner.h"
#include "Qt/qtimer.h"
#include "Qt/q3tl.h"
#include <Qt/q3scrollview.h>
#include <Qt/qdesktopwidget.h>


#include <stdlib.h>

using namespace std;

static bool isCanvasDebugEnabled() {
    return false;
}

//BEGIN class KtlQCanvasClusterizer


static void include(QRect& r, const QRect& rect)
{
	if (rect.left()<r.left()) {
		r.setLeft(rect.left());
	}
	if (rect.right()>r.right()) {
		r.setRight(rect.right());
	}
	if (rect.top()<r.top()) {
		r.setTop(rect.top());
	}
	if (rect.bottom()>r.bottom()) {
		r.setBottom(rect.bottom());
	}
}

/*
A KtlQCanvasClusterizer groups rectangles (QRects) into non-overlapping rectangles
by a merging heuristic.
*/
KtlQCanvasClusterizer::KtlQCanvasClusterizer(int maxclusters)
	: cluster(new QRect[maxclusters]), count(0), maxcl(maxclusters)
{
}

KtlQCanvasClusterizer::~KtlQCanvasClusterizer()
{
	delete [] cluster;
}


void KtlQCanvasClusterizer::clear()
{
	count=0;
}

void KtlQCanvasClusterizer::add(int x, int y)
{
	add(QRect(x,y,1,1));
}

void KtlQCanvasClusterizer::add(int x, int y, int w, int h)
{
	add(QRect(x,y,w,h));
}

void KtlQCanvasClusterizer::add(const QRect& rect)
{
	QRect biggerrect(rect.x()-1,rect.y()-1,rect.width()+2,rect.height()+2);

    //assert(rect.width()>0 && rect.height()>0);

	int cursor;

	for (cursor=0; cursor<count; cursor++) {
		if (cluster[cursor].contains(rect)) {
	    // Wholly contained already.
			return;
		}
	}

	int lowestcost=9999999;
	int cheapest=-1;
	cursor = 0;
	while( cursor<count ) {
		if (cluster[cursor].intersects(biggerrect)) {
			QRect larger=cluster[cursor];
			include(larger,rect);
			int cost = larger.width()*larger.height() -
					cluster[cursor].width()*cluster[cursor].height();

			if (cost < lowestcost) {
				bool bad=false;
				for (int c=0; c<count && !bad; c++) {
					bad=cluster[c].intersects(larger) && c!=cursor;
				}

				if (!bad) {
					cheapest=cursor;
					lowestcost=cost;
				}
			}
		}
		cursor++;
	}

	if (cheapest>=0) {
		include(cluster[cheapest],rect);
		return;
	}

	if (count < maxcl) {
		cluster[count++]=rect;
		return;
	}

    // Do cheapest of:
    //     add to closest cluster
    //     do cheapest cluster merge, add to new cluster

	lowestcost=9999999;
	cheapest=-1;
	cursor=0;
	while( cursor<count ) {
		QRect larger=cluster[cursor];
		include(larger,rect);
		int cost=larger.width()*larger.height()
				- cluster[cursor].width()*cluster[cursor].height();
		if (cost < lowestcost) {
			bool bad=false;
			for (int c=0; c<count && !bad; c++) {
				bad=cluster[c].intersects(larger) && c!=cursor;
			}

			if (!bad) {
				cheapest=cursor;
				lowestcost=cost;
			}
		}
		cursor++;
	}

    // ###
    // could make an heuristic guess as to whether we need to bother
    // looking for a cheap merge.

	int cheapestmerge1 = -1;
	int cheapestmerge2 = -1;

	int merge1 = 0;
	while( merge1 < count ) {
		int merge2=0;
		while( merge2 < count ) {
			if( merge1!=merge2) {
				QRect larger=cluster[merge1];
				include(larger,cluster[merge2]);
				int cost=larger.width()*larger.height()
						- cluster[merge1].width()*cluster[merge1].height()
						- cluster[merge2].width()*cluster[merge2].height();
				if (cost < lowestcost) {
					bool bad=false;

					for (int c=0; c<count && !bad; c++) {
						bad=cluster[c].intersects(larger) && c!=cursor;
					}

					if (!bad) {
						cheapestmerge1=merge1;
						cheapestmerge2=merge2;
						lowestcost=cost;
					}
				}
			}
			merge2++;
		}
		merge1++;
	}

	if (cheapestmerge1>=0) {
		include(cluster[cheapestmerge1],cluster[cheapestmerge2]);
		cluster[cheapestmerge2]=cluster[count--];
	} else {
	// if (!cheapest) debugRectangles(rect);
		include(cluster[cheapest],rect);
	}

    // NB: clusters do not intersect (or intersection will
    //     overwrite). This is a result of the above algorithm,
    //     given the assumption that (x,y) are ordered topleft
    //     to bottomright.

    // ###
	//
    // add explicit x/y ordering to that comment, move it to the top
    // and rephrase it as pre-/post-conditions.
}

const QRect& KtlQCanvasClusterizer::operator[](int i)
{
	return cluster[i];
}

//END class KtlQCanvasClusterizer


void KtlQCanvasItemList::sort()
{
	qHeapSort(*  /* ((QList<KtlQCanvasItemPtr>*)) */ this);
}


KtlQCanvasItemList KtlQCanvasItemList::operator+(const KtlQCanvasItemList &l) const
{
	KtlQCanvasItemList l2(*this);
	for(const_iterator it = l.begin(); it != l.end(); ++it)
		l2.append(*it);
	return l2;
}



static int gcd(int a, int b)
{
	int r;
	while ( (r = a%b) ) {
		a=b;
		b=r;
	}
	return b;
}

static int scm(int a, int b)
{
	int g = gcd(a,b);
	return a/g*b;
}


int KtlQCanvas::toChunkScaling( int x ) const
{
	return roundDown( x, chunksize );
}
	

void KtlQCanvas::initChunkSize( const QRect & s )
{
	m_chunkSize = QRect( toChunkScaling(s.left()),
			 toChunkScaling(s.top()),
			 ((s.width()-1)/chunksize)+3,
			 ((s.height()-1)/chunksize)+3 );
}
	

void KtlQCanvas::init(int w, int h, int chunksze, int mxclusters)
{
	init( QRect( 0, 0, w, h ), chunksze, mxclusters );
}

void KtlQCanvas::init( const QRect & r, int chunksze, int mxclusters )
{
	m_size = r ;
	chunksize=chunksze;
	maxclusters=mxclusters;
	initChunkSize( r );
	chunks=new KtlQCanvasChunk[m_chunkSize.width()*m_chunkSize.height()];
	update_timer = 0;
	bgcolor = Qt::white;
	grid = 0;
	htiles = 0;
	vtiles = 0;
	debug_redraw_areas = false;
}

KtlQCanvas::KtlQCanvas( QObject* parent, const char* name )
	: QObject( parent, name )
{
	init(0,0);
}

KtlQCanvas::KtlQCanvas(const int w, const int h)
{
	init(w,h);
}

KtlQCanvas::KtlQCanvas( QPixmap p, int h, int v, int tilewidth, int tileheight ) {

	init(h*tilewidth, v*tileheight, scm(tilewidth,tileheight) );
	setTiles( p, h, v, tilewidth, tileheight );
}

void qt_unview( KtlQCanvas * c )
{
	for (KtlQCanvasView* view=c->m_viewList.first(); view != 0; view=c->m_viewList.next())
		view->viewing = 0;
}

KtlQCanvas::~KtlQCanvas()
{
	qt_unview(this);
	KtlQCanvasItemList all = allItems();
	for (KtlQCanvasItemList::Iterator it=all.begin(); it!=all.end(); ++it)
		delete *it;
	delete [] chunks;
	delete [] grid;
}

/*!
	\internal
	Returns the chunk at a chunk position \a i, \a j.
 */
KtlQCanvasChunk& KtlQCanvas::chunk(int i, int j) const
{
	i -= m_chunkSize.left();
	j -= m_chunkSize.top();
    //return chunks[i+m_chunkSize.width()*j];
    const int chunkOffset = i + m_chunkSize.width() * j;
    if ((chunkOffset < 0) || (chunkOffset >= (m_chunkSize.width()*m_chunkSize.height())) ) {
        qWarning() << Q_FUNC_INFO << " invalid chunk coordinates: " << i << " " << j;
        return chunks[0]; // at least it should not crash
    }
    return chunks[chunkOffset];
}

/*!
	\internal
	Returns the chunk at a pixel position \a x, \a y.
 */
KtlQCanvasChunk& KtlQCanvas::chunkContaining(int x, int y) const
{
	return chunk( toChunkScaling(x), toChunkScaling(y) );
}

KtlQCanvasItemList KtlQCanvas::allItems()
{
	KtlQCanvasItemList list;
	SortedCanvasItems::iterator end = m_canvasItems.end();
	for ( SortedCanvasItems::iterator it = m_canvasItems.begin(); it != end; ++it )
		list << it->second;
	return list;
}

void KtlQCanvas::resize( const QRect & newSize )
{
	if ( newSize == m_size )
		return;

	KtlQCanvasItem* item;
	Q3PtrList<KtlQCanvasItem> hidden;
	SortedCanvasItems::iterator end = m_canvasItems.end();
	for ( SortedCanvasItems::iterator it = m_canvasItems.begin(); it != end; ++it )
	{
		KtlQCanvasItem * i = it->second;
		if ( i->isVisible() )
		{
			i->hide();
			hidden.append( i );
		}
	}

	initChunkSize( newSize );
	KtlQCanvasChunk* newchunks = new KtlQCanvasChunk[m_chunkSize.width()*m_chunkSize.height()];
	m_size = newSize;
	delete [] chunks;
	chunks=newchunks;

	for (item=hidden.first(); item != 0; item=hidden.next()) {
		item->show();
	}

	setAllChanged();

	emit resized();
}

void KtlQCanvas::retune(int chunksze, int mxclusters)
{
	maxclusters=mxclusters;

	if ( chunksize!=chunksze )
	{
		Q3PtrList<KtlQCanvasItem> hidden;
		SortedCanvasItems::iterator end = m_canvasItems.end();
		for ( SortedCanvasItems::iterator it = m_canvasItems.begin(); it != end; ++it )
		{
			KtlQCanvasItem * i = it->second;
			if ( i->isVisible() )
			{
				i->hide();
				hidden.append( i );
			}
		}

		chunksize=chunksze;

		initChunkSize( m_size );
		KtlQCanvasChunk* newchunks = new KtlQCanvasChunk[m_chunkSize.width()*m_chunkSize.height()];
		delete [] chunks;
		chunks=newchunks;

		for (KtlQCanvasItem* item=hidden.first(); item != 0; item=hidden.next()) {
			item->show();
		}
	}
}

void KtlQCanvas::addItem(KtlQCanvasItem* item)
{
	m_canvasItems.insert( make_pair( item->z(), item ) );
}

void KtlQCanvas::removeItem(const KtlQCanvasItem* item)
{
	SortedCanvasItems::iterator end = m_canvasItems.end();
	for ( SortedCanvasItems::iterator it = m_canvasItems.begin(); it != end; ++it )
	{
		if ( it->second == item )
		{
			m_canvasItems.erase( it );
			return;
		}
	}
}

void KtlQCanvas::addView(KtlQCanvasView* view)
{
	m_viewList.append(view);
	if ( htiles>1 || vtiles>1 || pm.isNull() )
		view->viewport()->setBackgroundColor(backgroundColor());
}

void KtlQCanvas::removeView(KtlQCanvasView* view)
{
	m_viewList.removeRef(view);
}

void KtlQCanvas::setUpdatePeriod(int ms)
{
	if ( ms<0 ) {
		if ( update_timer )
			update_timer->stop();
	} else {
		if ( update_timer )
			delete update_timer;
		update_timer = new QTimer(this);
		connect(update_timer,SIGNAL(timeout()),this,SLOT(update()));
		update_timer->start(ms);
	}
}

// Don't call this unless you know what you're doing.
// p is in the content's co-ordinate example.
void KtlQCanvas::drawViewArea( KtlQCanvasView* view, QPainter* p, const QRect& vr, bool dbuf )
{
	QPoint tl = view->contentsToViewport(QPoint(0,0));

	QWMatrix wm = view->worldMatrix();
	QWMatrix iwm = wm.invert();
    // ivr = covers all chunks in vr
	QRect ivr = iwm.map(vr);
	QWMatrix twm;
	twm.translate(tl.x(),tl.y());

// 	QRect all(0,0,width(),height());
	QRect all(m_size);

	if ( !all.contains(ivr) )
	{
		// Need to clip with edge of canvas.
		
		// For translation-only transformation, it is safe to include the right
		// and bottom edges, but otherwise, these must be excluded since they
		// are not precisely defined (different bresenham paths).
		Q3PointArray a;
		if ( wm.m12()==0.0 && wm.m21()==0.0 && wm.m11() == 1.0 && wm.m22() == 1.0 )
			a = Q3PointArray( QRect(all.x(),all.y(),all.width()+1,all.height()+1) );
		else	a = Q3PointArray( all );

		a = (wm*twm).map(a);
	
		if ( view->viewport()->backgroundMode() == Qt::NoBackground )
		{
			QRect cvr = vr; cvr.moveBy(tl.x(),tl.y());
			p->setClipRegion(QRegion(cvr)-QRegion(a));
			p->fillRect(vr,view->viewport()->palette()
					.brush(QPalette::Active,QColorGroup::Background));
		}
		p->setClipRegion(a);
	}

	if ( dbuf ) {
		offscr = QPixmap(vr.width(), vr.height());
		offscr.x11SetScreen(p->device()->x11Screen());
		QPainter dbp(&offscr);
	
		twm.translate(-vr.x(),-vr.y());
		twm.translate(-tl.x(),-tl.y());
		dbp.setWorldMatrix( wm*twm, true );
	
		dbp.setClipRect(0,0,vr.width(), vr.height());
// 		dbp.setClipRect(v);
		drawCanvasArea(ivr,&dbp,false);
		dbp.end();
		p->drawPixmap(vr.x(), vr.y(), offscr, 0, 0, vr.width(), vr.height());
	} else {
		QRect r = vr; r.moveBy(tl.x(),tl.y()); // move to untransformed co-ords
		if ( !all.contains(ivr) )
		{
			QRegion inside = p->clipRegion() & r;
	 	   //QRegion outside = p->clipRegion() - r;
		    //p->setClipRegion(outside);
		    //p->fillRect(outside.boundingRect(),red);
			p->setClipRegion(inside);
		} else {
			p->setClipRect(r);
		}
		p->setWorldMatrix( wm*twm );
	
		p->setBrushOrigin(tl.x(), tl.y());
		drawCanvasArea(ivr,p,false);
	}
}

void KtlQCanvas::advance()
{
    qWarning() << "KtlQCanvas::advance: TODO"; // TODO
}


/*!
	Repaints changed areas in all views of the canvas.
 */
void KtlQCanvas::update()
{
	KtlQCanvasClusterizer clusterizer(m_viewList.count());
	Q3PtrList<QRect> doneareas;
	doneareas.setAutoDelete(true);

	Q3PtrListIterator<KtlQCanvasView> it(m_viewList);
	KtlQCanvasView* view;
	while( (view=it.current()) != 0 ) {
		++it;
		QWMatrix wm = view->worldMatrix();
	
		QRect area(view->contentsX(),view->contentsY(),
				   view->visibleWidth(),view->visibleHeight());
		if (area.width()>0 && area.height()>0) {
		
			if ( !wm.isIdentity() )
			{
				// r = Visible area of the canvas where there are changes
				QRect r = changeBounds(view->inverseWorldMatrix().map(area));
				if ( !r.isEmpty() )
				{
                    //view->viewport()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true); // note: remove this when possible
					QPainter p(view->viewport());
		  		  // Translate to the coordinate system of drawViewArea().
					QPoint tl = view->contentsToViewport(QPoint(0,0));
					p.translate(tl.x(),tl.y());
					drawViewArea( view, &p, wm.map(r), true );
					doneareas.append(new QRect(r));
				}
			} else clusterizer.add(area);
		}
	}

	for (int i=0; i<clusterizer.clusters(); i++)
		drawChanges(clusterizer[i]);

	for ( QRect* r=doneareas.first(); r != 0; r=doneareas.next() )
		setUnchanged(*r);
}


/*!
	Marks the whole canvas as changed.
	All views of the canvas will be entirely redrawn when
	update() is called next.
 */
void KtlQCanvas::setAllChanged()
{
	setChanged(m_size);
}

/*!
	Marks \a area as changed. This \a area will be redrawn in all
	views that are showing it when update() is called next.
 */
void KtlQCanvas::setChanged(const QRect& area)
{
	QRect thearea = area.intersect( m_size );

	int mx = toChunkScaling(thearea.x()+thearea.width()+chunksize);
	int my = toChunkScaling(thearea.y()+thearea.height()+chunksize);
	if (mx > m_chunkSize.right())
		mx=m_chunkSize.right();
	if (my > m_chunkSize.bottom())
		my=m_chunkSize.bottom();

	int x= toChunkScaling(thearea.x());
	while( x<mx) {
		int y = toChunkScaling(thearea.y());
		while( y<my ) {
			chunk(x,y).change();
			y++;
		}
		x++;
	}
}

/*!
	Marks \a area as \e unchanged. The area will \e not be redrawn in
	the views for the next update(), unless it is marked or changed
	again before the next call to update().
 */
void KtlQCanvas::setUnchanged(const QRect& area)
{
	QRect thearea = area.intersect( m_size );

	int mx = toChunkScaling(thearea.x()+thearea.width()+chunksize);
	int my = toChunkScaling(thearea.y()+thearea.height()+chunksize);
	if (mx > m_chunkSize.right())
		mx=m_chunkSize.right();
	if (my > m_chunkSize.bottom())
		my=m_chunkSize.bottom();

	int x = toChunkScaling(thearea.x());
	while( x<mx) {
		int y = toChunkScaling(thearea.y());
		while( y<my ) {
			chunk(x,y).takeChange();
			y++;
		}
		x++;
	}
}


QRect KtlQCanvas::changeBounds(const QRect& inarea)
{
	QRect area=inarea.intersect( m_size );

	int mx = toChunkScaling(area.x()+area.width()+chunksize);
	int my = toChunkScaling(area.y()+area.height()+chunksize);
	if (mx > m_chunkSize.right())
		mx=m_chunkSize.right();
	if (my > m_chunkSize.bottom())
		my=m_chunkSize.bottom();

	QRect result;

	int x = toChunkScaling(area.x());
	while( x<mx ) {
		int y = toChunkScaling(area.y());
		while( y<my ) {
			KtlQCanvasChunk& ch=chunk(x,y);
			if ( ch.hasChanged() )
				result |= QRect(x,y,1,1);
			y++;
		}
		x++;
	}

	if ( !result.isEmpty() ) {
		result.rLeft() *= chunksize;
		result.rTop() *= chunksize;
		result.rRight() *= chunksize;
		result.rBottom() *= chunksize;
		result.rRight() += chunksize;
		result.rBottom() += chunksize;
	}

	return result;
}

/*!
	Redraws the area \a inarea of the KtlQCanvas.
 */
void KtlQCanvas::drawChanges(const QRect& inarea)
{
	QRect area=inarea.intersect( m_size );

	KtlQCanvasClusterizer clusters(maxclusters);

	int mx = toChunkScaling(area.x()+area.width()+chunksize);
	int my = toChunkScaling(area.y()+area.height()+chunksize);
	if (mx > m_chunkSize.right())
		mx=m_chunkSize.right();
	if (my > m_chunkSize.bottom())
		my=m_chunkSize.bottom();

	int x = toChunkScaling(area.x());
	while( x<mx ) {
		int y = toChunkScaling(area.y());
		while( y<my ) {
			KtlQCanvasChunk& ch=chunk(x,y);
			if ( ch.hasChanged() )
				clusters.add(x,y);
			y++;
		}
		x++;
	}

	for (int i=0; i<clusters.clusters(); i++)
	{
		QRect elarea=clusters[i];
		elarea.setRect(
				elarea.left()*chunksize,
		elarea.top()*chunksize,
		elarea.width()*chunksize,
		elarea.height()*chunksize
					  );
		drawCanvasArea(elarea);
	}
}

/*!
	Paints all canvas items that are in the area \a clip to \a
	painter, using double-buffering if \a dbuf is true.

    e.g. to print the canvas to a printer:
	\code
	QPrinter pr;
	if ( pr.setup() ) {
	QPainter p(&pr);
	canvas.drawArea( canvas.rect(), &p );
}
	\endcode
 */
void KtlQCanvas::drawArea(const QRect& clip, QPainter* painter, bool dbuf)
{
	if ( painter )
		drawCanvasArea( clip, painter, dbuf );
}


void KtlQCanvas::drawCanvasArea(const QRect& inarea, QPainter* p, bool double_buffer)
{
	QRect area=inarea.intersect( m_size );

	if (!m_viewList.first() && !p)
		return; // Nothing to do.

	int lx = toChunkScaling(area.x());
	int ly = toChunkScaling(area.y());
	int mx = toChunkScaling(area.right());
	int my = toChunkScaling(area.bottom());
	if (mx>=m_chunkSize.right())
		mx=m_chunkSize.right()-1;
	if (my>=m_chunkSize.bottom())
		my=m_chunkSize.bottom()-1;

    // Stores the region within area that need to be drawn. It is relative
    // to area.topLeft()  (so as to keep within bounds of 16-bit XRegions)
	QRegion rgn;

	for (int x=lx; x<=mx; x++) {
		for (int y=ly; y<=my; y++) {
	    	// Only reset change if all views updating, and
	    	// wholy within area. (conservative:  ignore entire boundary)
			//
	  		// Disable this to help debugging.
			//
			if (!p) {
				if ( chunk(x,y).takeChange() )
				{
					// ### should at least make bands
					rgn |= QRegion( x*chunksize-area.x(), y*chunksize-area.y(), chunksize,chunksize );
// 					allvisible += *chunk(x,y).listPtr();
					setNeedRedraw( chunk(x,y).listPtr() );
// 					chunk(x,y).listPtr()->first()->m_bNeedRedraw = true;
				}
			} else {
// 				allvisible += *chunk(x,y).listPtr();
				setNeedRedraw( chunk(x,y).listPtr() );
			}
		}
	}
// 	allvisible.sort();

	if ( double_buffer ) {
		offscr = QPixmap(area.width(), area.height());
		if (p) offscr.x11SetScreen(p->device()->x11Screen());
	}

	if ( double_buffer && !offscr.isNull() ) {
		QPainter painter;
		painter.begin(&offscr);
		painter.translate(-area.x(),-area.y());
		painter.setBrushOrigin(-area.x(),-area.y());

		if ( p ) {
			painter.setClipRect(QRect(0,0,area.width(),area.height()));
		} else {
			painter.setClipRegion(rgn);
		}

		drawBackground(painter,area);
// 		allvisible.drawUnique(painter);
		drawChangedItems( painter );
		drawForeground(painter,area);
		painter.end();

		if ( p ) {
			p->drawPixmap( area.x(), area.y(), offscr, 0, 0, area.width(), area.height() );
			return;
		}

	} else if ( p ) {
		drawBackground(*p,area);
// 		allvisible.drawUnique(*p);
		drawChangedItems( *p );
		drawForeground(*p,area);
		return;
	}

	QPoint trtr; // keeps track of total translation of rgn

	trtr -= area.topLeft();

	for (KtlQCanvasView* view=m_viewList.first(); view; view=m_viewList.next()) {
		if ( !view->worldMatrix().isIdentity() )
			continue; // Cannot paint those here (see callers).

        //view->viewport()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true); // note: remove this when possible
		QPainter painter(view->viewport());
		QPoint tr = view->contentsToViewport(area.topLeft());
		QPoint nrtr = view->contentsToViewport(QPoint(0,0)); // new translation
		QPoint rtr = nrtr - trtr; // extra translation of rgn
		trtr += rtr; // add to total
		
		if (double_buffer) {
			rgn.translate(rtr.x(),rtr.y());
			painter.setClipRegion(rgn);
			painter.drawPixmap(tr,offscr, QRect(QPoint(0,0),area.size()));
		} else {
			painter.translate(nrtr.x(),nrtr.y());
			rgn.translate(rtr.x(),rtr.y());
			painter.setClipRegion(rgn);
			drawBackground(painter,area);
// 			allvisible.drawUnique(painter);
			drawChangedItems( painter );
			drawForeground(painter,area);
			painter.translate(-nrtr.x(),-nrtr.y());
		}
	}
}


void KtlQCanvas::setNeedRedraw( const KtlQCanvasItemList * list )
{
	KtlQCanvasItemList::const_iterator end = list->end();
	for ( KtlQCanvasItemList::const_iterator it = list->begin(); it != end; ++it )
		(*it)->setNeedRedraw( true );
}


void KtlQCanvas::drawChangedItems( QPainter & painter )
{
	SortedCanvasItems::iterator end = m_canvasItems.end();
	for ( SortedCanvasItems::iterator it = m_canvasItems.begin(); it != end; ++it ) {
		KtlQCanvasItem * i = it->second;
		if ( i->needRedraw() ) {
			i->draw( painter );
			i->setNeedRedraw( false );
		}
	}
}

/*!
	\internal
	This method to informs the KtlQCanvas that a given chunk is
	`dirty' and needs to be redrawn in the next Update.

	(\a x,\a y) is a chunk location.

	The sprite classes call this. Any new derived class of KtlQCanvasItem
	must do so too. SetChangedChunkContaining can be used instead.
 */
void KtlQCanvas::setChangedChunk(int x, int y)
{
	if (validChunk(x,y)) {
		KtlQCanvasChunk& ch=chunk(x,y);
		ch.change();
	}
}

/*!
	\internal
	This method to informs the KtlQCanvas that the chunk containing a given
	pixel is `dirty' and needs to be redrawn in the next Update.

	(\a x,\a y) is a pixel location.

	The item classes call this. Any new derived class of KtlQCanvasItem must
	do so too. SetChangedChunk can be used instead.
 */
void KtlQCanvas::setChangedChunkContaining(int x, int y)
{
	if ( onCanvas(x, y) ) {
		KtlQCanvasChunk& chunk=chunkContaining(x,y);
		chunk.change();
	}
}

/*!
	\internal
	This method adds the KtlQCanvasItem \a g to the list of those which need to be
	drawn if the given chunk at location ( \a x, \a y ) is redrawn. Like
	SetChangedChunk and SetChangedChunkContaining, this method marks the
	chunk as `dirty'.
 */
void KtlQCanvas::addItemToChunk(KtlQCanvasItem* g, int x, int y)
{
	if (validChunk(x, y) ) {
		chunk(x,y).add(g);
	}
}

/*!
	\internal
	This method removes the KtlQCanvasItem \a g from the list of those which need to
	be drawn if the given chunk at location ( \a x, \a y ) is redrawn. Like
	SetChangedChunk and SetChangedChunkContaining, this method marks the chunk
	as `dirty'.
 */
void KtlQCanvas::removeItemFromChunk(KtlQCanvasItem* g, int x, int y)
{
	if (validChunk(x,y)) {
		chunk(x,y).remove(g);
	}
}

/*!
	\internal
	This method adds the KtlQCanvasItem \a g to the list of those which need to be
	drawn if the chunk containing the given pixel ( \a x, \a y ) is redrawn. Like
	SetChangedChunk and SetChangedChunkContaining, this method marks the
	chunk as `dirty'.
 */
void KtlQCanvas::addItemToChunkContaining(KtlQCanvasItem* g, int x, int y)
{
	if ( onCanvas( x, y ) ) {
		chunkContaining(x,y).add(g);
	}
}

/*!
	\internal
	This method removes the KtlQCanvasItem \a g from the list of those which need to
	be drawn if the chunk containing the given pixel ( \a x, \a y ) is redrawn.
	Like SetChangedChunk and SetChangedChunkContaining, this method
	marks the chunk as `dirty'.
 */
void KtlQCanvas::removeItemFromChunkContaining(KtlQCanvasItem* g, int x, int y)
{
	if ( onCanvas( x, y ) ) {
		chunkContaining(x,y).remove(g);
	}
}

/*!
	Returns the color set by setBackgroundColor(). By default, this is
	white.

	This function is not a reimplementation of
	QWidget::backgroundColor() (KtlQCanvas is not a subclass of QWidget),
	but all QCanvasViews that are viewing the canvas will set their
	backgrounds to this color.

	\sa setBackgroundColor(), backgroundPixmap()
 */
QColor KtlQCanvas::backgroundColor() const
{
	return bgcolor;
}

/*!
	Sets the solid background to be the color \a c.

	\sa backgroundColor(), setBackgroundPixmap(), setTiles()
 */
void KtlQCanvas::setBackgroundColor( const QColor& c )
{
	if ( bgcolor != c ) {
		bgcolor = c;
		KtlQCanvasView* view=m_viewList.first();
		while ( view != 0 ) {
	    /* XXX this doesn't look right. Shouldn't this
			be more like setBackgroundPixmap? : Ian */
			view->viewport()->setEraseColor( bgcolor );
			view=m_viewList.next();
		}
		setAllChanged();
	}
}

/*!
	Returns the pixmap set by setBackgroundPixmap(). By default,
	this is a null pixmap.

	\sa setBackgroundPixmap(), backgroundColor()
 */
QPixmap KtlQCanvas::backgroundPixmap() const
{
	return pm;
}

/*!
	Sets the solid background to be the pixmap \a p repeated as
	necessary to cover the entire canvas.

	\sa backgroundPixmap(), setBackgroundColor(), setTiles()
 */
void KtlQCanvas::setBackgroundPixmap( const QPixmap& p )
{
	setTiles(p, 1, 1, p.width(), p.height());
	KtlQCanvasView* view = m_viewList.first();
	while ( view != 0 ) {
		view->updateContents();
		view = m_viewList.next();
	}
}

/*!
	This virtual function is called for all updates of the canvas. It
	renders any background graphics using the painter \a painter, in
	the area \a clip. If the canvas has a background pixmap or a tiled
	background, that graphic is used, otherwise the canvas is cleared
	using the background color.

	If the graphics for an area change, you must explicitly call
	setChanged(const QRect&) for the result to be visible when
	update() is next called.

	\sa setBackgroundColor(), setBackgroundPixmap(), setTiles()
 */
void KtlQCanvas::drawBackground(QPainter& painter, const QRect& clip)
{
	painter.fillRect( clip, Qt::white );
	
	if ( pm.isNull() )
		painter.fillRect(clip,bgcolor);
	
	else if ( !grid ) {
		for (int x=clip.x()/pm.width();
				   x<(clip.x()+clip.width()+pm.width()-1)/pm.width(); x++)
		{
			for (int y=clip.y()/pm.height();
						  y<(clip.y()+clip.height()+pm.height()-1)/pm.height(); y++)
			{
				painter.drawPixmap(x*pm.width(), y*pm.height(),pm);
			}
		}
	} else {
		const int x1 = roundDown( clip.left(), tilew );
		int x2 = roundDown( clip.right(), tilew );
		const int y1 = roundDown( clip.top(), tileh );
		int y2 = roundDown( clip.bottom(), tileh );

		const int roww = pm.width()/tilew;

		for (int j=y1; j<=y2; j++) {
			int tv = tilesVertically();
			int jj = ((j%tv)+tv)%tv;
			for (int i=x1; i<=x2; i++)
			{
				int th = tilesHorizontally();
				int ii = ((i%th)+th)%th;
				int t = tile(ii,jj);
				int tx = t % roww;
				int ty = t / roww;
				painter.drawPixmap( i*tilew, j*tileh, pm,
									tx*tilew, ty*tileh, tilew, tileh );
			}
		}
	}
}

void KtlQCanvas::drawForeground(QPainter& painter, const QRect& clip)
{
	if ( debug_redraw_areas )
	{
		painter.setPen( Qt::red );
		painter.setBrush(Qt::NoBrush);
		painter.drawRect(clip);
	}
}

void KtlQCanvas::setTiles( QPixmap p, int h, int v, int tilewidth, int tileheight )
{
	if ( !p.isNull() && (!tilewidth || !tileheight ||
			 p.width() % tilewidth != 0 || p.height() % tileheight != 0 ) )
		return;

	htiles = h;
	vtiles = v;
	delete[] grid;
	pm = p;
	if ( h && v && !p.isNull() ) {
		grid = new ushort[h*v];
		memset( grid, 0, h*v*sizeof(ushort) );
		tilew = tilewidth;
		tileh = tileheight;
	} else {
		grid = 0;
	}
	if ( h + v > 10 ) {
		int s = scm(tilewidth,tileheight);
		retune( s < 128 ? s : QMAX(tilewidth,tileheight) );
	}
	setAllChanged();
}


void KtlQCanvas::setTile( int x, int y, int tilenum )
{
	ushort& t = grid[x+y*htiles];
	if ( t != tilenum ) {
		t = tilenum;
		if ( tilew == tileh && tilew == chunksize )
			setChangedChunk( x, y );	    // common case
		else	setChanged( QRect(x*tilew,y*tileh,tilew,tileh) );
	}
}


KtlQCanvasItem::KtlQCanvasItem(KtlQCanvas* canvas)
	: val(false), myx(0), myy(0), myz(0), cnv(canvas),
	 ext(0), m_bNeedRedraw(true), vis(false), sel(false)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
	if (cnv) cnv->addItem(this);
}


KtlQCanvasItem::~KtlQCanvasItem()
{
	if (cnv) cnv->removeItem(this);
	delete ext;
}


KtlQCanvasItemExtra& KtlQCanvasItem::extra()
{
	if ( !ext )
		ext = new KtlQCanvasItemExtra;
	return *ext;
}


void KtlQCanvasItem::setZ(double a)
{
	if ( myz == a )
		return;
	
	// remove and then add the item so that z-ordered list in canvas is updated
	
	if (cnv)
		cnv->removeItem(this);

	myz=a;
	changeChunks();

	if (cnv)
		cnv->addItem(this);
}


void KtlQCanvasItem::moveBy(const double dx, const double dy )
{
	if ( dx || dy ) {
		removeFromChunks();
		myx += dx;
		myy += dy;
		addToChunks();
	}
}


void KtlQCanvasItem::move(const double x, const double y )
{
	moveBy( x-myx, y-myy );
}


void KtlQCanvasItem::setCanvas(KtlQCanvas* c)
{
	bool v=isVisible();
	setVisible(false);
	if (cnv) {
		cnv->removeItem(this);
	}

	cnv=c;

	if (cnv) {
		cnv->addItem(this);
	}

	setVisible(v);
}


void KtlQCanvasItem::show()
{
	setVisible(true);
}

void KtlQCanvasItem::hide()
{
	setVisible(false);
}

void KtlQCanvasItem::setVisible(bool yes)
{
	if (vis != yes) {
		if (yes) {
			vis=(uint)yes;
			addToChunks();
		} else {
			removeFromChunks();
			vis=(uint)yes;
		}
	}
}


void KtlQCanvasItem::setSelected(const bool yes)
{
	if ((bool)sel!=yes) {
		sel=(uint)yes;
		changeChunks();
	}
}


static bool collision_double_dispatch( const KtlQCanvasPolygonalItem* p1,
					const KtlQCanvasRectangle* r1,
					const KtlQCanvasEllipse* e1,
					const KtlQCanvasPolygonalItem* p2,
					const KtlQCanvasRectangle* r2,
					const KtlQCanvasEllipse* e2 )
{
    const KtlQCanvasItem *i1 = NULL;
    if (p1) {
        i1 = p1;
    } else {
        if (r1) {
            i1 = r1;
        } else {
            i1 = e1;
        }
    }
    const KtlQCanvasItem *i2 = NULL;
    if (i2) {
        i2 = p2;
    } else {
        if (r2) {
            i2 = r2;
        } else {
            i2 = e2;
        }
    }
// 	const KtlQCanvasItem* i1 = p1 ?
// 			(const KtlQCanvasItem*)p1 : r1 ?
// 			(const KtlQCanvasItem*)r1 : (const KtlQCanvasItem*)e1;
// 	const KtlQCanvasItem* i2 = p2 ?
// 			(const KtlQCanvasItem*)p2 : r2 ?
// 			(const KtlQCanvasItem*)r2 : (const KtlQCanvasItem*)e2;

	if ( r1 && r2 )
	{
		// b
		QRect rc1 = i1->boundingRect();
		QRect rc2 = i2->boundingRect();
		return rc1.intersects(rc2);
	} else if ( e1 && e2
			&& e1->angleLength()>=360*16 && e2->angleLength()>=360*16
			&& e1->width()==e1->height()
			&& e2->width()==e2->height() )
	{
		// c
		double xd = (e1->x())-(e2->x());
		double yd = (e1->y())-(e2->y());
		double rd = (e1->width()+e2->width())/2;
		return xd*xd+yd*yd <= rd*rd;
	} else if ( p1 && p2 ) {
		// d
		Q3PointArray pa1 = p1->areaPoints();
		Q3PointArray pa2 = p2 ? p2->areaPoints()
			: Q3PointArray(i2->boundingRect());
		bool col= !(QRegion(pa1) & QRegion(pa2,true)).isEmpty();

		return col;
	} else {
		return collision_double_dispatch(p2,r2,e2,p1,r1,e1);
	}
}

bool KtlQCanvasPolygonalItem::collidesWith( const KtlQCanvasItem* i ) const
{
	return i->collidesWith(this,0,0);
}

bool KtlQCanvasPolygonalItem::collidesWith( const KtlQCanvasPolygonalItem* p, const KtlQCanvasRectangle* r, const KtlQCanvasEllipse* e ) const
{
	return collision_double_dispatch(p,r,e,this,0,0);
}

bool KtlQCanvasRectangle::collidesWith( const KtlQCanvasItem* i ) const
{
	return i->collidesWith(this,this,0);
}

bool KtlQCanvasRectangle::collidesWith( const KtlQCanvasPolygonalItem* p, const KtlQCanvasRectangle* r, const KtlQCanvasEllipse* e ) const
{
	return collision_double_dispatch(p,r,e,this,this,0);
}

bool KtlQCanvasEllipse::collidesWith( const KtlQCanvasItem* i ) const
{
	return i->collidesWith(this,0,this);
}

bool KtlQCanvasEllipse::collidesWith( const KtlQCanvasPolygonalItem* p, const KtlQCanvasRectangle* r, const KtlQCanvasEllipse* e ) const
{
	return collision_double_dispatch(p,r,e,this,0,this);
}

KtlQCanvasItemList KtlQCanvasItem::collisions(const bool exact) const
{
	return canvas()->collisions(chunks(),this,exact);
}

KtlQCanvasItemList KtlQCanvas::collisions(const QPoint& p) /* const */
{
	return collisions(QRect(p,QSize(1,1)));
}

KtlQCanvasItemList KtlQCanvas::collisions(const QRect& r) /* const */
{
	KtlQCanvasRectangle *i = new KtlQCanvasRectangle(r, /*(KtlQCanvas*) */ this); // TODO verify here, why is crashing ?!
	i->setPen( QPen( Qt::NoPen) );
	i->show(); // doesn't actually show, since we destroy it
	KtlQCanvasItemList l = i->collisions(true);
    delete  i;
	l.sort();
	return l;
}


KtlQCanvasItemList KtlQCanvas::collisions(const Q3PointArray& chunklist, const KtlQCanvasItem* item, bool exact) const
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " test item: " << item;
        for (SortedCanvasItems::const_iterator itIt = m_canvasItems.begin(); itIt != m_canvasItems.end(); ++itIt) {
            const KtlQCanvasItem *i = itIt->second;
            qDebug() << "   in canvas item: " << i;
        }
        qDebug() << "end canvas item list";
    }

	//Q3PtrDict<void> seen;
    QHash<KtlQCanvasItem*, bool> seen;
	KtlQCanvasItemList result;
	for (int i=0; i<(int)chunklist.count(); i++) {
		int x = chunklist[i].x();
		int y = chunklist[i].y();
		if ( validChunk(x,y) ) {
			const KtlQCanvasItemList* l = chunk(x,y).listPtr();
			for (KtlQCanvasItemList::ConstIterator it=l->begin(); it!=l->end(); ++it) {
				KtlQCanvasItem *g=*it;
				if ( g != item ) {
					//if ( !seen.find(g) ) {
                    if ( seen.find(g) == seen.end() ) {
						//seen.replace(g,(void*)1);
                        seen.take(g);
                        seen.insert(g, true);
						//if ( !exact || item->collidesWith(g) )
						//	result.append(g);
                        if (!exact) {
                            result.append(g);
                        }
                        if (isCanvasDebugEnabled()) {
                            qDebug() <<"test collides " << item << " with " << g;
                        }
                        if (item->collidesWith(g)) {
                            result.append(g);
                        }
					}
				}
			}
		}
	}
	return result;
}

void KtlQCanvasItem::addToChunks()
{
	if (isVisible() && canvas()) {
		Q3PointArray pa = chunks();
		for (int i=0; i<(int)pa.count(); i++)
			canvas()->addItemToChunk(this,pa[i].x(),pa[i].y());
		val = true;
	}
}


void KtlQCanvasItem::removeFromChunks()
{
	if (isVisible() && canvas()) {
		Q3PointArray pa = chunks();
		for (int i=0; i<(int)pa.count(); i++)
			canvas()->removeItemFromChunk(this,pa[i].x(),pa[i].y());
	}
}


void KtlQCanvasItem::changeChunks()
{
	if (isVisible() && canvas()) {
		if (!val)
			addToChunks();
		Q3PointArray pa = chunks();
		for (int i=0; i<(int)pa.count(); i++)
			canvas()->setChangedChunk(pa[i].x(),pa[i].y());
	}
}

Q3PointArray KtlQCanvasItem::chunks() const
{
	Q3PointArray r;
	int n=0;
	QRect br = boundingRect();
	if (isVisible() && canvas())
	{
		br &= canvas()->rect();
		if ( br.isValid() ) {
			r.resize((canvas()->toChunkScaling( br.width() )+2)*(canvas()->toChunkScaling( br.height() )+2));
			for (int j=canvas()->toChunkScaling(br.top()); j<=canvas()->toChunkScaling(br.bottom()); j++) {
				for (int i=canvas()->toChunkScaling(br.left()); i<=canvas()->toChunkScaling(br.right()); i++) {
					r[n++] = QPoint(i,j);
				}
			}
		}
	}
	r.resize(n);
	return r;
}

KtlQCanvasView::KtlQCanvasView(QWidget* parent, const char* name, Qt::WFlags f)
	: Q3ScrollView(parent,name,f|Qt::WResizeNoErase|Qt::WStaticContents)
{
	d = new KtlQCanvasViewData;
	viewing = 0;
	setCanvas(0);
	connect(this,SIGNAL(contentsMoving(int,int)),this,SLOT(cMoving(int,int)));
}

KtlQCanvasView::KtlQCanvasView(KtlQCanvas* canvas, QWidget* parent, const char* name, Qt::WFlags f)
	: Q3ScrollView(parent,name,f|Qt::WResizeNoErase|Qt::WStaticContents)
{
	d = new KtlQCanvasViewData;
	viewing = 0;
	setCanvas(canvas);

	connect(this,SIGNAL(contentsMoving(int,int)),this,SLOT(cMoving(int,int)));
}


KtlQCanvasView::~KtlQCanvasView()
{
	delete d;
	setCanvas(0);
}


void KtlQCanvasView::setCanvas(KtlQCanvas* canvas)
{
	if (viewing) {
		disconnect(viewing);
		viewing->removeView(this);
	}
	viewing=canvas;
	if (viewing) {
		connect(viewing,SIGNAL(resized()), this, SLOT(updateContentsSize()));
		viewing->addView(this);
	}
	if ( d ) // called by d'tor
		updateContentsSize();
}


const QWMatrix &KtlQCanvasView::worldMatrix() const
{
	return d->xform;
}


const QWMatrix &KtlQCanvasView::inverseWorldMatrix() const
{
	return d->ixform;
}


bool KtlQCanvasView::setWorldMatrix( const QWMatrix & wm )
{
	bool ok = wm.isInvertible();
	if ( ok ) {
		d->xform = wm;
		d->ixform = wm.invert();
		updateContentsSize();
		viewport()->update();
	}
	return ok;
}

void KtlQCanvasView::updateContentsSize()
{
	if ( viewing ) {
		QRect br;
// 			br = d->xform.map(QRect(0,0,viewing->width(),viewing->height()));
		br = d->xform.map(viewing->rect());

		if ( br.width() < contentsWidth() ) {
			QRect r(contentsToViewport(QPoint(br.width(),0)),
					QSize(contentsWidth()-br.width(),contentsHeight()));
			viewport()->erase(r);
		}
		if ( br.height() < contentsHeight() ) {
			QRect r(contentsToViewport(QPoint(0,br.height())),
					QSize(contentsWidth(),contentsHeight()-br.height()));
			viewport()->erase(r);
		}

		resizeContents(br.width(),br.height());
	} else {
		viewport()->erase();
		resizeContents(1,1);
	}
}

void KtlQCanvasView::cMoving(int x, int y)
{
    // A little kludge to smooth up repaints when scrolling
	int dx = x - contentsX();
	int dy = y - contentsY();
	d->repaint_from_moving = abs(dx) < width() / 8 && abs(dy) < height() / 8;
}

/*!
	Repaints part of the KtlQCanvas that the canvas view is showing
	starting at \a cx by \a cy, with a width of \a cw and a height of \a
	ch using the painter \a p.

	\warning When double buffering is enabled, drawContents() will
	not respect the current settings of the painter when setting up
	the painter for the double buffer (e.g., viewport() and
	window()). Also, be aware that KtlQCanvas::update() bypasses
	drawContents(), which means any reimplementation of
	drawContents() is not called.

	\sa setDoubleBuffering() 
 */
void KtlQCanvasView::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
	QRect r(cx,cy,cw,ch);
	r = r.normalize();
	
	if (viewing) {
	//viewing->drawViewArea(this,p,r,true);
		viewing->drawViewArea(this,p,r,!d->repaint_from_moving);
		d->repaint_from_moving = false;
	} else {
		p->eraseRect(r);
	}
}

/*!
	\reimp
	\internal

	(Implemented to get rid of a compiler warning.)
 */
void KtlQCanvasView::drawContents( QPainter * )
{
}

/*!
	Suggests a size sufficient to view the entire canvas.
 */
QSize KtlQCanvasView::sizeHint() const
{
	if ( !canvas() )
		return Q3ScrollView::sizeHint();
    // should maybe take transformations into account
	return ( canvas()->size() + 2 * QSize(frameWidth(), frameWidth()) )
			.boundedTo( 3 * QApplication::desktop()->size() / 4 );
}


/*
	Since most polygonal items don't have a pen, the default is
	NoPen and a black brush.
*/
static const QPen& defaultPolygonPen()
{
	static QPen* dp=0;
	if ( !dp )
		dp = new QPen;
	return *dp;
}

static const QBrush& defaultPolygonBrush()
{
	static QBrush* db=0;
	if ( !db )
		db = new QBrush;
	return *db;
}

KtlQCanvasPolygonalItem::KtlQCanvasPolygonalItem(KtlQCanvas* canvas)
	: KtlQCanvasItem(canvas),
	br(defaultPolygonBrush()),
	pn(defaultPolygonPen()), wind(false)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << "created KtlQCanvasPolygonalItem at " << this;
    }
}

KtlQCanvasPolygonalItem::~KtlQCanvasPolygonalItem()
{
    if (isCanvasDebugEnabled()) {
        qDebug() << "destroying KtlQCanvasPolygonalItem at " << this;
    }
}

bool KtlQCanvasPolygonalItem::winding() const
{
	return wind;
}

void KtlQCanvasPolygonalItem::setWinding(bool enable)
{
	wind = enable;
}

void KtlQCanvasPolygonalItem::invalidate()
{
	val = false;
	removeFromChunks();
}


Q3PointArray KtlQCanvasPolygonalItem::chunks() const
{
	Q3PointArray pa = areaPoints();

	if ( !pa.size() )
	{
		pa.detach(); // Explicit sharing is stupid.
		return pa;
	}

	KtlQPolygonalProcessor processor(canvas(),pa);

	scanPolygon(pa, wind, processor);

	return processor.result;
}

Q3PointArray KtlQCanvasRectangle::chunks() const
{
    // No need to do a polygon scan!
	return KtlQCanvasItem::chunks();
}


QRect KtlQCanvasPolygonalItem::boundingRect() const
{
	return areaPoints().boundingRect();
}

void KtlQCanvasPolygonalItem::draw(QPainter & p)
{
	p.setPen(pn);
	p.setBrush(br);
	drawShape(p);
}

void KtlQCanvasPolygonalItem::setPen( const QPen & p )
{
	if ( pn == p )
		return;
	
	pn.setColor( p.color() );
	
	// if only the color was different, then don't need to re-add to chunks
	if ( pn == p ) {
		changeChunks();
	} else {
		// Have to re-add to chunks as e.g. pen width might have changed
		removeFromChunks();
		pn = p;
		addToChunks();
	}
}


void KtlQCanvasPolygonalItem::setBrush( const QBrush & b )
{
	if ( br != b) {
		br = b;
		changeChunks();
	}
}


KtlQCanvasPolygon::KtlQCanvasPolygon(KtlQCanvas* canvas)
	: KtlQCanvasPolygonalItem(canvas)
    , guardBef()
    , poly(new Q3PointArray)
    , guardAft()
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}


KtlQCanvasPolygon::~KtlQCanvasPolygon()
{
	hide();
    delete poly;
}


void KtlQCanvasPolygon::drawShape(QPainter & p)
{
    // ### why can't we draw outlines? We could use drawPolyline for it. Lars
    // ### see other message. Warwick

    p.setPen(Qt::NoPen); // since QRegion(Q3PointArray) excludes outline :-(  )-:
	p.drawPolygon(*poly);
}


void KtlQCanvasPolygon::setPoints(Q3PointArray pa)
{
	removeFromChunks();
	*poly = pa;
	poly->detach(); // Explicit sharing is stupid.
	poly->translate((int)x(),(int)y());
	addToChunks();
}


void KtlQCanvasPolygon::moveBy(double dx, double dy)
{
    // Note: does NOT call KtlQCanvasPolygonalItem::moveBy(), since that
    // only does half this work.
	//
	int idx = int(x()+dx)-int(x());
	int idy = int(y()+dy)-int(y());
	if ( idx || idy ) {
		removeFromChunks();
		poly->translate(idx,idy);
	}
	myx+=dx;
	myy+=dy;
	if ( idx || idy ) {
		addToChunks();
	}
}

Q3PointArray KtlQCanvasPolygon::points() const
{
	Q3PointArray pa = areaPoints();
	pa.translate(int(-x()),int(-y()));
	return pa;
}

Q3PointArray KtlQCanvasPolygon::areaPoints() const
{
	return poly->copy();
}

// ### mark: Why don't we offer a constructor that lets the user set the
// points -- that way for some uses just the constructor call would be
// required?

KtlQCanvasLine::KtlQCanvasLine(KtlQCanvas* canvas)
	: KtlQCanvasPolygonalItem(canvas)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
	x1 = y1 = x2 = y2 = 0;
}

KtlQCanvasLine::~KtlQCanvasLine()
{
	hide();
}

void KtlQCanvasLine::setPen( const QPen & p )
{
	KtlQCanvasPolygonalItem::setPen(p);
}

void KtlQCanvasLine::setPoints(int xa, int ya, int xb, int yb)
{
	if ( x1 != xa || x2 != xb || y1 != ya || y2 != yb ) {
		removeFromChunks();
		x1 = xa;
		y1 = ya;
		x2 = xb;
		y2 = yb;
		addToChunks();
	}
}

void KtlQCanvasLine::drawShape(QPainter &p)
{
	p.drawLine((int)(x()+x1), (int)(y()+y1), (int)(x()+x2), (int)(y()+y2));
}

Q3PointArray KtlQCanvasLine::areaPoints() const
{
	Q3PointArray p(4);
	int xi = int(x());
	int yi = int(y());
	int pw = pen().width();
	int dx = abs(x1-x2);
	int dy = abs(y1-y2);
	pw = pw*4/3+2; // approx pw*sqrt(2)
	int px = x1<x2 ? -pw : pw ;
	int py = y1<y2 ? -pw : pw ;
	if ( dx && dy && (dx > dy ? (dx*2/dy <= 2) : (dy*2/dx <= 2)) ) {
	// steep
		if ( px == py ) {
			p[0] = QPoint(x1+xi   ,y1+yi+py);
			p[1] = QPoint(x2+xi-px,y2+yi   );
			p[2] = QPoint(x2+xi   ,y2+yi-py);
			p[3] = QPoint(x1+xi+px,y1+yi   );
		} else {
			p[0] = QPoint(x1+xi+px,y1+yi   );
			p[1] = QPoint(x2+xi   ,y2+yi-py);
			p[2] = QPoint(x2+xi-px,y2+yi   );
			p[3] = QPoint(x1+xi   ,y1+yi+py);
		}
	} else if ( dx > dy ) {
	// horizontal
		p[0] = QPoint(x1+xi+px,y1+yi+py);
		p[1] = QPoint(x2+xi-px,y2+yi+py);
		p[2] = QPoint(x2+xi-px,y2+yi-py);
		p[3] = QPoint(x1+xi+px,y1+yi-py);
	} else {
	// vertical
		p[0] = QPoint(x1+xi+px,y1+yi+py);
		p[1] = QPoint(x2+xi+px,y2+yi-py);
		p[2] = QPoint(x2+xi-px,y2+yi-py);
		p[3] = QPoint(x1+xi-px,y1+yi+py);
	}
	return p;
}

void KtlQCanvasLine::moveBy(double dx, double dy)
{
	KtlQCanvasPolygonalItem::moveBy(dx, dy);
}


KtlQCanvasRectangle::KtlQCanvasRectangle(KtlQCanvas* canvas) :
		KtlQCanvasPolygonalItem(canvas),
    w(32), h(32)
{
    setName("KtlQCanvasRectangle");
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}


KtlQCanvasRectangle::KtlQCanvasRectangle(const QRect& r, KtlQCanvas* canvas) :
		KtlQCanvasPolygonalItem(canvas),
    w(r.width()), h(r.height())
{
    setName("KtlQCanvasRectangle");
	move(r.x(),r.y());
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}


KtlQCanvasRectangle::KtlQCanvasRectangle(int x, int y, int width, int height, KtlQCanvas* canvas)
	: KtlQCanvasPolygonalItem(canvas),
	w(width), h(height)
{
    setName("KtlQCanvasRectangle");
	move(x,y);
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}


KtlQCanvasRectangle::~KtlQCanvasRectangle()
{
	hide();
}


int KtlQCanvasRectangle::width() const
{
	return w;
}

int KtlQCanvasRectangle::height() const
{
	return h;
}


void KtlQCanvasRectangle::setSize(const int width, const int height)
{
	if ( w != width || h != height ) {
		removeFromChunks();
		w = width;
		h = height;
		addToChunks();
	}
}


Q3PointArray KtlQCanvasRectangle::areaPoints() const
{
	Q3PointArray pa(4);
	int pw = (pen().width()+1)/2;
	if ( pw < 1 ) pw = 1;
	if ( pen() == Qt::NoPen ) pw = 0;
	pa[0] = QPoint((int)x()-pw,(int)y()-pw);
	pa[1] = pa[0] + QPoint(w+pw*2,0);
	pa[2] = pa[1] + QPoint(0,h+pw*2);
	pa[3] = pa[0] + QPoint(0,h+pw*2);
	return pa;
}


void KtlQCanvasRectangle::drawShape(QPainter &p)
{
	p.drawRect((int)x(), (int)y(), w, h);
}


KtlQCanvasEllipse::KtlQCanvasEllipse(KtlQCanvas* canvas)
	: KtlQCanvasPolygonalItem(canvas),
	w(32), h(32),
	a1(0), a2(360*16)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}

/*!
	Constructs a \a width by \a height pixel ellipse, centered at
	(0, 0) on \a canvas.
 */
	KtlQCanvasEllipse::KtlQCanvasEllipse(int width, int height, KtlQCanvas* canvas)
	: KtlQCanvasPolygonalItem(canvas),
	w(width),h(height),
	a1(0),a2(360*16)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}


KtlQCanvasEllipse::KtlQCanvasEllipse(int width, int height,
    int startangle, int angle, KtlQCanvas* canvas) :
			KtlQCanvasPolygonalItem(canvas),
	w(width),h(height),
	a1(startangle),a2(angle)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}


KtlQCanvasEllipse::~KtlQCanvasEllipse()
{
	hide();
}


int KtlQCanvasEllipse::width() const
{
	return w;
}


int KtlQCanvasEllipse::height() const
{
	return h;
}


void KtlQCanvasEllipse::setSize(int width, int height)
{
	if ( w != width || h != height ) {
		removeFromChunks();
		w = width;
		h = height;
		addToChunks();
	}
}


void KtlQCanvasEllipse::setAngles(int start, int length)
{
	if ( a1 != start || a2 != length ) {
		removeFromChunks();
		a1 = start;
		a2 = length;
		addToChunks();
	}
}


Q3PointArray KtlQCanvasEllipse::areaPoints() const
{
	Q3PointArray r;
    // makeArc at 0,0, then translate so that fixed point math doesn't overflow
	r.makeArc(int(x()-w/2.0+0.5)-1, int(y()-h/2.0+0.5)-1, w+3, h+3, a1, a2);
	r.resize(r.size()+1);
	r.setPoint(r.size()-1,int(x()),int(y()));
	return r;
}


void KtlQCanvasEllipse::drawShape(QPainter & p)
{
    p.setPen(Qt::NoPen); // since QRegion(Q3PointArray) excludes outline :-(  )-:
	if ( !a1 && a2 == 360*16 ) {
		p.drawEllipse(int(x()-w/2.0+0.5), int(y()-h/2.0+0.5), w, h);
	} else {
		p.drawPie(int(x()-w/2.0+0.5), int(y()-h/2.0+0.5), w, h, a1, a2);
	}
}


void KtlQCanvasPolygonalItem::scanPolygon(const Q3PointArray& pa, int winding, KtlQPolygonalProcessor& process) const
{
	KtlQCanvasPolygonScanner scanner(process);
	scanner.scan(pa,winding);
}

#include "canvas.moc"
