/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton <david@bluehaze.org>          *
 *   Copyright (C) 2010 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTECHLAB_NODE_H
#define KTECHLAB_NODE_H

#include "../ktlinterfacesexport.h"
#include <QGraphicsEllipseItem>
#include "idocumentitem.h"

namespace KTechLab {

class IDocumentScene;

/**
 * \short A standard node that can be associated with a Connector
 * \author David Saxton, Julian Bäume
 *
 * \subsection Visualisation
 * A Node will be shown on a KTechLab::IDocumentScene instance. It is represented by a small circle,
 * that might be visible or not. Visability depends on the these citeria:
 *
 * \li n > 3 connections are associated with the Node
 *      -> has a parent IComponentItem and at least 2 ConnectorItem instances connected
 *      -> has at least 3 ConnectorItem instances connected
 * \li Node is located beneath mouse-cursor
 */
class KTLINTERFACES_EXPORT Node : public QGraphicsEllipseItem, public IDocumentItem
{
public:
    /**
     * Create a Node from given data
     */
    Node(QGraphicsItem* parent = 0, IDocumentScene* scene = 0);
    virtual ~Node();

    /**
     * Set a unique identifier for this object
     */
    void setId(const QString& id);
    /**
     * Set the parent
     */
    void setParent(IDocumentItem* item);
    /**
    * Returns the unique identifier for the parent, if there is one.
    * Returns an empty string, otherwise.
    */
    QString parentId() const;
    /**
     * Returns if this Node is valid. Only Nodes within a Scene are valid.
     */
    bool isValid() const;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

private:
    bool fetchDocumentScene();
    int countConnectors() const;
    IDocumentItem* m_parent;
    IDocumentScene* m_documentScene;
};

}
#endif
