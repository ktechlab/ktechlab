/*
    Copyright (C) 2003-2004  David Saxton <david@bluehaze.org>
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

#include "ktlautomaticrouterplugin.h"
#include "cells.h"

#include <KGenericFactory>
#include <KAboutData>
#include <KDebug>
#include <QPointF>
#include <interfaces/idocumentscene.h>

K_PLUGIN_FACTORY(KTLAutomaticRouterFactory, registerPlugin<AutomaticRouter>(); )
K_EXPORT_PLUGIN(KTLAutomaticRouterFactory(
    KAboutData("ktlautomatic_router","ktlautomatic_router",
               ki18n("KTechLab Automatic Router"), "0.1",
               ki18n("Automatic Routing in Circuit files"),
               KAboutData::License_LGPL))
)

AutomaticRouter::AutomaticRouter(QObject* parent, const QVariantList& args)
    : IPlugin( KTLAutomaticRouterFactory::componentData(), parent)
{
    KDEV_USE_EXTENSION_INTERFACE( KTechLab::IRouterPlugin )
}

void AutomaticRouter::generateRoutingInfo(KTechLab::IDocumentScene* scene)
{
    Cells* cells = new Cells(scene,this);
    connect(scene,SIGNAL(sceneRectChanged(QRectF)),cells,SLOT(updateSceneRect(QRectF)));
    connect(scene,SIGNAL(componentsAboutToMove(QList<KTechLab::IComponentItem*>)),
            cells,SLOT(removeComponents(QList<KTechLab::IComponentItem*>)));
    connect(scene,SIGNAL(componentsMoved(QList<KTechLab::IComponentItem*>)),
            cells,SLOT(addComponents(QList<KTechLab::IComponentItem*>)));
    connect(scene,SIGNAL(aboutToReroute(QList<KTechLab::ConnectorItem*>)),
            cells,SLOT(removeConnectors(QList<KTechLab::ConnectorItem*>)));
    connect(scene,SIGNAL(routed(QList<KTechLab::ConnectorItem*>)),
            cells,SLOT(addConnectors(QList<KTechLab::ConnectorItem*>)));
    connect(scene,SIGNAL(itemRemoved(QGraphicsItem*)),
            cells,SLOT(removeGraphicsItem(QGraphicsItem*)));
    scene->setRoutingInfo(QSharedPointer<Cells>(cells));
}

#include "ktlautomaticrouterplugin.moc"
