//
// C++ Implementation: simplecomponent
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//

#include "simplecomponent.h"
#include "circuitdocument.h"

SimpleComponent::SimpleComponent(ICNDocument *icnDocument, bool newItem, const QString &id) 
	: Component(icnDocument, newItem, id) {

	for (int i = 0; i < 4; ++i) {
		m_pPNode[i] = 0;
		m_pNNode[i] = 0;
	}
}

SimpleComponent::~SimpleComponent() {
}

void SimpleComponent::init1PinLeft(int h1) {
	if(h1 == -1) h1 = offsetY() + height() / 2;

	m_pNNode[0] = createPin(offsetX() - 8, h1, 0, "n1");
}

void SimpleComponent::init2PinLeft(int h1, int h2) {
	if(h1 == -1) h1 = offsetY() + 8;
	if(h2 == -1) h2 = offsetY() + height() - 8;

	m_pNNode[0] = createPin(offsetX() - 8, h1, 0, "n1");
	m_pNNode[1] = createPin(offsetX() - 8, h2, 0, "n2");
}

void SimpleComponent::init3PinLeft(int h1, int h2, int h3) {
	if(h1 == -1) h1 = offsetY() + 8;
	if(h2 == -1) h2 = offsetY() + height() / 2;
	if(h3 == -1) h3 = offsetY() + height() - 8;

	m_pNNode[0] = createPin(offsetX() - 8, h1, 0, "n1");
	m_pNNode[1] = createPin(offsetX() - 8, h2, 0, "n2");
	m_pNNode[2] = createPin(offsetX() - 8, h3, 0, "n3");
}

void SimpleComponent::init4PinLeft(int h1, int h2, int h3, int h4) {
	if(h1 == -1) h1 = offsetY() + 8;
	if(h2 == -1) h2 = offsetY() + 24;
	if(h3 == -1) h3 = offsetY() + height() - 24;
	if(h4 == -1) h4 = offsetY() + height() - 8;

	m_pNNode[0] = createPin(offsetX() - 8, h1, 0, "n1");
	m_pNNode[1] = createPin(offsetX() - 8, h2, 0, "n2");
	m_pNNode[2] = createPin(offsetX() - 8, h3, 0, "n3");
	m_pNNode[3] = createPin(offsetX() - 8, h4, 0, "n4");
}

void SimpleComponent::init1PinRight(int h1) {
	if(h1 == -1) h1 = offsetY() + height() / 2;

	m_pPNode[0] = createPin(offsetX() + width() + 8, h1, 180, "p1");
}

void SimpleComponent::init2PinRight(int h1, int h2) {
	if(h1 == -1) h1 = offsetY() + 8;
	if(h2 == -1) h2 = offsetY() + height() - 8;

	m_pPNode[0] = createPin(offsetX() + width() + 8, h1, 180, "p1");
	m_pPNode[1] = createPin(offsetX() + width() + 8, h2, 180, "p2");
}

void SimpleComponent::init3PinRight(int h1, int h2, int h3) {
	if(h1 == -1) h1 = offsetY() + 8;
	if(h2 == -1) h2 = offsetY() + height() / 2;
	if(h3 == -1) h3 = offsetY() + height() - 8;

	m_pPNode[0] = createPin(offsetX() + width() + 8, h1, 180, "p1");
	m_pPNode[1] = createPin(offsetX() + width() + 8, h2, 180, "p2");
	m_pPNode[2] = createPin(offsetX() + width() + 8, h3, 180, "p3");
}

void SimpleComponent::init4PinRight(int h1, int h2, int h3, int h4) {
	if(h1 == -1) h1 = offsetY() + 8;
	if(h2 == -1) h2 = offsetY() + 24;
	if(h3 == -1) h3 = offsetY() + height() - 24;
	if(h4 == -1) h4 = offsetY() + height() - 8;

	m_pPNode[0] = createPin(offsetX() + width() + 8, h1, 180, "p1");
	m_pPNode[1] = createPin(offsetX() + width() + 8, h2, 180, "p2");
	m_pPNode[2] = createPin(offsetX() + width() + 8, h3, 180, "p3");
	m_pPNode[3] = createPin(offsetX() + width() + 8, h4, 180, "p4");
}

