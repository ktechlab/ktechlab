/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>

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

#include "connectoritem.h"
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOption>
#include <QGraphicsScene>
#include "connector.h"
#include <idocumentscene.h>
#include "icomponentitem.h"
#include <KDebug>

using namespace KTechLab;

ConnectorItem::ConnectorItem(IDocumentScene* scene, QGraphicsItem* parent)
    : QGraphicsPathItem(parent, scene),
      m_connector(0), m_scene(scene)
{
    init();
}

ConnectorItem::ConnectorItem(const QVariantMap& connectorData, IDocumentScene* scene, QGraphicsItem* parent)
    : QGraphicsPathItem(parent, scene), m_scene(scene)
{
    init();
    m_connector = new Connector(connectorData);
    setPath(m_connector->route());
    if (connectorData.value("id").canConvert(QVariant::String))
        setId(connectorData.value("id").toString());

    const Node* s = 0;
    const Node* e = 0;
    //TODO: remove these ugly checks
    if (connectorData.value("start-node-is-child").toString() == "1"){
        const IComponentItem* parent = scene->item(connectorData.value("start-node-parent").toString());
        if (parent && parent->node(connectorData.value("start-node-cid").toString()))
            s = parent->node(connectorData.value("start-node-cid").toString());
    } else if (connectorData.value("start-node-is-child").toString() == "0"){
        if (scene->node(connectorData.value("start-node-id").toString()))
            s = scene->node(connectorData.value("start-node-id").toString());
    }
    if (connectorData.value("end-node-is-child").toString() == "1"){
        const IComponentItem* parent = scene->item(connectorData.value("end-node-parent").toString());
        if (parent && parent->node(connectorData.value("end-node-cid").toString()))
            e = parent->node(connectorData.value("end-node-cid").toString());
    } else if (connectorData.value("end-node-is-child").toString() == "0"){
        if (scene->node(connectorData.value("end-node-id").toString()))
            e = scene->node(connectorData.value("end-node-id").toString());
    }
    m_connector->setStartNode(s);
    m_startNode = s;
    m_connector->setEndNode(e);
    m_endNode = e;
}

void ConnectorItem::init()
{
    m_startNode = 0;
    m_endNode = 0;
    setType("connector");
    setAcceptHoverEvents(true);
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
    setZValue(-1);
}

ConnectorItem::~ConnectorItem()
{
    delete m_connector;
    m_connector = 0;
}

void ConnectorItem::setConnector(const Connector& connector)
{
    if (m_connector){
        delete m_connector;
        m_connector = 0;
    }

    m_connector = new Connector(connector);
    setPath(m_connector->route());
}

Connector* ConnectorItem::connector() const
{
    return m_connector;
}

void ConnectorItem::setStartNode(const Node* node)
{
    m_startNode = node;
}
void ConnectorItem::setEndNode(const Node* node)
{
    m_endNode = node;
}
const Node* ConnectorItem::endNode() const
{
    return m_endNode;
}
const Node* ConnectorItem::startNode() const
{
    return m_startNode;
}

void ConnectorItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected()){
        setPen(scene()->palette().highlight().color());
        event->accept();
    }
    QGraphicsPathItem::hoverEnterEvent(event);
}

void ConnectorItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected()){
        setPen(scene()->palette().windowText().color());
        event->accept();
    }
    QGraphicsPathItem::hoverLeaveEvent(event);
}

QVariant ConnectorItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged && value.toBool()){
        setPen(scene()->palette().highlight().color());
    } else if (change == ItemSelectedHasChanged && !value.toBool()){
        setPen(scene()->palette().windowText().color());
    } else if (change == ItemPositionChange) {
        return QPointF(0,0);
    }
    return QGraphicsItem::itemChange(change, value);
}

void KTechLab::ConnectorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    //This is quite ugly. we copy the whole object, because it's const
    //may be, we should just paint the path on our own.
    QStyleOptionGraphicsItem *cOption = new QStyleOptionGraphicsItem(*option);
    //reset selected state, since we handle painting of selection-related visualisation
    cOption->state &= ~QStyle::State_Selected;

    QGraphicsPathItem::paint(painter, cOption, widget);
    delete cOption;
}

QVariantMap KTechLab::ConnectorItem::data() const
{
    QVariantMap map = KTechLab::IDocumentItem::data();
    QVariantMap d = m_connector->data();
    foreach(const QString& key, d.keys()){
        map.insert(key,d.value(key));
    }
    return map;
}
