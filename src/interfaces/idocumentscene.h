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

#ifndef IDOCUMENTSCENE_H
#define IDOCUMENTSCENE_H

#include "ktlinterfacesexport.h"
#include <QtGui/QGraphicsScene>
#include <QVariantMap>

class QGraphicsSceneMouseEvent;

namespace KTechLab {

class Node;
class IComponentItem;
class IRouterPlugin;
class IRoutingInformation;

/**
 * \short A QGraphicsScene with icons connectable through (complex) routed connections
 *
 * This class is used as a base-class for Documents that need routed connections
 * between items located on a \class QGraphicsScene. All routing is handled by this
 * class using a plugin implementing the \class KTechLab::IConRouter interface.
 * The routing plugin can get information from the scene to find better routes within
 * this scene.
 *
 * This class also implements some standard behaviour for key events.
 * \author Julian Bäume <julian@svg4all.de>
 */
class KTLINTERFACES_EXPORT IDocumentScene : public QGraphicsScene
{
    Q_OBJECT
public:
    IDocumentScene(QObject* parent = 0);
    virtual ~IDocumentScene();

    /**
     * Returns true, if the user is just routing a connections within this
     * scene.
     *
     * \returns true, if the user is routing, false otherwise
     */
    bool isRouting() const;

    /**
    * Start the routing process at point \param pos
    */
    void startRouting(const QPointF &pos);
    /**
    * Abort the routing process.
    */
    void abortRouting();
    /**
    * Finish the routing process and therefore place the route.
    */
    void finishRouting();

    virtual IComponentItem* itemById(const QString& id) const =0;
    virtual Node* node(const QString& id) const =0;

    /**
     * Get the routing information stored by the routing plugin.
     *
     * Each scene can store routing information provided by the
     * plugin for later usage.
     */
    QSharedPointer<IRoutingInformation> routingInfo();
    /**
     * Set the routing information to be stored.
     *
     * Each scene can store routing information provided by the
     * plugin for later usage.
     */
    void setRoutingInfo( QSharedPointer<IRoutingInformation> info );

public slots:
    virtual void updateData( const QString &name, const QVariantMap &value );

protected:
    /**
     * Take a list of items and check if they need to be re-routed.
     * If re-routing is nessessary, it will be performed.
     *
     * \param items - the list containing items, that need checking
     */
    void rerouteConnectors(QList< QGraphicsItem* > items);

    /**
     * This method tracks mouse movement during the routing process.
     * Make sure to call this method, in case you override it.
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    /**
     * Align a given \param point to a grid.
     *
     * The scene will automatically align all graphic-items to a grid
     * that is provided by the routing plugin. You can override this
     * method to change this behaviour.
     *
     * \param point - the point to be aligned
     * \returns the aligned point
     */
    virtual QPointF alignToGrid( const QPointF& point );

    /**
     * handle default key events like delete
     */
    virtual void keyPressEvent(QKeyEvent* event);

    /**
     * Fetch a plugin implementing \class KTechLab::IRouterPlugin and return it.
     * \returns a plugin capable of mapping a route
     */
    virtual void fetchRouter();

    virtual void drawForeground(QPainter* painter, const QRectF& rect);
private:
    QGraphicsPathItem* m_routePath;
    QSharedPointer<IRoutingInformation> m_routingInfo;
    QPointF m_startPos;
    QPointF m_oldSelectionPos;
    bool m_movingSelection;
};

}

#endif // IDOCUMENTSCENE_H
