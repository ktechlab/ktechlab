/*
    Abstraction for the routing of a connection
    Copyright (C) 2003-2004 by David Saxton <david@bluehaze.org>
    Copyright (C) 2010  Julian Bäume <julian@svg4all.de>

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

#ifndef IROUTERPLUGIN_H
#define IROUTERPLUGIN_H

#include "ktlinterfacesexport.h"

#include <QtCore/QObject>

#if KDEV_PLUGIN_VERSION <= 10
#include <interfaces/iextension.h>
#endif

class QPointF;
class QPainterPath;

namespace KTechLab
{

class IRoutingInformation;
class IDocumentScene;

/**
 * \short Abstraction for the routing of a connection
 * This is a base-class to provide a list of points that need to be connected
 * in order to draw a connection between 2 pins of a component.
 */
class KTLINTERFACES_EXPORT IRouterPlugin
{
public:

    IRouterPlugin();
    virtual ~IRouterPlugin();

    /**
     * Set the scene for the document to provide necessary information to
     * calculate the route. This way the router can take components into
     * account. The scene will also provide mechanisms for collision
     * detection.
     * \param scene - the scene to set
     */
    void setDocumentScene( IDocumentScene *scene );

protected:
    /**
     * Create internal routing information and attach it to the scene.
     *
     * \param scene - routing information is created for this scene
     */
    virtual void generateRoutingInfo( IDocumentScene *scene )=0;
};

}

#if KDEV_PLUGIN_VERSION <= 10
KDEV_DECLARE_EXTENSION_INTERFACE_NS(KTechLab, IRouterPlugin, "org.ktechlab.IRouterPlugin")
#endif

Q_DECLARE_INTERFACE( KTechLab::IRouterPlugin, "org.ktechlab.IRouterPlugin" )

#endif // IROUTERPLUGIN_H
