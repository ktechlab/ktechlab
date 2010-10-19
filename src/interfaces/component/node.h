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

namespace KTechLab {

class IDocumentItem;


/**
 * \short A standard node that can be associated with a Connector
 * \author David Saxton, Julian Bäume
 */
class KTLINTERFACES_EXPORT Node : public QGraphicsEllipseItem
{
public:
    /**
     * Create a Node from given data
     */
    Node(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
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
     * Get the unique identifier of this object
     */
    QString id() const;
    /**
    * Returns the unique identifier for the parent, if there is one.
    * Returns an empty string, otherwise.
    */
    QString parentId() const;
    /**
     * Returns if this Node is valid. Only Nodes within a Scene are valid.
     */
    bool isValid() const;

private:
    QString m_id;
    IDocumentItem* m_parent;
};

}
#endif
