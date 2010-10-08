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
#include <QGraphicsItem>

class QGraphicsSceneMouseEvent;

namespace KTechLab {

struct GraphicsItems {
    enum {
        ConnectorItemType = QGraphicsItem::UserType+1,
        ComponentItemType = QGraphicsItem::UserType+2
    };
};

class Node;
class IComponentItem;
class ConnectorItem;
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
    *
    * \returns the connector item that represents the connection
    */
    ConnectorItem* startRouting(const QPointF &pos);
    /**
    * Abort the routing process.
    */
    void abortRouting();
    /**
    * Finish the routing process and therefore place the route to \param pos.
    *
    * \returns the connector item that represents the connection
    */
    ConnectorItem* finishRouting(const QPointF &pos);

    virtual IComponentItem* item(const QString& id) const =0;
    virtual Node* node(const QString& id) const =0;

    /**
     * Provide a set of items, that are moving at this moment.
     *
     * Note: for now, only ComponentItems can be moved and will be in this list
     */
    QSet<QGraphicsItem*> movingItems() const;

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

signals:
    /**
     * Emitted, when the scene is about to reroute some ConnectorItems.
     */
    void aboutToReroute(QList<KTechLab::ConnectorItem*>);
    /**
     * Emitted, when the scene routed some ConnectorItems.
     */
    void routed(QList<KTechLab::ConnectorItem*>);
    /**
     * Emitted, when some components are about to be moved.
     */
    void componentsAboutToMove(QList<KTechLab::IComponentItem*>);
    /**
     * Emitted, when some components have been moved.
     */
    void componentsMoved(QList<KTechLab::IComponentItem*>);
    /**
     * Emitted, when some item is removed from the scene.
     */
    void itemRemoved( QGraphicsItem* );

    /**
     * Emitted when the user aborts his current action. The scene will be
     * reset to the last well-defined state.
     *
     * \sa transactionCompleted
     */
    void transactionAborted();

    /**
     * Emitted when the user finishes his current action. This will change the
     * scene permanently.
     */
    void transactionCompleted();

public slots:
    virtual void updateData( const QString &name, const QVariantMap &value );

protected:
    /**
     * Take a list of items and check if they need to be re-routed.
     * If re-routing is nessessary, it will be performed.
     *
     * \param items - the list containing items, that need checking
     */
    void rerouteConnectors(QList< ConnectorItem* > items);

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

    virtual void drawBackground(QPainter* painter, const QRectF& rect);
private:
    template<class T> inline QList<T*> filterItemList(QList<QGraphicsItem*> list) const;
    ConnectorItem* m_routePath;
    QSharedPointer<IRoutingInformation> m_routingInfo;
    QList<ConnectorItem*> m_needReroutingList;
    QPointF m_startPos;
    QPointF m_oldSelectionPos;
    bool m_movingSelection;
};

}

#endif // IDOCUMENTSCENE_H
