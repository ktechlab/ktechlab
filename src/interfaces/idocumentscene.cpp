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

class IDocumentScene::IDocumentScenePrivate
{
public:
    IDocumentScenePrivate()
        :   routePath( 0 ), routingInfo( 0 ),
            movingSelection(false)
    {}

    template<class T> inline QList<T*> filterItemList(QList<QGraphicsItem*> list) const
    {
        QList<T*> result;
        foreach(QGraphicsItem* item, list) {
            T* tItem = qgraphicsitem_cast<T*>(item);
            if (tItem) result << tItem;
        }
        return result;
    };

    ConnectorItem* routePath;
    QSharedPointer<IRoutingInformation> routingInfo;
    QList<ConnectorItem*> needReroutingList;
    QPointF startPos;
    QPointF oldSelectionPos;
    bool movingSelection;
};

IDocumentScene::IDocumentScene(QObject* parent)
    : QGraphicsScene(parent),
      d( new IDocumentScenePrivate() )
{
}

IDocumentScene::~IDocumentScene()
{
    d->routingInfo.clear();
}

bool IDocumentScene::isRouting() const
{
    return d->routePath != 0;
}

QSet< QGraphicsItem* > IDocumentScene::movingItems() const
{
    QSet<QGraphicsItem*> set;
    if (d->movingSelection)
        foreach(IComponentItem* item, d->filterItemList<IComponentItem>(selectedItems())){
            set << item;
        }

    return set;
}

void IDocumentScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (d->routePath){
        if (!routingInfo()) {
            event->ignore();
            return;
        }
        d->routingInfo->mapRoute(d->startPos, event->scenePos());
        d->routePath->setRoute(d->routingInfo->paintedRoute());
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
        QList<IComponentItem*> componentList = d->filterItemList<IComponentItem>(selectedItems());
        if (componentList.isEmpty())
            return;

        d->needReroutingList.clear();
        d->oldSelectionPos = componentList.first()->pos();
        foreach(IComponentItem* item, componentList) {
            d->needReroutingList << d->filterItemList<ConnectorItem>(item->collidingItems());
        }
        //FIXME: tell routingInfo we moved, but actually haven't, yet
        emit componentsAboutToMove(componentList);
        d->movingSelection = true;
    }
}

void IDocumentScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (isRouting())
        return;
    if (!selectedItems().isEmpty() && d->movingSelection){
        QList<IComponentItem*> selectedComponents = d->filterItemList<IComponentItem>(selectedItems());
        if (selectedComponents.first()->pos() != d->oldSelectionPos){
            bool moved = alignToGrid(selectedComponents.first()->pos()) != d->oldSelectionPos;
            foreach (QGraphicsItem* item, selectedItems()){
                item->setPos(alignToGrid(item->pos()));
            }
            emit componentsMoved(selectedComponents);
            if (moved) {
                QSet<ConnectorItem*> scheduledConnectors;
                foreach (ConnectorItem* c, d->needReroutingList) {
                    scheduledConnectors << c;
                }
                foreach(IComponentItem* item, selectedComponents) {
                    foreach(ConnectorItem* c, d->filterItemList<ConnectorItem>(item->collidingItems())){
                        scheduledConnectors << c;
                    }
                }
                rerouteConnectors(scheduledConnectors.toList());
                emit transactionCompleted();
            }
        } else {
            //FIXME: this is needed, because routingInfo thinks we moved, but actually haven't
            emit componentsMoved(selectedComponents);
        }
        d->movingSelection = false;
    }
}

void IDocumentScene::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete){
        foreach(QGraphicsItem *item, selectedItems()){
            IDocumentItem* docItem = dynamic_cast<IDocumentItem*>(item);
            Q_ASSERT(docItem);

            emit itemRemoved(docItem);
            removeItem(item);
            delete item;
        }
        emit transactionCompleted();
    }
}

QPointF IDocumentScene::alignToGrid(const QPointF& point)
{
    if (d->routingInfo)
        return d->routingInfo->alignToGrid(point);

    return point;
}

ConnectorItem* IDocumentScene::startRouting(const QPointF& pos)
{
    if (!routingInfo()) {
        kError() << "Can't start routing without routing plugin";
        return 0;
    }
    d->startPos = pos;
    d->routingInfo->mapRoute(pos,pos);
    d->routePath = new ConnectorItem(this);
    QList<Node*> nodes = d->filterItemList<Node>(items(pos));
    if (!nodes.isEmpty())
        d->routePath->setStartNode(nodes.first());
    d->routePath->setRoute(d->routingInfo->paintedRoute());
    return d->routePath;
}

void IDocumentScene::abortRouting()
{
    if (!d->routePath)
        return;

    removeItem(d->routePath);
    delete d->routePath;
    d->routePath = 0;
    emit transactionAborted();
}

ConnectorItem* IDocumentScene::finishRouting(const QPointF& pos)
{
    ConnectorItem* c = d->routePath;

    d->routingInfo->mapRoute(d->startPos,pos);
    d->routePath->setRoute(d->routingInfo->paintedRoute());
    QList<Node*> nodes = d->filterItemList<Node>(items(pos));
    if (!nodes.isEmpty())
        d->routePath->setEndNode(nodes.first());

    // this item is still part of the scene, we just forget about it, here
    d->routePath = 0;
    emit routeCreated(c);
    emit transactionCompleted();

    return c;
}

void IDocumentScene::updateData(const QString& name, const QVariantMap& value)
{

}

void IDocumentScene::rerouteConnectors(QList< ConnectorItem* > items)
{
    if (!d->routingInfo)
        return;

    emit aboutToReroute(items);
    foreach (ConnectorItem* c, items){
        QPointF start = c->startNode()->scenePos();
        QPointF end = c->endNode()->scenePos();
        d->routingInfo->mapRoute(start,end);
        c->setRoute(d->routingInfo->paintedRoute());
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
    if (!d->routingInfo)
        fetchRouter();

    return d->routingInfo;
}

void IDocumentScene::setRoutingInfo(QSharedPointer< IRoutingInformation > info)
{
    d->routingInfo = info;
}

void IDocumentScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    if (views().isEmpty())
        return;

    if (!routingInfo())
        return;

    const QPixmap& pixmap = d->routingInfo->visualizedData(rect);
    if (pixmap.isNull())
        return;

    painter->save();
    painter->drawPixmap(rect, pixmap, QRectF(QPointF(0,0),rect.size()));
    painter->restore();
}

#include "idocumentscene.moc"
