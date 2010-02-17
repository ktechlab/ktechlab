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

class QGraphicsSceneMouseEvent;

namespace KTechLab {

class IConRouter;

/**
 * \short A QGraphicsScene with icons connectable through (complex) routed connections
 *
 * This class is used as a base-class for Documents that need routed connections
 * between items located on a \class QGraphicsScene. All routing is handled by this
 * class using a plugin implementing the \class KTechLab::IConRouter interface.
 * The routing plugin can get information from the scene to find better routes within
 * this scene.
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
    bool isRouting();

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
protected:
    /**
     * This method tracks mouse movement during the routing process.
     * Make sure to call this method, in case you override it.
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    /**
     * Fetch a plugin implementing \class KTechLab::IConRouter and return it.
     * \returns a plugin capable of mapping a route
     */
    virtual IConRouter *fetchRouter() const;

private:
    QGraphicsPathItem* m_routePath;
};

}

#endif // IDOCUMENTSCENE_H
