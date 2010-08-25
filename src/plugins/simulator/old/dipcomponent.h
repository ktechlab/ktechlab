//
// C++ Interface: dipcomponent
//
// Description:
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef DIPCOMPONENT_H
#define DIPCOMPONENT_H

#include "component.h"

/**
base class for DIP type components
*/
class DIPComponent : public Component {
public:
	DIPComponent(ICNDocument *icnDocument, bool newItem, const char *id)
		: Component(icnDocument, newItem, id) { }

protected:
	/**
	 * This creates a set of nodes with their internal IDs set to those in QStringList pins.
	 * The pins are in a DIP arrangement, and are spaced width() apart.
	 */
	void initDIP(const QStringList &pins);

	/**
	 * Creates the DIP symbol:
	 * @li constructs rectangular shape
	 * @li puts on text labels in appropriate positions from QStringList pins
	 */
	void initDIPSymbol(const QStringList &pins, int width);
};

#endif
