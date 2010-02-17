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
#include <interfaces/icore.h>
#include <interfaces/iplugincontroller.h>
#include <KDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPathItem>

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

bool IDocumentScene::isRouting()
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
                cr->mapRoute(event->buttonDownScenePos(Qt::LeftButton), event->scenePos());
                m_routePath = addPath(cr->paintedRoute());
                event->accept();
    } else {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void IDocumentScene::startRouting(const QPointF& pos)
{
    IConRouter *cr = fetchRouter();
    if (!cr) {
        return;
    }
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

#include "idocumentscene.moc"