//
// C++ Interface: simplecomponent
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef SIMPLECOMPONENT_H
#define SIMPLECOMPONENT_H

#include "component.h"

class SimpleComponent : public Component {
public: 
	SimpleComponent(ICNDocument *icnDocument, bool newItem, const QString &id);

	~SimpleComponent();

protected: 

	/**
	 * Create 1 pin on the left of the component, placed half way down if h1 is
	 * -1 - else at the position of h1.
	 */
	void init1PinLeft(int h1 = -1);

	/**
	 * Create 2 pins on the left of the component, either spread out, or at the
	 * given heights.
	 */
	void init2PinLeft(int h1 = -1, int h2 = -1);

	/**
	 * Create 3 pins on the left of the component, either spread out, or at the
	 * given heights.
	 */
	void init3PinLeft(int h1 = -1, int h2 = -1, int h3 = -1);

	/**
	 * Create 4 pins on the left of the component, either spread out, or at the
	 * given heights.
	 */
	void init4PinLeft(int h1 = -1, int h2 = -1, int h3 = -1, int h4 = -1);

	/**
	 * Create 1 pin on the right of the component, placed half way down if h1 is
	 * -1 - else at the position of h1.
	 */
	void init1PinRight(int h1 = -1);

	/**
	 * Create 2 pins on the right of the component, either spread out, or at the
	 * given heights.
	 */
	void init2PinRight(int h1 = -1, int h2 = -1);

	/**
	 * Create 3 pins on the right of the component, either spread out, or at the
	 * given heights.
	 */
	void init3PinRight(int h1 = -1, int h2 = -1, int h3 = -1);

	/**
	 * Create 4 pins on the right of the component, either spread out, or at the
	 * given heights.
	 */
	void init4PinRight(int h1 = -1, int h2 = -1, int h3 = -1, int h4 = -1);

	// Pointers to commonly used nodes
// It simplifies code greatly to have two lists... 
	ECNode *m_pPNode[4];
	ECNode *m_pNNode[4];

};

#endif 