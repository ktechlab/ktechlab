/*
    Copyright (C) 2010 Julia Bäume <julian@svg4all.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "connector.h"
#include "node.h"
#include <QStringList>
#include <QPointF>
#include <QPainterPath>

#if KDE_ENABLED
#include <KDebug>
#else
#include <QDebug>
#endif

using namespace KTechLab;

class KTechLab::ConnectorPrivate
{
public:
    ConnectorPrivate(){
        startNode = 0;
        endNode = 0;
    };

    void setEndNode(const Node* node);
    void setStartNode(const Node* node);

    QVariantMap data() const;
    const Node* startNode;
    const Node* endNode;
};

void ConnectorPrivate::setStartNode(const Node* node)
{
    if (!node || !node->isValid()){
#if KDE_ENABLED
        kWarning() << "Invalid node set as start node:" << node;
#else
        qWarning() << "Invalid node set as start node:" << node;
#endif
        return;
    }
    startNode = node;
}

QVariantMap ConnectorPrivate::data() const
{
    QVariantMap map;

    Q_ASSERT(startNode);
    QString parentId = startNode->parentId();
    if (!parentId.isEmpty()){
        map.insert("start-node-is-child", 1);
        map.insert("start-node-parent", parentId);
        map.insert("start-node-cid", startNode->id());
    } else {
        map.insert("start-node-is-child", 0);
        map.insert("start-node-id", startNode->id());
    }

    Q_ASSERT(endNode);
    parentId = endNode->parentId();
    if (!parentId.isEmpty()){
        map.insert("end-node-is-child", 1);
        map.insert("end-node-parent", parentId);
        map.insert("end-node-cid", endNode->id());
    } else {
        map.insert("end-node-is-child", 0);
        map.insert("end-node-id", endNode->id());
    }

    return map;
}

void ConnectorPrivate::setEndNode(const Node* node)
{
    if (!node || !node->isValid()){
#if KDE_ENABLED
        kWarning() << "Invalid node set as end node:" << node;
#else
        qWarning() << "Invalid node set as end node:" << node;
#endif
        return;
    }
    endNode = node;
}


Connector::Connector(QObject* parent)
    : QObject(parent),
      d(new ConnectorPrivate())
{
}

Connector::Connector(const KTechLab::Connector& connector)
    : QObject(connector.parent()),
      d(new ConnectorPrivate())
{
    setStartNode(connector.startNode());
    setEndNode(connector.endNode());
}

Connector::~Connector()
{
    delete d;
}

bool Connector::connectsTo(const KTechLab::Node* node) const
{
    if (!node->isValid())
        return false;

    bool found = false;
    const QVariantMap& data = d->data();
    //// stand-alone nodes
    // test start node
    found |= data.value("start-node-is-child") == 0 &&
             data.value("start-node-id") == node->id();
    //test end node
    found |= data.value("end-node-is-child") == 0 &&
             data.value("end-node-id") == node->id();
    //// child-nodes of items
    // test start node
    found |= data.value("start-node-is-child") == 1 &&
             data.value("start-node-parent") == node->parentId() &&
             data.value("start-node-cid") == node->id();
    // test end node
    found |= data.value("end-node-is-child") == 1 &&
             data.value("end-node-parent") == node->parentId() &&
             data.value("end-node-cid") == node->id();
    return found;
}

void Connector::setStartNode(const Node* node)
{
    d->setStartNode(node);
}
const Node* Connector::startNode() const
{
    return d->startNode;
}

void Connector::setEndNode(const Node* node)
{
    d->setEndNode(node);
}
const Node* Connector::endNode() const
{
    return d->endNode;
}

QVariantMap Connector::data() const
{
    return d->data();
}

// #include "connector.moc"
