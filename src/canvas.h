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

#include "Qt/q3scrollview.h"
#include "Qt/qpixmap.h"
#include "Qt/q3ptrlist.h"
#include "Qt/qbrush.h"
#include "Qt/qpen.h"
#include "Qt/qlist.h"
#include "Qt/q3pointarray.h"

class QCanvasPolygonalItem;
class QCanvasRectangle;
class QCanvasPolygon;
class QCanvasEllipse;
class QCanvasLine;
class QCanvasChunk;
class QCanvas;
class QCanvasItem;
class QCanvasView;

typedef std::multimap< double, QCanvasItem* > SortedCanvasItems;


class QCanvasItemList : public QList<QCanvasItem*>
{
	public:
		void sort();
		QCanvasItemList operator+(const QCanvasItemList &l) const;
};


class QCanvasItemExtra;

class QCanvasItem : public QObject
{
    Q_OBJECT
	public:
		QCanvasItem(QCanvas* canvas);
		virtual ~QCanvasItem();

		double x() const { return myx; }
		double y() const { return myy; }
		double z() const { return myz; } // (depth)

		virtual void moveBy(double const dx, double const dy);
		void move(double const x, double const y);
		void setX(double a) { move(a,y()); }
		void setY(double a) { move(x(),a); }
		void setZ(double a);

		virtual bool collidesWith( const QCanvasItem* ) const=0;

		QCanvasItemList collisions(const bool exact /* NO DEFAULT */ ) const;

		virtual void setCanvas(QCanvas*);

		virtual void draw(QPainter&)=0;

		void show();
		void hide();

		virtual void setVisible(bool yes);
		bool isVisible() const { return vis; }
		virtual void setSelected(const bool yes);
		bool isSelected() const { return sel; }

		virtual QRect boundingRect() const=0;

		QCanvas* canvas() const { return cnv; }
		
		virtual bool collidesWith( const QCanvasPolygonalItem*,
					   const QCanvasRectangle*,
					   const QCanvasEllipse* ) const = 0;

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
		QCanvas* cnv;
		static QCanvas* current_canvas;
		QCanvasItemExtra *ext;
		QCanvasItemExtra& extra();
		bool m_bNeedRedraw;
		bool vis;
		bool sel;

};

class QCanvas : public QObject
{
	Q_OBJECT
	public:
		QCanvas( QObject* parent = 0, const char* name = 0 );
		QCanvas( const int w, const int h);
		QCanvas( QPixmap p, int h, int v, int tilewidth, int tileheight );

		virtual ~QCanvas();

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
		void addItemToChunk(QCanvasItem*, int i, int j);
		void removeItemFromChunk(QCanvasItem*, int i, int j);
		void addItemToChunkContaining(QCanvasItem*, int x, int y);
		void removeItemFromChunkContaining(QCanvasItem*, int x, int y);

		QCanvasItemList allItems();
		QCanvasItemList collisions( const QPoint&) const;
		QCanvasItemList collisions( const QRect&) const;
		QCanvasItemList collisions( const Q3PointArray& pa, const QCanvasItem* item,
						bool exact) const;

		void drawArea(const QRect&, QPainter* p, bool double_buffer=false);

		// These are for QCanvasView to call
		virtual void addView(QCanvasView*);
		virtual void removeView(QCanvasView*);
		void drawCanvasArea(const QRect&, QPainter* p=0, bool double_buffer=true);
		void drawViewArea( QCanvasView* view, QPainter* p, const QRect& r, bool dbuf );

		// These are for QCanvasItem to call
		virtual void addItem(QCanvasItem*);
		virtual void removeItem(const QCanvasItem*);

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
		void setNeedRedraw( const QCanvasItemList * list );

		QPixmap offscr;
		int chunksize;
		int maxclusters;
		QRect m_size;
		QRect m_chunkSize;
		QCanvasChunk* chunks;

		SortedCanvasItems m_canvasItems;
		Q3PtrList<QCanvasView> m_viewList;

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

		friend void qt_unview(QCanvas* c);

		QCanvas( const QCanvas & );
		QCanvas &operator=( const QCanvas & );
};

class QCanvasViewData;

class QCanvasView : public Q3ScrollView
{
	Q_OBJECT
	public:

		QCanvasView(QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
		QCanvasView(QCanvas* viewing, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
		~QCanvasView();

		QCanvas* canvas() const
		{ return viewing; }
		void setCanvas(QCanvas* v);

		const QMatrix &worldMatrix() const;
		const QMatrix &inverseWorldMatrix() const;
		bool setWorldMatrix( const QMatrix & );

	protected:
		void drawContents( QPainter*, int cx, int cy, int cw, int ch );
		QSize sizeHint() const;

	private:
		void drawContents( QPainter* );
		QCanvas* viewing;
		QCanvasViewData* d;
		friend void qt_unview(QCanvas* c);
		QCanvasView( const QCanvasView & );
		QCanvasView &operator=( const QCanvasView & );

	private slots:
		void cMoving(int,int);
		void updateContentsSize();

};


class QPolygonalProcessor;

class QCanvasPolygonalItem : public QCanvasItem
{
	public:
		QCanvasPolygonalItem(QCanvas* canvas);
		virtual ~QCanvasPolygonalItem();

		bool collidesWith( const QCanvasItem* ) const;

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

		bool collidesWith( const QCanvasPolygonalItem*,
				   const QCanvasRectangle*,
				   const QCanvasEllipse* ) const;

		QBrush br;
		QPen pn;
		bool wind;
};


class QCanvasRectangle : public QCanvasPolygonalItem
{
	public:
		QCanvasRectangle(QCanvas* canvas);
		QCanvasRectangle(const QRect&, QCanvas* canvas);
		QCanvasRectangle(int x, int y, int width, int height, QCanvas* canvas);

		~QCanvasRectangle();

		int width() const;
		int height() const;
		void setSize(const int w, const int h);
		QSize size() const
		{ return QSize(w,h); }
		Q3PointArray areaPoints() const;
		QRect rect() const
		{ return QRect(int(x()),int(y()),w,h); }

		bool collidesWith( const QCanvasItem* ) const;

	protected:
		void drawShape(QPainter &);
		Q3PointArray chunks() const;

	private:
		bool collidesWith( const QCanvasPolygonalItem*,
				   const QCanvasRectangle*,
				   const QCanvasEllipse* ) const;

		int w, h;
};


class QCanvasPolygon : public QCanvasPolygonalItem
{
	public:
		QCanvasPolygon(QCanvas* canvas);
		~QCanvasPolygon();
		void setPoints(Q3PointArray);
		Q3PointArray points() const;
		void moveBy(double dx, double dy);

		Q3PointArray areaPoints() const;

	protected:
		void drawShape(QPainter &);
		Q3PointArray poly;
};


class QCanvasLine : public QCanvasPolygonalItem
{
	public:
		QCanvasLine(QCanvas* canvas);
		~QCanvasLine();
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


class QCanvasEllipse : public QCanvasPolygonalItem
{

	public:
		QCanvasEllipse( QCanvas* canvas );
		QCanvasEllipse( int width, int height, QCanvas* canvas );
		QCanvasEllipse( int width, int height, int startangle, int angle,
					QCanvas* canvas );

		~QCanvasEllipse();

		int width() const;
		int height() const;
		void setSize(int w, int h);
		void setAngles(int start, int length);
		int angleStart() const { return a1; }
		int angleLength() const { return a2; }
		Q3PointArray areaPoints() const;

		bool collidesWith( const QCanvasItem* ) const;

	protected:
		void drawShape(QPainter &);

	private:
		bool collidesWith( const QCanvasPolygonalItem*,
				   const QCanvasRectangle*,
				   const QCanvasEllipse* ) const;
		int w, h;
		int a1, a2;
};


#endif // QCANVAS_H
