/***************************************************************************
 *   Copyright (C) 1999-2005 Trolltech AS                                  *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This file may be distributed and/or modified under the terms of the   *
 *   GNU General Public License version 2 as published by the Free         *
 *   Software Foundation                                                   *
 ***************************************************************************/

#ifndef QCANVAS_H
#define QCANVAS_H

#include <map>

#include "q3scrollview.h"
#include "qpixmap.h"
#include "q3ptrlist.h"
#include "qbrush.h"
#include "qpen.h"
#include "q3valuelist.h"
#include "q3pointarray.h"

class Q3CanvasPolygonalItem;
class Q3CanvasRectangle;
class Q3CanvasPolygon;
class Q3CanvasEllipse;
class Q3CanvasLine;
class QCanvasChunk;
class Q3Canvas;
class Q3CanvasItem;
class Q3CanvasView;

typedef std::multimap< double, Q3CanvasItem* > SortedCanvasItems;


class Q3CanvasItemList : public Q3ValueList<Q3CanvasItem*>
{
	public:
		void sort();
		Q3CanvasItemList operator+(const Q3CanvasItemList &l) const;
};


class QCanvasItemExtra;

class Q3CanvasItem : public QObject
{
	public:
		Q3CanvasItem(Q3Canvas* canvas);
		virtual ~Q3CanvasItem();

		double x() const { return myx; }
		double y() const { return myy; }
		double z() const { return myz; } // (depth)

		virtual void moveBy(double const dx, double const dy);
		void move(double const x, double const y);
		void setX(double a) { move(a,y()); }
		void setY(double a) { move(x(),a); }
		void setZ(double a);

		virtual bool collidesWith( const Q3CanvasItem* ) const=0;

		Q3CanvasItemList collisions(const bool exact /* NO DEFAULT */ ) const;

		virtual void setCanvas(Q3Canvas*);

		virtual void draw(QPainter&)=0;

		void show();
		void hide();

		virtual void setVisible(bool yes);
		bool isVisible() const { return vis; }
		virtual void setSelected(const bool yes);
		bool isSelected() const { return sel; }

		virtual QRect boundingRect() const=0;

		Q3Canvas* canvas() const { return cnv; }
		
		virtual bool collidesWith( const Q3CanvasPolygonalItem*,
					   const Q3CanvasRectangle*,
					   const Q3CanvasEllipse* ) const = 0;

		bool needRedraw() const { return m_bNeedRedraw; }
		void setNeedRedraw( const bool needRedraw ) { m_bNeedRedraw = needRedraw; }

	protected:
		void update() { changeChunks(); }

		virtual Q3PointArray chunks() const;
		virtual void addToChunks();
		virtual void removeFromChunks();
		virtual void changeChunks();
		
		bool val;
		double myx,myy,myz;

	private:
		Q3Canvas* cnv;
		static Q3Canvas* current_canvas;
		QCanvasItemExtra *ext;
		QCanvasItemExtra& extra();
		bool m_bNeedRedraw;
		bool vis;
		bool sel;

};

class Q3Canvas : public QObject
{
	Q_OBJECT
	public:
		Q3Canvas( QObject* parent = 0, const char* name = 0 );
		Q3Canvas( const int w, const int h);
		Q3Canvas( QPixmap p, int h, int v, int tilewidth, int tileheight );

		virtual ~Q3Canvas();

		virtual void setTiles( QPixmap tiles, int h, int v,
							   int tilewidth, int tileheight );
		virtual void setBackgroundPixmap( const QPixmap& p );
		QPixmap backgroundPixmap() const;

		virtual void setBackgroundColor( const QColor& c );
		QColor backgroundColor() const;

		virtual void setTile( int x, int y, int tilenum );
		int tile( int x, int y ) const { return grid[x+y*htiles]; }

		int tilesHorizontally() const { return htiles; }
		int tilesVertically() const { return vtiles; }

		int tileWidth()  const { return tilew; }
		int tileHeight() const { return tileh; }

		virtual void resize( const QRect & newSize );
		int width()  const { return size().width(); }
		int height() const { return size().height(); }
		QSize size() const { return m_size.size(); }
		QRect rect() const { return m_size; }
		bool onCanvas( const int x, const int y ) const { return onCanvas( QPoint( x, y ) ); }
		bool onCanvas( const QPoint& p ) const { return m_size.contains( p ); }
		bool validChunk( const int x, const int y ) const { return validChunk( QPoint( x, y ) ); }
		bool validChunk( const QPoint& p ) const { return m_chunkSize.contains( p ); }

		int chunkSize() const { return chunksize; }
		virtual void retune(int chunksize, int maxclusters=100);
		virtual void setChangedChunk(int i, int j);
		virtual void setChangedChunkContaining(int x, int y);
		virtual void setAllChanged();
		virtual void setChanged(const QRect& area);
		virtual void setUnchanged(const QRect& area);

		// These call setChangedChunk.
		void addItemToChunk(Q3CanvasItem*, int i, int j);
		void removeItemFromChunk(Q3CanvasItem*, int i, int j);
		void addItemToChunkContaining(Q3CanvasItem*, int x, int y);
		void removeItemFromChunkContaining(Q3CanvasItem*, int x, int y);

		Q3CanvasItemList allItems();
		Q3CanvasItemList collisions( const QPoint&) const;
		Q3CanvasItemList collisions( const QRect&) const;
		Q3CanvasItemList collisions( const Q3PointArray& pa, const Q3CanvasItem* item,
						bool exact) const;

		void drawArea(const QRect&, QPainter* p, bool double_buffer=false);

		// These are for QCanvasView to call
		virtual void addView(Q3CanvasView*);
		virtual void removeView(Q3CanvasView*);
		void drawCanvasArea(const QRect&, QPainter* p=0, bool double_buffer=true);
		void drawViewArea( Q3CanvasView* view, QPainter* p, const QRect& r, bool dbuf );

		// These are for QCanvasItem to call
		virtual void addItem(Q3CanvasItem*);
		virtual void removeItem(const Q3CanvasItem*);

		virtual void setUpdatePeriod(int ms);
		int toChunkScaling( int x ) const;

	signals:
		void resized();

	public slots:
		virtual void advance();
		virtual void update();

	protected:
		virtual void drawBackground(QPainter&, const QRect& area);
		virtual void drawForeground(QPainter&, const QRect& area);

	private:
		void init(int w, int h, int chunksze=16, int maxclust=100);
		void init(const QRect & r, int chunksze=16, int maxclust=100);
		void initChunkSize( const QRect & s );

		QCanvasChunk& chunk(int i, int j) const;
		QCanvasChunk& chunkContaining(int x, int y) const;

		QRect changeBounds(const QRect& inarea);
		void drawChanges(const QRect& inarea);
		void drawChangedItems( QPainter & painter );
		void setNeedRedraw( const Q3CanvasItemList * list );

		QPixmap offscr;
		int chunksize;
		int maxclusters;
		QRect m_size;
		QRect m_chunkSize;
		QCanvasChunk* chunks;

		SortedCanvasItems m_canvasItems;
		Q3PtrList<Q3CanvasView> m_viewList;

		void initTiles(QPixmap p, int h, int v, int tilewidth, int tileheight);
		ushort *grid;
		ushort htiles;
		ushort vtiles;
		ushort tilew;
		ushort tileh;
		bool oneone;
		QPixmap pm;
		QTimer* update_timer;
		QColor bgcolor;
		bool debug_redraw_areas;

		friend void qt_unview(Q3Canvas* c);

		Q3Canvas( const Q3Canvas & );
		Q3Canvas &operator=( const Q3Canvas & );
};

class QCanvasViewData;

class Q3CanvasView : public Q3ScrollView
{
	Q_OBJECT
	public:

		Q3CanvasView(QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
		Q3CanvasView(Q3Canvas* viewing, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
		~Q3CanvasView();

		Q3Canvas* canvas() const
		{ return viewing; }
		void setCanvas(Q3Canvas* v);

		const QMatrix &worldMatrix() const;
		const QMatrix &inverseWorldMatrix() const;
		bool setWorldMatrix( const QMatrix & );

	protected:
		void drawContents( QPainter*, int cx, int cy, int cw, int ch );
		QSize sizeHint() const;

	private:
		void drawContents( QPainter* );
		Q3Canvas* viewing;
		QCanvasViewData* d;
		friend void qt_unview(Q3Canvas* c);
		Q3CanvasView( const Q3CanvasView & );
		Q3CanvasView &operator=( const Q3CanvasView & );

	private slots:
		void cMoving(int,int);
		void updateContentsSize();

};


class QPolygonalProcessor;

class Q3CanvasPolygonalItem : public Q3CanvasItem
{
	public:
		Q3CanvasPolygonalItem(Q3Canvas* canvas);
		virtual ~Q3CanvasPolygonalItem();

		bool collidesWith( const Q3CanvasItem* ) const;

		virtual void setPen( const QPen & p );
		virtual void setBrush( const QBrush & b );

		QPen pen() const
		{ return pn; }
		QBrush brush() const
		{ return br; }

		virtual Q3PointArray areaPoints() const=0;
		QRect boundingRect() const;

	protected:
		void draw(QPainter &);
		virtual void drawShape(QPainter &) = 0;

		bool winding() const;
		void setWinding(bool);

		void invalidate();
		bool isValid() const
		{ return (bool)val; }

	private:
		void scanPolygon( const Q3PointArray& pa, int winding,
					QPolygonalProcessor& process ) const;

		Q3PointArray chunks() const;

		bool collidesWith( const Q3CanvasPolygonalItem*,
				   const Q3CanvasRectangle*,
				   const Q3CanvasEllipse* ) const;

		QBrush br;
		QPen pn;
		bool wind;
};


class Q3CanvasRectangle : public Q3CanvasPolygonalItem
{
	public:
		Q3CanvasRectangle(Q3Canvas* canvas);
		Q3CanvasRectangle(const QRect&, Q3Canvas* canvas);
		Q3CanvasRectangle(int x, int y, int width, int height, Q3Canvas* canvas);

		~Q3CanvasRectangle();

		int width() const;
		int height() const;
		void setSize(const int w, const int h);
		QSize size() const
		{ return QSize(w,h); }
		Q3PointArray areaPoints() const;
		QRect rect() const
		{ return QRect(int(x()),int(y()),w,h); }

		bool collidesWith( const Q3CanvasItem* ) const;

	protected:
		void drawShape(QPainter &);
		Q3PointArray chunks() const;

	private:
		bool collidesWith( const Q3CanvasPolygonalItem*,
				   const Q3CanvasRectangle*,
				   const Q3CanvasEllipse* ) const;
		int w, h;
};

class Q3CanvasPolygon : public Q3CanvasPolygonalItem
{
	public:
		Q3CanvasPolygon(Q3Canvas* canvas);
		~Q3CanvasPolygon();
		void setPoints(Q3PointArray);
		Q3PointArray points() const;
		void moveBy(double dx, double dy);

		Q3PointArray areaPoints() const;

	protected:
		void drawShape(QPainter &);
		Q3PointArray poly;
};

class Q3CanvasLine : public Q3CanvasPolygonalItem
{
	public:
		Q3CanvasLine(Q3Canvas* canvas);
		~Q3CanvasLine();
		void setPoints(int x1, int y1, int x2, int y2);

		QPoint startPoint() const
		{ return QPoint(x1,y1); }
		QPoint endPoint() const
		{ return QPoint(x2,y2); }

		void setPen( const QPen & p );
		void moveBy(double dx, double dy);

	protected:
		void drawShape(QPainter &);
		Q3PointArray areaPoints() const;

	private:
		int x1,y1,x2,y2;
};


class Q3CanvasEllipse : public Q3CanvasPolygonalItem
{

	public:
		Q3CanvasEllipse( Q3Canvas* canvas );
		Q3CanvasEllipse( int width, int height, Q3Canvas* canvas );
		Q3CanvasEllipse( int width, int height, int startangle, int angle,
					Q3Canvas* canvas );

		~Q3CanvasEllipse();

		int width() const;
		int height() const;
		void setSize(int w, int h);
		void setAngles(int start, int length);
		int angleStart() const { return a1; }
		int angleLength() const { return a2; }
		Q3PointArray areaPoints() const;

		bool collidesWith( const Q3CanvasItem* ) const;

	protected:
		void drawShape(QPainter &);

	private:
		bool collidesWith( const Q3CanvasPolygonalItem*,
				   const Q3CanvasRectangle*,
				   const Q3CanvasEllipse* ) const;
		int w, h;
		int a1, a2;
};


#endif // QCANVAS_H
