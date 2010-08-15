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

using namespace KTechLab;

ConnectorItem::ConnectorItem(QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsPathItem(parent, scene),
      m_connector(0)
{
    init();
}

ConnectorItem::ConnectorItem(const Connector& connector, QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsPathItem(parent, scene)
{
    init();
    setConnector(connector);
}

ConnectorItem::ConnectorItem(const QVariantMap& connectorData, QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsPathItem(parent, scene)
{
    init();
    m_connector = new Connector(connectorData);
    setPath(m_connector->route());
}

void ConnectorItem::init()
{
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable,true);
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

void ConnectorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton){
        if (event->modifiers() != Qt::ControlModifier)
            scene()->clearSelection();
        setSelected(true);
        event->accept();
    }
}

QVariant ConnectorItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged && value.toBool()){
        setPen(scene()->palette().highlight().color());
    } else if (change == ItemSelectedHasChanged && !value.toBool()){
        setPen(scene()->palette().windowText().color());
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
