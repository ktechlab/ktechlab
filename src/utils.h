
#ifndef UTILS_H
#define UTILS_H

#include <qpoint.h>
#include <math.h>

inline int roundDown(const int x, const int roundness) {
//FIXME: What is this function really supposed to do?
// must be important because it's called millions of times!
	if (x < 0)
		return (x - roundness + 1) / roundness;
	else	return (x / roundness);
}

// WHAT THE HELL IS THIS CODE SUPPOSED TO DO?
inline int roundDown(double x, int roundness) {
	return roundDown(int(x), roundness);
}

inline QPoint roundDown(const QPoint &p, int roundness) {
	return QPoint(roundDown(p.x(), roundness), roundDown(p.y(), roundness));
}

inline int toCanvas(int pos) {
	return (pos << 3); // + 4;
}

inline int fromCanvas(int pos) {
//	return roundDown(pos - 4, 8);
	return (pos + 4) >> 3;
}

inline QPoint toCanvas(const QPoint &pos) {
	return QPoint(toCanvas(pos.x()), toCanvas(pos.y()));
}

inline QPoint fromCanvas(const QPoint &pos) {
	return QPoint(fromCanvas(pos.x()), fromCanvas(pos.y()));
}

inline int roundDouble(double x) {
	return int(floor(x + 0.5));
}

inline double qpoint_distance(const QPoint &p1, const QPoint &p2) {
	return hypot(p1.x() - p2.x(), p1.y() - p2.y());
}

#endif
