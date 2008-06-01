/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECNODE_H
#define ECNODE_H

#include "node.h"

#include <qvaluevector.h>

class ECNode;
class Element;
class Pin;
class Switch;
class QTimer;

typedef QValueList<ECNode*> ECNodeList;
typedef QValueList<Element*> ElementList;
typedef QValueVector<Pin*> PinVector;

/**
@short Electrical node with voltage / current / etc properties
@author David Saxton
*/
class ECNode : public Node
{
	Q_OBJECT
	public:
		ECNode( ICNDocument *icnDocument, Node::node_type type, int dir, const QPoint &pos, QString *id = 0L );
		~ECNode();

		virtual void setParentItem( CNItem *parentItem );
		virtual void drawShape( QPainter &p );
		/**
		 * Set the number of pins "contained" in this node.
		 */
		void setNumPins( unsigned num );
		/**
		 * @return the number of pins in this node.
		 * @see setNumPins
		 */
		unsigned numPins() const { return m_pins.size(); }
		PinVector pins() const { return m_pins; }
		Pin * pin( unsigned num = 0 ) const { return (num < m_pins.size()) ? m_pins[num] : 0l; }
		bool showVoltageBars() const { return m_bShowVoltageBars; }
		void setShowVoltageBars( bool show ) { m_bShowVoltageBars = show; }
		bool showVoltageColor() const { return m_bShowVoltageColor; }
		void setShowVoltageColor( bool show ) { m_bShowVoltageColor = show; }
		void setNodeChanged();

	signals:
		void numPinsChanged( unsigned newNum );

	protected slots:
		void removeElement( Element * e );
		void removeSwitch( Switch * sw );

	protected:
		bool m_bShowVoltageBars;
		bool m_bShowVoltageColor;
		double m_prevV;
		double m_prevI;
		QCanvasRectangle * m_pinPoint;
		PinVector m_pins;
};

#endif

