
#ifndef UTILS_H
#define UTILS_H

#include <Qt/qpoint.h>
#include <cmath>

inline int roundDown( int x, int roundness )
{
	if ( x < 0 )
		return (x-roundness+1) / roundness;
	else
		return (x / roundness);
}
inline int roundDown( double x, int roundness )
{
	return roundDown( int(x), roundness );
}

inline QPoint roundDown( const QPoint & p, int roundness )
{
	return QPoint( roundDown( p.x(), roundness ), roundDown( p.y(), roundness ) );
}

inline int toCanvas( int pos )
{
    return pos*8+4;
}
inline int fromCanvas( int pos )
{
	return roundDown( pos-4, 8 );
}

inline QPoint toCanvas( const QPoint * pos )
{
	return QPoint( toCanvas(pos->x()), toCanvas(pos->y()) );
}
inline QPoint fromCanvas( const QPoint * pos )
{
	return QPoint( fromCanvas(pos->x()), fromCanvas(pos->y()) );
}

inline QPoint toCanvas( const QPoint & pos )
{
	return QPoint( toCanvas(pos.x()), toCanvas(pos.y()) );
}
inline QPoint fromCanvas( const QPoint & pos )
{
	return QPoint( fromCanvas(pos.x()), fromCanvas(pos.y()) );
}

inline int roundDouble( double x )
{
	return int(std::floor(x+0.5));
}

inline double qpoint_distance( const QPoint & p1, const QPoint & p2 )
{
	double dx = p1.x() - p2.x();
	double dy = p1.y() - p2.y();

	return std::sqrt( dx*dx + dy*dy );
}


inline int snapToCanvas( int x )
{
	return roundDown( x, 8 )*8 + 4;
}
inline int snapToCanvas( double x )
{
	return snapToCanvas( int(x) );
}

#endif
