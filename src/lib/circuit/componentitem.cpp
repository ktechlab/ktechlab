/***************************************************************************
 *   Copyright (C) 2009-2010 Julian Bäume <julian@svg4all.de>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "componentitem.h"
#include "theme.h"

#include <QVariantMap>
#include <QSvgRenderer>
#include <KDebug>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QKeyEvent>
#include <QFile>
#include "pinitem.h"
#include <QGraphicsScene>
#include <interfaces/idocumentmodel.h>

using namespace KTechLab;

ComponentItem::ComponentItem ( const QVariantMap& data, Theme *theme, QGraphicsItem* parentItem )
    : IComponentItem( parentItem ),
      m_renderer( new QSvgRenderer() ),
      m_theme( theme )
{
    setData(0, data);
    setId(data.value("id").toString());
    setName(data.value("name").toString());
    setType(data.value("type").toString());
    QString fileName = m_theme->findFirstFile( data.value("fileName").toString() );
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!m_svgDocument.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    initPins();

    // may be the renderer should provided by the Theme. Then different renderers for
    // the same component can be shared
    m_renderer->load( fileName );
    setSharedRenderer(m_renderer);
    // move to position in the scene.
    QPoint pos(data.value("x").toInt(),data.value("y").toInt());
    // coordinates from circuit file represent the center
    // of the component, so we have to move it -QPoint(32,32)
    pos -= QPoint(32,32);
    setPos( pos );
    setTransformOriginPoint(QPoint(32,32));

    m_shape.addRect(m_renderer->boundsOnElement("icon"));

    setRotation(data.value("angle").toReal());
    QTransform t;
    bool flipped = data.value("flipped").toBool();
    QPointF center = boundingRect().center();
    t.translate(center.x(),center.y())
     .scale((flipped ? -1 : 1), 1)
     .translate(-center.x(),-center.y());
    connect(this,SIGNAL(dataUpdated(QString,QVariantMap)),
            this,SLOT(updateData(QString,QVariantMap)));
}

ComponentItem::~ComponentItem()
{
    delete m_renderer;
}

void ComponentItem::updateData( const QString &name, const QVariantMap &data )
{
    kDebug() << name << "changed to:" << data;
    //updated component, so repaint
    update();
}

void ComponentItem::initPins()
{
    QDomNode pinsNode;
    QDomNodeList list = m_svgDocument.elementsByTagName("g");
    for (int i=0;i<list.count();i++){
        QDomNamedNodeMap attrs = list.item(i).attributes();
        if (attrs.contains("id") &&
            attrs.namedItem("id").toAttr().value() == QString("pins")){
            pinsNode = list.item(i);
            break;
        }
    }
    if (pinsNode.isNull() || !pinsNode.hasChildNodes()){
        kWarning() << "No pins definition found for this component";
        return;
    }
    QDomElement pin = pinsNode.firstChildElement();
    while (!pin.isNull()) {
        QRectF pinRect;
        double r = pin.attribute("r").toDouble();
        pinRect.setLeft(pin.attribute("cx").toDouble());
        pinRect.setTop(pin.attribute("cy").toDouble());
        pinRect.setWidth(r*2);
        pinRect.setHeight(r*2);
        PinItem* p = new PinItem(pinRect, this, scene());
        p->setId(pin.attribute("id"));
        pin = pin.nextSiblingElement();
    }
}

void ComponentItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    IComponentItem::hoverEnterEvent(event);
}
void ComponentItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    IComponentItem::hoverLeaveEvent(event);
}

QVariant ComponentItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemScenePositionHasChanged){
        kDebug() << change << value;
    }
    return IComponentItem::itemChange(change, value);
}

QPainterPath ComponentItem::shape() const
{
    return m_shape;
}

QVariantMap ComponentItem::data() const
{
    QVariantMap map = KTechLab::IComponentItem::data();
    QPointF pos = mapToScene(boundingRect().center());
    map.insert("x", pos.x());
    map.insert("y", pos.y());
    int flipCorrection = (transform().m22() == -1) ? 180 : 0;
    map.insert("angle", normalize(rotation()-flipCorrection));
    map.insert("flipped", (isFlipped()) ? 1 : 0);
    return map;
}

bool ComponentItem::isFlipped() const
{
    return (transform().m11() == -1) ^ (transform().m22() == -1);
}

int ComponentItem::normalize(qreal value) const
{
    while (value < 0)
        value += 360;
    return (int)value % 360;
}

#include "componentitem.moc"
// vim: sw=4 sts=4 et tw=100
