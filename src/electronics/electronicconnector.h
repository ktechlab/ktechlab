//
// C++ Interface: electronicconnector
//
// Description:
//
//
// Author: David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ELECTRONICCONNECTOR_H
#define ELECTRONICCONNECTOR_H

#include <connector.h>

#include "ecnode.h"

/**
	@author David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>

	An electronic connector, connecting 2 ECNodes.
*/
class ElectronicConnector : public Connector {
	Q_OBJECT

public:
	ElectronicConnector(ECNode* startNode, ECNode* endNode, ICNDocument* _ICNDocument, const QString &id = QString());

	~ElectronicConnector();

	/**
	  * Node at start of connector (which refers to this as the output connector)
	  */
	virtual Node* startNode() const {
		return m_startEcNode;
	}

	/**
	 * Node at end of connector (which refers to this as the input connector)
	 */
	virtual Node* endNode() const {
		return m_endEcNode;
	}

	unsigned numWires() const { return m_wires.size(); }

	Wire *wire(unsigned num = 0) const {
		return (num < m_wires.size()) ? m_wires[num] : 0;
	}

	/**
	 * Increases the currentAnimationOffset according to the current flowing in
	 * the connector and deltaTime.
	 */
	void incrementCurrentAnimation(double deltaTime);

	/**
	 * Modular offset of moving dots in connector, indicating current (in
	 * pixels).
	 */
	double currentAnimationOffset() const { return m_currentAnimationOffset; }

signals :
	void removed(ECNode *node);
	void removed(ElectronicConnector *connector);

public slots:
	/**
	 * Takes the minimum pin count of the start and end nodes, and creates a
	 * connector for each pin up to that minimum.
	 */
	void syncWiresWithNodes();

private:

	double m_currentAnimationOffset;

	/// pointers to the endnodes of the connectors
	ECNode *m_startEcNode;
	ECNode *m_endEcNode;
};

#endif

