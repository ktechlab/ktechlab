/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton <david@bluehaze.org>          *
 *   Copyright (C) 2010 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "connector.h"
#include "node.h"

#include <KDebug>
#include "idocumentitem.h"
#include <idocumentscene.h>
#include "connectoritem.h"
#include <QGraphicsSceneMouseEvent>

using namespace KTechLab;

Node::Node(QGraphicsItem* parent, IDocumentScene* scene)
    : QGraphicsEllipseItem(parent, scene),
      m_parent(0),
      m_documentScene( scene )
{
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
    //disable moving for now, until interaction is implemented
    //setFlag(ItemIsMovable);
    setRect(-2,-2,4,4);
    setBrush(QBrush(Qt::SolidPattern));
}

Node::~Node()
{
}

void Node::setId(const QString& id)
{
    IDocumentItem::setId(id);
}

void Node::setParent(IDocumentItem* item)
{
    m_parent = item;
}

QString Node::parentId() const
{
    if (!m_parent) return QString();

    return m_parent->id();
}

bool Node::isValid() const
{
    return scene() != 0;
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!fetchDocumentScene()){
        event->ignore();
        QGraphicsEllipseItem::mousePressEvent(event);
        return;
    }
    if (!m_documentScene->isRouting()){
        const QPointF &center = mapToScene(rect().center());
        ConnectorItem* c = m_documentScene->startRouting(center);
        c->setStartNode(this);
        if (parentItem())
            setOpacity(0.01);
        event->accept();
    } else {
        const QPointF &center = mapToScene(rect().center());
        ConnectorItem* c = m_documentScene->finishRouting(center);
        c->setEndNode(this);
    }
}

bool Node::fetchDocumentScene()
{
    if (!m_documentScene)
        m_documentScene = qobject_cast<IDocumentScene*>(this->scene());

    return m_documentScene != 0;
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (isUnderMouse() || isSelected() || countConnectors() > 2){
        setOpacity(1);
    } else {
        setOpacity(0.01);
    }
    QGraphicsEllipseItem::paint(painter, option, widget);
}

int Node::countConnectors() const
{
    int c = 0;
    if (parentItem()) c++;

    QList<QGraphicsItem*> list = collidingItems();
    foreach(QGraphicsItem* i, list) {
        ConnectorItem* cItem = qgraphicsitem_cast<ConnectorItem*>(i);
        if (!cItem) continue;
        if (this == cItem->startNode() || this == cItem->endNode()) c++;
    }
    return c;
}
