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
#include "component/connector.h"
#include "component/node.h"
#include "component/icomponentitem.h"
#include "idocumentmodel.h"

using namespace KTechLab;

IDocumentScene::IDocumentScene(QObject* parent)
    : QGraphicsScene(parent),
      m_routePath( 0 ),
      m_routingInfo( 0 ),
      m_movingSelection(false)
{
}

IDocumentScene::~IDocumentScene()
{
    m_routingInfo.clear();
}

bool IDocumentScene::isRouting() const
{
    return m_routePath != 0;
}

QSet< QGraphicsItem* > IDocumentScene::movingItems() const
{
    QSet<QGraphicsItem*> set;
    if (m_movingSelection)
        foreach(IComponentItem* item, filterItemList<IComponentItem>(selectedItems())){
            set << item;
        }

    return set;
}

void IDocumentScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_routePath){
        if (!routingInfo()) {
            event->ignore();
            return;
        }
        m_routingInfo->mapRoute(m_startPos, event->scenePos());
        m_routePath->setPath(m_routingInfo->paintedRoute());
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
    if (!selectedItems().isEmpty()){
        QList<IComponentItem*> componentList= filterItemList<IComponentItem>(selectedItems());
        if (componentList.isEmpty())
            return;

        m_needReroutingList.clear();
        m_oldSelectionPos = componentList.first()->pos();
        foreach(IComponentItem* item, componentList) {
            m_needReroutingList << filterItemList<ConnectorItem>(item->collidingItems());
        }
        //FIXME: tell routingInfo we moved, but actually haven't, yet
        emit componentsAboutToMove(componentList);
        m_movingSelection = true;
    }
}

template<class T> inline QList<T*> IDocumentScene::filterItemList(QList<QGraphicsItem*> list) const
{
    QList<T*> result;
    foreach(QGraphicsItem* item, list) {
        T* tItem = qgraphicsitem_cast<T*>(item);
        if (tItem) result << tItem;
    }
    return result;
}

void IDocumentScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (isRouting())
        return;
    if (!selectedItems().isEmpty() && m_movingSelection){
        QList<IComponentItem*> selectedComponents = filterItemList<IComponentItem>(selectedItems());
        if (selectedComponents.first()->pos() != m_oldSelectionPos){
            bool moved = alignToGrid(selectedComponents.first()->pos()) != m_oldSelectionPos;
            foreach (QGraphicsItem* item, selectedItems()){
                item->setPos(alignToGrid(item->pos()));
                //TODO: update model
            }
            emit componentsMoved(selectedComponents);
            if (moved) {
                QSet<ConnectorItem*> scheduledConnectors;
                foreach (ConnectorItem* c, m_needReroutingList) {
                    scheduledConnectors << c;
                }
                foreach(IComponentItem* item, selectedComponents) {
                    foreach(ConnectorItem* c, filterItemList<ConnectorItem>(item->collidingItems())){
                        scheduledConnectors << c;
                    }
                }
                rerouteConnectors(scheduledConnectors.toList());
            }
        } else {
            //FIXME: this is needed, because routingInfo thinks we moved, but actually haven't
            emit componentsMoved(selectedComponents);
        }
        m_movingSelection = false;
    }
}

void IDocumentScene::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete){
        foreach(QGraphicsItem *item,selectedItems()){
            emit itemRemoved(item);
            removeItem(item);
            delete item;
        }
        emit transactionCompleted();
    }
}

QPointF IDocumentScene::alignToGrid(const QPointF& point)
{
    if (m_routingInfo)
        return m_routingInfo->alignToGrid(point);

    return point;
}

ConnectorItem* IDocumentScene::startRouting(const QPointF& pos)
{
    if (!routingInfo()) {
        kError() << "Can't start routing without routing plugin";
        return 0;
    }
    m_startPos = pos;
    m_routingInfo->mapRoute(pos,pos);
    m_routePath = new ConnectorItem(this);
    m_routePath->setPath(m_routingInfo->paintedRoute());
    return m_routePath;
}

void IDocumentScene::abortRouting()
{
    if (!m_routePath)
        return;

    removeItem(m_routePath);
    delete m_routePath;
    m_routePath = 0;
    emit transactionAborted();
}

ConnectorItem* IDocumentScene::finishRouting(const QPointF& pos)
{
    ConnectorItem* c = m_routePath;

    m_routingInfo->mapRoute(m_startPos,pos);
    m_routePath->setPath(m_routingInfo->paintedRoute());
    // this item is still part of the scene, we just forget about it, here
    m_routePath = 0;
    QList< ConnectorItem* > items;
    items << c;
    emit routed(items);
    emit transactionCompleted();

    return c;
}

void IDocumentScene::updateData(const QString& name, const QVariantMap& value)
{

}

void IDocumentScene::rerouteConnectors(QList< ConnectorItem* > items)
{
    if (!m_routingInfo)
        return;

    emit aboutToReroute(items);
    foreach (ConnectorItem* c, items){
        QPointF start = c->startNode()->scenePos();
        QPointF end = c->endNode()->scenePos();
        m_routingInfo->mapRoute(start,end);
        c->setPath(m_routingInfo->paintedRoute());
    }
    emit routed(items);
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

void IDocumentScene::drawBackground(QPainter* painter, const QRectF& rect)
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
