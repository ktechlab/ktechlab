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

using namespace KTechLab;

ComponentItem::ComponentItem ( const QVariantMap& data, Theme *theme, QGraphicsItem* parentItem )
    : QGraphicsSvgItem ( parentItem ),
      m_renderer( new QSvgRenderer() ),
      m_theme( theme )
{
    setAcceptHoverEvents(true);
    setFlags(
        ItemIsFocusable | ItemIsSelectable |
        ItemIsMovable | ItemSendsScenePositionChanges
    );

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
    // to compensate the SVG viewBox and -QPoint(4,4) to compensate
    // the raster of the kde3 version
    pos -= QPoint(36,36);
    setPos( pos );

    m_shape.addRect(m_renderer->boundsOnElement("icon"));
    connect(this,SIGNAL(dataChanged(QString,QVariantMap)),
            this,SLOT(dataUpdated(QString,QVariantMap)));
}

ComponentItem::~ComponentItem()
{
    delete m_renderer;
}

void ComponentItem::dataUpdated( const QString &name, const QVariantMap &data )
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
        pinRect.setLeft(pin.attribute("cx").toDouble()-r);
        pinRect.setTop(pin.attribute("cy").toDouble()-r);
        pinRect.setWidth(r*2);
        pinRect.setHeight(r*2);
        new PinItem(pinRect, this, scene());
        pin = pin.nextSiblingElement();
    }
}

void ComponentItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_oldPos = pos();
    QGraphicsSvgItem::mousePressEvent(event);
}

void ComponentItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsSvgItem::mouseMoveEvent(event);
}

void ComponentItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (   event->button() == Qt::LeftButton
        && contains(event->scenePos())
        && contains(event->buttonDownScenePos(Qt::LeftButton)) ){
        if (event->modifiers() != Qt::ControlModifier)
            scene()->clearSelection();
        setSelected(true);
        event->accept();
    }
    if (pos() != m_oldPos){
        //TODO: make grid configurable
        //align to grid
        QPoint p = pos().toPoint() / 8;
        p*=8;
        setPos(p);
        if (p != m_oldPos)
            itemChange(ItemScenePositionHasChanged,pos());
    }
    QGraphicsSvgItem::mouseReleaseEvent(event);
}

void ComponentItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsSvgItem::hoverEnterEvent(event);
}
void ComponentItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsSvgItem::hoverLeaveEvent(event);
}

QVariant ComponentItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemScenePositionHasChanged){
        kDebug() << change << value;
    }
    return QGraphicsItem::itemChange(change, value);
}

QPainterPath ComponentItem::shape() const
{
    return m_shape;
}

#include "componentitem.moc"
// vim: sw=4 sts=4 et tw=100
