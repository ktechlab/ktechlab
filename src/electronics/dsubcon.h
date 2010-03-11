//
// C++ Interface: dsubcon
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef DSUBCON_H
#define DSUBCON_H

#include "component.h"

/**
a Component subclass that provides layout and stuff for
the "D-subminiature" style connector.
*/
class DSubCon : public Component {

public:
	DSubCon(ICNDocument *icnDocument, bool newItem, const char *id)
		: Component(icnDocument, newItem, id) { }

	/**
	 * Convenience functionality provided for components in a port shape
	 * (such as ParallelPortComponent and SerialPortComponent).
	 */
protected: 
	void drawPortShape(QPainter &p);
};

#endif

