/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cnitem.h"
#include "icndocument.h"
#include "connector.h"
#include "itemdocumentdata.h"
#include "node.h"

#include <kdebug.h>

#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>
#include "ktlcanvas.h"

QColor Node::m_selectedColor = QColor(101, 134, 192);

Node::Node(ICNDocument *icnDocument, Node::node_type type, int dir, const QPoint &pos, QString *id)
		: QObject(), Q3CanvasPolygon(icnDocument ? icnDocument->canvas() : 0) {
	m_length = 8;
	p_nodeGroup = 0;
	p_parentItem = 0;
	b_deleted = false;
	m_dir = dir;
	m_type = type;
	p_icnDocument = icnDocument;
	m_level = 0;

	if(p_icnDocument) {
		if (id) {
			m_id = *id;

			if (!p_icnDocument->registerUID(*id))
				kdError() << k_funcinfo << "Could not register id " << *id << endl;
		} else m_id = p_icnDocument->generateUID("node" + QString::number(type));
	}

	initPoints();

	move(pos.x(), pos.y());
	setBrush(Qt::black);
	setPen(Qt::black);
	show();

	emit(moved(this));
}

Node::~Node() {
	if (p_icnDocument)
		p_icnDocument->unregisterUID(id());
}

// TODO: move to a nested document model to remove all this "level" complexity from both here and 
// ICNDocument. 
void Node::setLevel(const int level) {
	m_level = level;
}

void Node::setLength(const int length) {
	if (m_length == length) return;

	m_length = length;
	initPoints();
}

void Node::setOrientation(const int dir) {
	if (m_dir == dir) return;

	m_dir = dir;
	initPoints();
}

void Node::initPoints() {
	// Bounding rectangle, facing right
	Q3PointArray pa(QRect(0, -8, m_length, 16));

	QMatrix m;
	m.rotate(m_dir);
	pa = m.map(pa);
	setPoints(pa);
}

void Node::setVisible(const bool yes) {
	if (isVisible() == yes) return;

	Q3CanvasPolygon::setVisible(yes);
}

void Node::setParentItem(CNItem *parentItem) {
	if (!parentItem) {
		kdError() << k_funcinfo << "no parent item" << endl;
		return;
	}

	p_parentItem = parentItem;

	setLevel(p_parentItem->level());

	connect(p_parentItem, SIGNAL(movedBy(double, double)), this, SLOT(moveBy(double, double)));
	connect(p_parentItem, SIGNAL(removed(Item*)), this, SLOT(removeNode(Item*)));
}

void Node::removeNode() {
	if(b_deleted) return;

	b_deleted = true;
	emit removed(this);
	p_icnDocument->appendDeleteList(this);
}

void Node::moveBy(const double dx, const double dy) {
	if(dx == 0 && dy == 0) return;

	Q3CanvasPolygon::moveBy(dx, dy);

	emit moved(this);
}

NodeData Node::nodeData() const {
	NodeData data;
	data.x = x();
	data.y = y();
	return data;
}

// again, do we really want to know about the UI here? 
// is there a more elegant way to do this?
void Node::setNodeSelected(const bool yes) {
	if (isSelected() == yes) return;

	Q3CanvasItem::setSelected(yes);
	setPen(yes ? m_selectedColor : Qt::black);
	setBrush(yes ? m_selectedColor : Qt::black);
}

void Node::initPainter(QPainter &p) {
	p.translate(int(x()), int(y()));
	p.rotate(m_dir);
}

void Node::deinitPainter(QPainter &p) {
	p.rotate(-m_dir);
	p.translate(-int(x()), -int(y()));
}

#include "node.moc"
