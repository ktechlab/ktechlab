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
#include <kdevplatform/interfaces/iextension.h>
#include <qpainter.h>

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

    enum VisualizeOption {
        NoVisualization = 0x0, /** No visualization at all */
        VisualizeRaster = 0x01, /** Visualize a raster that is used internally */
        VisualizeRoutingInformation = 0x02 /** Visualize internal routing information */
    };
    Q_DECLARE_FLAGS(VisualizeOptions,VisualizeOption)

    IRouterPlugin();
    virtual ~IRouterPlugin();

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
     * Set the scene for the document to provide necessary information to
     * calculate the route. This way the router can take components into
     * account. The scene will also provide mechanisms for collision
     * detection.
     * \param scene - the scene to set
     */
    void setDocumentScene( IDocumentScene *scene );

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

    /**
     * Provide a \ref QPixmap that visualizes the internal data
     * of the routing algorithm.
     *
     * This can be useful to debug or understand the underlying
     * routing algorithm. Since this method will be called quite
     * frequently by the scene, make sure the pixmap is created
     * and cached in the plugin.
     *
     * The rectangle region can be provided to only visualize parts
     * of the scene.
     *
     * This method will provide fore-ground data for a raster, that
     * visualizes the internal structure of the scene and other internal
     * values, that are used to calculate the route. Each of this data
     * can be switched on and off individually by setting visualization flags
     * using \ref setVisualizationFlags.
     *
     * \param region - a \ref QRectF describing the region of interest
     * \returns a \ref QPixmap visualizing the internal data,
     *          the default implementation will return a null-pixmap
     */
    QPixmap visualizedData( const QRectF &region = QRectF() ) const;

protected:
    /**
     * Paint a raster used internally by the routing plugin.
     * This can help the user when positioning the components.
     *
     * The default implementation does nothing.
     *
     * \param p - the QPainter that should be painted on
     * \param region - the region of interest
     */
    virtual void paintRaster(QPainter* p, const QRectF& region) const;
    /**
     * Paint internal routing information. This can be anything that visualizes,
     * which data is the base for the routing decisions. The region of interest can
     * be larger than the actual \ref IDocumentScene::sceneRect, so only the intersecting
     * part of the region needs to be painted. The two parameters are rects with the same
     * size, but different relation points. The \param target rect defines the position
     * relative to the region of interest. The \param source rect defines the position
     * relative to the scene rect. If your data is in the same koordinate system as the
     * scene, the two rects will be equal.
     *
     * The default implementation does nothing.
     *
     * \param target - position of the intersecting part relative to the region of interest
     * \param source - position of the intersecting part relative to the scene rect
     *
     * You can use that in \ref QPainter::drawPixmap or \ref QPainter::drawImage to
     * paint a region from the scene rect into a region of the region of interest.
     */
    virtual void paintRoutingInfo(QPainter* p, const QRectF& target, const QRectF& source) const;
    /**
     * Create internal routing information and attach it to the scene.
     *
     * \param scene - routing information is created for this scene
     */
    virtual void generateRoutingInfo( IDocumentScene *scene )=0;
    QList<QPointF> m_route;
    QSharedPointer<IRoutingInformation> m_routingInfo;
    IDocumentScene* m_documentScene;

private:
    VisualizeOptions m_visualize;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IRouterPlugin::VisualizeOptions)

}

KDEV_DECLARE_EXTENSION_INTERFACE_NS( KTechLab, IRouterPlugin, "org.ktechlab.IRouterPlugin" )
Q_DECLARE_INTERFACE( KTechLab::IRouterPlugin, "org.ktechlab.IRouterPlugin" )

#endif // IROUTERPLUGIN_H
