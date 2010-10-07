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
#include <KDebug>

using namespace KTechLab;

class KTechLab::ConnectorPrivate
{
public:
    ConnectorPrivate(const QVariantMap& connectorData){
        startNode = 0;
        endNode = 0;
        data = connectorData;
        parseRoute(data.value( "route" ).toString());
    };

    void parseRoute(const QString pathString);

    void setRoute(const QPainterPath& path);
    void setEndNode(const Node* node);
    void setStartNode(const Node* node);

    QVariantMap data;
    QPainterPath route;
    const Node* startNode;
    const Node* endNode;
};

void ConnectorPrivate::setRoute(const QPainterPath& path)
{
    route = path;
    data.remove("route");
    QString pathString;
    for (int i=0; i<path.elementCount(); ++i){
        QPainterPath::Element e = path.elementAt(i);
        pathString += (e.x-4)/8 + ',' + (e.y-4)/8 + ',';
    }
    data.insert("route",pathString);
}
void ConnectorPrivate::parseRoute(const QString pathString)
{
    QStringList routeList = pathString.split(',');
    if (routeList.length() == 0){
        kError() << "Cannot parse route:" << pathString;
        return;
    }

    //remove last entry, if it is empty
    if (routeList.last().isEmpty())
        routeList.removeLast();

    if (routeList.length() % 2 != 0){
        kError() << "Cannot parse route:" << pathString;
        return;
    }

    QStringList::const_iterator it = routeList.constBegin();
    QPointF p;
    p.setX((*it++).toDouble()*8+4);
    p.setY((*it++).toDouble()*8+4);
    route.moveTo(p);
    while (it != routeList.constEnd()){
        p.setX((*it++).toDouble()*8+4);
        p.setY((*it++).toDouble()*8+4);
        route.lineTo(p);
    }
}

void ConnectorPrivate::setStartNode(const Node* node)
{
    if (!node || !node->isValid()){
        kError() << "Invalid node set as start node:" << node;
        return;
    }
    startNode = node;

    data.remove("start-node-id");
    data.remove("start-node-cid");
    data.remove("start-node-parent");
    data.remove("start-node-is-child");
    QString parentId = node->parentId();
    if (parentId.isEmpty()){
        data.insert("start-node-is-child", 1);
        data.insert("start-node-parent", parentId);
        data.insert("start-node-cid", node->id());
    } else {
        data.insert("start-node-is-child", 0);
        data.insert("start-node-id", node->id());
    }
}

void ConnectorPrivate::setEndNode(const Node* node)
{
    if (!node || !node->isValid()){
        kError() << "Invalid node set as end node:" << node;
        return;
    }
    endNode = node;

    data.remove("end-node-id");
    data.remove("end-node-cid");
    data.remove("end-node-parent");
    data.remove("end-node-is-child");
    QString parentId = node->parentId();
    if (parentId.isEmpty()){
        data.insert("end-node-is-child", 1);
        data.insert("end-node-parent", parentId);
        data.insert("end-node-cid", node->id());
    } else {
        data.insert("end-node-is-child", 0);
        data.insert("end-node-id", node->id());
    }
}


Connector::Connector(const QVariantMap& connectorData, QObject* parent)
    : QObject(parent),
      d(new ConnectorPrivate(connectorData))
{
}

Connector::Connector(const KTechLab::Connector& connector)
    : QObject(connector.parent()),
      d(new ConnectorPrivate(connector.data()))
{
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
    //// stand-alone nodes
    // test start node
    found |= d->data.value("start-node-is-child") == 0 &&
             d->data.value("start-node-id") == node->id();
    //test end node
    found |= d->data.value("end-node-is-child") == 0 &&
             d->data.value("end-node-id") == node->id();
    //// child-nodes of items
    // test start node
    found |= d->data.value("start-node-is-child") == 1 &&
             d->data.value("start-node-parent") == node->parentId() &&
             d->data.value("start-node-cid") == node->id();
    // test end node
    found |= d->data.value("end-node-is-child") == 1 &&
             d->data.value("end-node-parent") == node->parentId() &&
             d->data.value("end-node-cid") == node->id();
    return found;
}

void Connector::setRoute(const QPainterPath& route)
{
    d->setRoute(route);
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

QPainterPath Connector::route() const
{
    return d->route;
}

QVariantMap Connector::data() const
{
    return d->data;
}

#include "connector.moc"