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
#include "qlist.h"
#include "q3pointarray.h"

class KtlQCanvasPolygonalItem;
class KtlQCanvasRectangle;
class KtlQCanvasPolygon;
class KtlQCanvasEllipse;
class KtlQCanvasLine;
class KtlQCanvasChunk;
class KtlQCanvas;
class KtlQCanvasItem;
class KtlQCanvasView;

typedef std::multimap< double, KtlQCanvasItem* > SortedCanvasItems;


class KtlQCanvasItemList : public QList<KtlQCanvasItem*>
{
	public:
		void sort();
		KtlQCanvasItemList operator+(const KtlQCanvasItemList &l) const;
};


class KtlQCanvasItemExtra;

class KtlQCanvasItem : public QObject
{
    Q_OBJECT
	public:
		KtlQCanvasItem(KtlQCanvas* canvas);
		virtual ~KtlQCanvasItem();

		double x() const { return myx; }
		double y() const { return myy; }
		double z() const { return myz; } // (depth)

		virtual void moveBy(double const dx, double const dy);
		void move(double const x, double const y);
		void setX(double a) { move(a,y()); }
		void setY(double a) { move(x(),a); }
		void setZ(double a);

		virtual bool collidesWith( const KtlQCanvasItem* ) const=0;

		KtlQCanvasItemList collisions(const bool exact /* NO DEFAULT */ ) const;

		virtual void setCanvas(KtlQCanvas*);

		virtual void draw(QPainter&)=0;

		void show();
		void hide();

		virtual void setVisible(bool yes);
		bool isVisible() const { return vis; }
		virtual void setSelected(const bool yes);
		bool isSelected() const { return sel; }

		virtual QRect boundingRect() const=0;

		KtlQCanvas* canvas() const { return cnv; }
		
		virtual bool collidesWith( const KtlQCanvasPolygonalItem*,
					   const KtlQCanvasRectangle*,
					   const KtlQCanvasEllipse* ) const = 0;

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
		KtlQCanvas* cnv;
		static KtlQCanvas* current_canvas;
		KtlQCanvasItemExtra *ext;
		KtlQCanvasItemExtra& extra();
		bool m_bNeedRedraw;
		bool vis;
		bool sel;

};

class KtlQCanvas : public QObject
{
	Q_OBJECT
	public:
		KtlQCanvas( QObject* parent = 0, const char* name = 0 );
		KtlQCanvas( const int w, const int h);
		KtlQCanvas( QPixmap p, int h, int v, int tilewidth, int tileheight );

		virtual ~KtlQCanvas();

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
		void addItemToChunk(KtlQCanvasItem*, int i, int j);
		void removeItemFromChunk(KtlQCanvasItem*, int i, int j);
		void addItemToChunkContaining(KtlQCanvasItem*, int x, int y);
		void removeItemFromChunkContaining(KtlQCanvasItem*, int x, int y);

		KtlQCanvasItemList allItems();
		KtlQCanvasItemList collisions( const QPoint&) /* const */ ;
		KtlQCanvasItemList collisions( const QRect&) /* const */;
		KtlQCanvasItemList collisions( const Q3PointArray& pa, const KtlQCanvasItem* item,
						bool exact) const;

		void drawArea(const QRect&, QPainter* p);

		// These are for KtlQCanvasView to call
		virtual void addView(KtlQCanvasView*);
		virtual void removeView(KtlQCanvasView*);
		void drawCanvasArea(const QRect&, QPainter* p, bool double_buffer);
		void drawViewArea( KtlQCanvasView* view, QPainter* p, const QRect& r, bool dbuf );

		// These are for KtlQCanvasItem to call
		virtual void addItem(KtlQCanvasItem*);
		virtual void removeItem(const KtlQCanvasItem*);

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

		KtlQCanvasChunk& chunk(int i, int j) const;
		KtlQCanvasChunk& chunkContaining(int x, int y) const;

		QRect changeBounds(const QRect& inarea);
		void drawChanges(const QRect& inarea);
		void drawChangedItems( QPainter & painter );
		void setNeedRedraw( const KtlQCanvasItemList * list );

		QPixmap offscr;
		int chunksize;
		int maxclusters;
		QRect m_size;
		QRect m_chunkSize;
		KtlQCanvasChunk* chunks;

		SortedCanvasItems m_canvasItems;
		Q3PtrList<KtlQCanvasView> m_viewList;

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

		friend void qt_unview(KtlQCanvas* c);

		KtlQCanvas( const KtlQCanvas & );
		KtlQCanvas &operator=( const KtlQCanvas & );
};

class KtlQCanvasViewData;

class KtlQCanvasView : public Q3ScrollView
{
	Q_OBJECT
	public:

		KtlQCanvasView(QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
		KtlQCanvasView(KtlQCanvas* viewing, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
		~KtlQCanvasView();

		KtlQCanvas* canvas() const
		{ return viewing; }
		void setCanvas(KtlQCanvas* v);

		const QMatrix &worldMatrix() const;
		const QMatrix &inverseWorldMatrix() const;
		bool setWorldMatrix( const QMatrix & );

	protected:
        /** overrides Q3ScrollView::drawContents() */
		virtual void drawContents( QPainter*, int cx, int cy, int cw, int ch );
		QSize sizeHint() const;

	private:
		void drawContents( QPainter* );
		KtlQCanvas* viewing;
		KtlQCanvasViewData* d;
		friend void qt_unview(KtlQCanvas* c);
		KtlQCanvasView( const KtlQCanvasView & );
		KtlQCanvasView &operator=( const KtlQCanvasView & );

	private slots:
		void cMoving(int,int);
		void updateContentsSize();

};


class KtlQPolygonalProcessor;

class KtlQCanvasPolygonalItem : public KtlQCanvasItem
{
	public:
		KtlQCanvasPolygonalItem(KtlQCanvas* canvas);
		virtual ~KtlQCanvasPolygonalItem();

		bool collidesWith( const KtlQCanvasItem* ) const;

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
		{ return val; }

	private:
		void scanPolygon( const Q3PointArray& pa, int winding,
					KtlQPolygonalProcessor& process ) const;

		Q3PointArray chunks() const;

		bool collidesWith( const KtlQCanvasPolygonalItem*,
				   const KtlQCanvasRectangle*,
				   const KtlQCanvasEllipse* ) const;

		QBrush br;
		QPen pn;
		bool wind;
};


class KtlQCanvasRectangle : public KtlQCanvasPolygonalItem
{
	public:
		KtlQCanvasRectangle(KtlQCanvas* canvas);
		KtlQCanvasRectangle(const QRect&, KtlQCanvas* canvas);
		KtlQCanvasRectangle(int x, int y, int width, int height, KtlQCanvas* canvas);

		~KtlQCanvasRectangle();

		int width() const;
		int height() const;
		void setSize(const int w, const int h);
		QSize size() const
		{ return QSize(w,h); }
		Q3PointArray areaPoints() const;
		QRect rect() const
		{ return QRect(int(x()),int(y()),w,h); }

		bool collidesWith( const KtlQCanvasItem* ) const;

	protected:
		void drawShape(QPainter &);
		Q3PointArray chunks() const;

	private:
		bool collidesWith( const KtlQCanvasPolygonalItem*,
				   const KtlQCanvasRectangle*,
				   const KtlQCanvasEllipse* ) const;

		int w, h;
};


class KtlQCanvasPolygon : public KtlQCanvasPolygonalItem
{
	public:
		KtlQCanvasPolygon(KtlQCanvas* canvas);
		~KtlQCanvasPolygon();
		void setPoints(Q3PointArray);
		Q3PointArray points() const;
		void moveBy(double dx, double dy);

		Q3PointArray areaPoints() const;

	protected:
		void drawShape(QPainter &);
        // TODO FIXME guarts are added for debugging memory corruption (poly takes non-pointer values)
        int guardBef[10];
		Q3PointArray *poly;
        int guardAft[10];
};


class KtlQCanvasLine : public KtlQCanvasPolygonalItem
{
	public:
		KtlQCanvasLine(KtlQCanvas* canvas);
		~KtlQCanvasLine();
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


class KtlQCanvasEllipse : public KtlQCanvasPolygonalItem
{

	public:
		KtlQCanvasEllipse( KtlQCanvas* canvas );
		KtlQCanvasEllipse( int width, int height, KtlQCanvas* canvas );
		KtlQCanvasEllipse( int width, int height, int startangle, int angle,
					KtlQCanvas* canvas );

		~KtlQCanvasEllipse();

		int width() const;
		int height() const;
		void setSize(int w, int h);
		void setAngles(int start, int length);
		int angleStart() const { return a1; }
		int angleLength() const { return a2; }
		Q3PointArray areaPoints() const;

		bool collidesWith( const KtlQCanvasItem* ) const;

	protected:
		void drawShape(QPainter &);

	private:
		bool collidesWith( const KtlQCanvasPolygonalItem*,
				   const KtlQCanvasRectangle*,
				   const KtlQCanvasEllipse* ) const;
		int w, h;
		int a1, a2;
};


#endif // QCANVAS_H
