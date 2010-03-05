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

using namespace KTechLab;

Node::Node(QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsEllipseItem(parent, scene)
{
}

Node::~Node()
{
}

void Node::setId(const QString& id)
{
    m_id = id;
}

void Node::setParentId(const QString& id)
{
    m_parentId = id;
}

QString Node::id() const
{
    return m_id;
}

QString Node::parentId() const
{
    return m_parentId;
}

bool Node::isValid() const
{
    return scene() != 0;
}

#include "node.moc"