/*
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

#include "idocumentscene.h"
#include "irouterplugin.h"
#include "iroutinginformation.h"
#include "component/connectoritem.h"
#include <interfaces/icore.h>
#include <interfaces/iplugincontroller.h>
#include <KDebug>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

using namespace KTechLab;

IDocumentScene::IDocumentScene(QObject* parent)
    : QGraphicsScene(parent),
      m_routePath( 0 ),
      m_routingInfo( 0 )
{
}

IDocumentScene::~IDocumentScene()
{
    delete m_routePath;
    m_routingInfo.clear();
}

bool IDocumentScene::isRouting() const
{
    return m_routePath != 0;
}

void IDocumentScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_routePath){
        removeItem(m_routePath);
        delete m_routePath;
        m_routePath = 0;
        if (!routingInfo()) {
            event->ignore();
            return;
        }
        m_routingInfo->mapRoute(m_startPos, event->scenePos());
        m_routePath = new ConnectorItem();
        m_routePath->setPath(m_routingInfo->paintedRoute());
        addItem(m_routePath);
        event->accept();
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void IDocumentScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mousePressEvent(event);
    if (!event->isAccepted()){
        abortRouting();
    }
}

void IDocumentScene::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete){
        foreach(QGraphicsItem *item,selectedItems()){
            removeItem(item);
            delete (item);
            item = 0;
        }
    }
}


void IDocumentScene::startRouting(const QPointF& pos)
{
    if (!routingInfo()) {
        return;
    }
    m_startPos = pos;
    m_routingInfo->mapRoute(pos,pos);
    m_routePath = addPath(m_routingInfo->paintedRoute());
}

void IDocumentScene::abortRouting()
{
    if (!m_routePath)
        return;

    removeItem(m_routePath);
    delete m_routePath;
    m_routePath = 0;
}

void IDocumentScene::finishRouting()
{
    // this item is still part of the scene, we just forget about it, here
    m_routePath = 0;
}

void IDocumentScene::updateData(const QString& name, const QVariantMap& value)
{

}

void IDocumentScene::fetchRouter()
{
    KDevelop::IPluginController *pc = KDevelop::ICore::self()->pluginController();
    IRouterPlugin* router = pc->extensionForPlugin<IRouterPlugin>("org.ktechlab.IRouterPlugin", "ktlautomatic_router");
    if (!router) {
        kWarning() << "No Plugin found for extension: org.ktechlab.IRouterPlugin";
        return;
    }
    router->setDocumentScene(this);
}

QSharedPointer< IRoutingInformation > IDocumentScene::routingInfo()
{
    if (!m_routingInfo)
        fetchRouter();

    return m_routingInfo;
}

void IDocumentScene::setRoutingInfo(QSharedPointer< IRoutingInformation > info)
{
    m_routingInfo = info;
}

void IDocumentScene::drawForeground(QPainter* painter, const QRectF& rect)
{
    if (views().isEmpty())
        return;

    if (!routingInfo())
        return;

    const QPixmap& pixmap = m_routingInfo->visualizedData(rect);
    if (pixmap.isNull())
        return;

    painter->save();
    painter->drawPixmap(rect, pixmap, QRectF(QPointF(0,0),rect.size()));
    painter->restore();
}

#include "idocumentscene.moc"
