/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
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

using namespace KTechLab;

ComponentItem::ComponentItem ( const QVariantMap& data, Theme *theme, QGraphicsItem* parentItem )
    : QGraphicsSvgItem ( parentItem ),
      m_renderer( new QSvgRenderer() ),
      m_theme( theme )
{
    setAcceptHoverEvents(true);
    setFlags(
        QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable |
        QGraphicsItem::ItemIsMovable
    );
    // may be the renderer should provided by the Theme. Then different renderers for
    // the same component can be shared
    QString fileName = m_theme->findFirstFile( data.value("fileName").toString() );
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

void ComponentItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    setCursor(Qt::ClosedHandCursor);
    event->accept();
}

void ComponentItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
        .length() < QApplication::startDragDistance()) {
        event->ignore();
        return;
    }

    if (!m_dragged && !m_posBeforeDrag.isNull()) {
        //dragging has been aborted
        event->ignore();
        return;
    } else if (!m_dragged && boundingRect().contains( event->buttonDownPos(Qt::LeftButton) )){
        //begin dragging
        m_posBeforeDrag = scenePos();
        m_dragged = true;
        setFocus(Qt::MouseFocusReason);
    } else if ( m_dragged ){
        setPos(event->scenePos() - event->buttonDownPos(Qt::LeftButton));
        event->accept();
    }
}

void ComponentItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    setCursor(Qt::ArrowCursor);
    if (!m_dragged){
        //reset all dragging related members
        m_posBeforeDrag = QPointF();
        event->ignore();
        return;
    }

    QVariantMap data;
    data.insert("position",scenePos());
    emit dataChanged( "position", data);
    m_dragged = false;
    m_posBeforeDrag = QPointF();
    event->accept();
}

void ComponentItem::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && m_dragged){
        setPos(m_posBeforeDrag);
        m_dragged = false;
        event->accept();
    } else {
        event->ignore();
    }
}

void ComponentItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(Qt::OpenHandCursor);
    event->accept();
}
void ComponentItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(Qt::ArrowCursor);
    event->accept();
}

#include "componentitem.moc"
// vim: sw=4 sts=4 et tw=100
