/*
    Copyright (C) 2009-2010 Julian Bäume <julian@svg4all.de>

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

#ifndef KTECHLAB_ICOMPONENTITEM_H
#define KTECHLAB_ICOMPONENTITEM_H

#include "../ktlinterfacesexport.h"
#include <QGraphicsSvgItem>

namespace KTechLab {

class IDocumentModel;

/**
 * \short Components that are shown on a scene
 *
 * This is a base-class for components which are rendered onto \ref IDocumentScene.
 * Each component needs an id which is unique within a document. Each component needs
 * to notify the model after it has been altered by user-interaction.
 */
class KTLINTERFACES_EXPORT IComponentItem : public QGraphicsSvgItem
{
public:
    IComponentItem(QGraphicsItem* parentItem = 0);
    ~IComponentItem();

    /**
     * Set the document model for this component. This will be used to notify
     * the model about changes and to retrieve information about itself.
     */
    void setDocumentModel(IDocumentModel* model);
    /**
     * Get the document model.
     */
    IDocumentModel* documentModel() const;

    /**
     * Get the id of this component.
     */
    QString id() const;

public slots:
    /**
     * Inform the component, that some data has been changed, so it can
     * update it's data and take appropriate action.
     *
     * @param name - tell, which data has been changed
     * @param data - some data that is important for the object to know
     */
    virtual void updateData( const QString &name, const QVariantMap &data );

signals:
    void dataUpdated( const QString &name, const QVariantMap &data );

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

    IDocumentModel *m_document;
    QString m_id;
};

}

#endif // KTECHLAB_ICOMPONENTITEM_H
