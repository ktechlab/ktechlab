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

#ifndef ICONROUTER_H
#define ICONROUTER_H

#include "ktlinterfacesexport.h"

#include <QtCore/QObject>
#include <kdevplatform/interfaces/iextension.h>
#include <qpainter.h>

class QPointF;
class QPainterPath;

namespace KTechLab
{

class IDocumentModel;
/**
 * \short Abstraction for the routing of a connection
 * This is a base-class to provide a list of points that need to be connected
 * in order to draw a connection between 2 pins of a component.
 */
class KTLINTERFACES_EXPORT IConRouter
{
public:
    IConRouter();
    virtual ~IConRouter();

    /**
     * What this class is all about - finding a route, from p1 to p2.
     * This is the default way to map a route. Override this method
     * to provide an implementation for the routing algorithm.
     * \param p1 - starting point of the route
     * \param p2 - end-point of the route
     */
    virtual void mapRoute( QPointF p1, QPointF p2 )=0;

    /**
     * What this class is all about - finding a route, from (sx,sy) to (ex,ey).
     * The default implementation calls mapRoute(QPointF(sx,sy),QPointF(ex,ey)).
     * This method is provided for convenience.
     */
    virtual void mapRoute( qreal sx, qreal sy, qreal ex, qreal ey);

    /**
     * Set the model for the document to provide necessary information to
     * calculate the route. This way the router can take components into
     * account.
     * \param model - the model to set
     */
    void setDocumentModel( IDocumentModel *model );

    /**
     * Get the route in a paintable format. This can be directly used
     * with the QGraphicsView framework.
     * \returns the path describing the route
     */
    QPainterPath paintedRoute() const;

    /**
     * Get the list of points representing the route
     * \returns the list of points representing the route
     */
    virtual QList<QPointF> route() const;

    /**
     * Provide a list of points representing the route.
     * \param route - the points on the route
     */
    virtual void setRoute( const QList<QPointF> &route );

protected:
    QList<QPointF> m_route;
    IDocumentModel *m_documentModel;
};

}

KDEV_DECLARE_EXTENSION_INTERFACE_NS( KTechLab, IConRouter, "org.ktechlab.IConRouter" )
Q_DECLARE_INTERFACE( KTechLab::IConRouter, "org.ktechlab.IConRouter" )

#endif // ICONROUTER_H
