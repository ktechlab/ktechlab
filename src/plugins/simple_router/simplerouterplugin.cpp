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


#include "simplerouterplugin.h"
#include "simpleroutinginformation.h"
#include <KAboutData>
#include <KGenericFactory>
#include <interfaces/idocumentscene.h>

using namespace KTechLab;

K_PLUGIN_FACTORY(KTLSimpleRouterFactory, registerPlugin<SimpleRouterPlugin>(); )
K_EXPORT_PLUGIN(KTLSimpleRouterFactory(
    KAboutData("ktlsimple_router","ktlsimple_router",
               ki18n("KTechLab Simple Router"), "0.1",
               ki18n("Simple Routing in Circuit files"),
               KAboutData::License_LGPL))
)

SimpleRouterPlugin::SimpleRouterPlugin(QObject* parent, const QVariantList& args)
    : IPlugin( KTLSimpleRouterFactory::componentData(), parent)
{
    KDEV_USE_EXTENSION_INTERFACE( KTechLab::IRouterPlugin )
}

void SimpleRouterPlugin::generateRoutingInfo(KTechLab::IDocumentScene* scene)
{
    SimpleRoutingInformation* info = new SimpleRoutingInformation(scene, this);
    scene->setRoutingInfo(QSharedPointer<SimpleRoutingInformation>(info));
}

