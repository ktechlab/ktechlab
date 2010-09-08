
#ifndef UTILS_H
#define UTILS_H

#include <qpoint.h>
#include <math.h>

class QSize;
inline int roundDown(const int x, const int roundness) {
//FIXME: What is this function really supposed to do?
// must be important because it's called millions of times!

	if (x < 0)
		return (x - roundness + 1) / roundness;
	else	return (x / roundness);
}

inline QPoint roundDown(const QPoint &p, const int roundness) {
	return QPoint(roundDown(p.x(), roundness), roundDown(p.y(), roundness));
}

inline int toCanvas(const int pos) {
	return (pos << 3) + 4;
}

inline int fromCanvas(const int pos) {
	return (pos - 4) >> 3;
}

inline QPoint toCanvas(const QPoint &pos) {
	return QPoint(toCanvas(pos.x()), toCanvas(pos.y()));
}

inline QPoint fromCanvas(const QPoint &pos) {
	return QPoint(fromCanvas(pos.x()), fromCanvas(pos.y()));
}

inline int roundDouble(const double x) {
	return int(floor(x + 0.5));
}

inline double qpoint_distance(const QPoint &p1, const QPoint &p2) {
	return hypot(p1.x() - p2.x(), p1.y() - p2.y());
}

inline int snapToCanvas(const int x) {
	return (x & ~7) | 4;
}

inline int snapToCanvas(const double x) {
	return snapToCanvas(int(x));
}

inline QPoint snapToCanvas(const QPoint &pos) {
	return QPoint(snapToCanvas(pos.x()), snapToCanvas(pos.y()));
}

template<class T> inline T mapToCells(const T& value)
{
    return (value - T(4,4)) / 8;
}
template<class T> inline T mapFromCells(const T& value)
{
    return value * 8 + T(4,4);
}
template<> inline QSize mapToCells(const QSize& value)
{
    return value / 8;
}
template<> inline QSize mapFromCells(const QSize& value)
{
    return value * 8;
}


#endif

