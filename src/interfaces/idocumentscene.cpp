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
#include "iconrouter.h"
#include "component/connectoritem.h"
#include <interfaces/icore.h>
#include <interfaces/iplugincontroller.h>
#include <KDebug>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

using namespace KTechLab;

IDocumentScene::IDocumentScene(QObject* parent)
    : QGraphicsScene(parent),
      m_routePath( 0 )
{

}

IDocumentScene::~IDocumentScene()
{
    delete m_routePath;
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
        IConRouter *cr = fetchRouter();
        if (!cr) {
            event->ignore();
            return;
        }
        cr->mapRoute(m_startPos, event->scenePos());
        m_routePath = new ConnectorItem();
        m_routePath->setPath(cr->paintedRoute());
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
    IConRouter *cr = fetchRouter();
    if (!cr) {
        return;
    }
    m_startPos = pos;
    cr->mapRoute(pos,pos);
    m_routePath = addPath(cr->paintedRoute());
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

IConRouter *IDocumentScene::fetchRouter() const
{
    KDevelop::IPlugin *plugin = KDevelop::ICore::self()->pluginController()->pluginForExtension("org.ktechlab.IConRouter", "ktlautomatic_router");
    if (!plugin) {
        kWarning() << "No Plugin found for extension: org.ktechlab.IConRouter";
        return 0;
    }
    IConRouter *cr = plugin->extension<IConRouter>();
    cr->setDocumentScene(this);
    return cr;
}

void IDocumentScene::drawForeground(QPainter* painter, const QRectF& rect)
{
    if (views().isEmpty())
        return;

    IConRouter *cr = fetchRouter();
    if (!cr)
	return;

    QPixmap pixmap = cr->visualizedData(rect);
    if (pixmap.isNull())
        return;

    painter->save();
    painter->drawPixmap(rect, pixmap, rect);
    painter->restore();
}

#include "idocumentscene.moc"