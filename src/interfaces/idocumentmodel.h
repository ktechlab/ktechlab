/*
    Copyright (C) 2009-2010  Julian Bäume <julian@svg4all.de>

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

#ifndef IDOCUMENTMODEL_H
#define IDOCUMENTMODEL_H

#include "ktlinterfacesexport.h"
#include <QAbstractItemModel>
#include <QDomDocument>

class QTextDocument;
namespace KTechLab
{

class IComponent;
class IDocumentModelPrivate;

/**
 * \short A model to represent documents with components connected by connectors
 *
 * This is a base class for a table based model describing documents that contain
 * some sort of components that are connected by connectors. Data is stored
 * as \ref QVariantMap and must at least have a value for the "id" key and a unique
 * id stored as it's value.
 *
 * The data is represented as an adjacent matrix with all components on the main diagonal
 * and valid connectors at all other positions if there is a connection between the components
 * corresponding to the rows and columns of the matrix. So, if there is a connection from
 * the component at (i,i) to the component at (j,j) there will be valid data for
 * a connector at (i,j).
 */
class KTLINTERFACES_EXPORT IDocumentModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    IDocumentModel ( QDomDocument doc, QObject* parent = 0 );
    virtual ~IDocumentModel();

    /**
     * Special roles for KTechLab::IDocumentModel extending the Qt::ItemDataRole enum
     */
    enum {
        XMLDataRole = Qt::UserRole /** XML representation of the data */
    };

    /**
     * Add a valid component to the model. A component must at least have
     * a field "id" which has to be unique. Adding a component with an id that
     * already is in the model is not possible.
     *
     * @param component - A \ref QVariantMap containing all data for the component
     * @return true if the component was added to the model
     */
    virtual void addComponent ( const QVariantMap& component );
    /**
     * Get a \ref QVariantMap containing all components mapped from id
     * to a \ref QVariantMap containing all the data for the component.
     *
     * @return all components
     */
    virtual QVariantMap components() const;
    /**
     * Get a \ref QVariantMap containing all the data for the component specified
     * by the given key.
     *
     * @param key - the id of the component
     * @return a \ref QVariantMap containing the data
     */
    virtual QVariantMap component( const QString &key ) const;
    /**
    * Add a valid connector to the model. A connector must at least have
    * a field "id" which has to be unique. Adding a connector with an id that
    * already is in the model is not possible.
    *
    * @param component - A \ref QVariantMap containing all data for the component
    * @return true if the component was added to the model
    */
    virtual void addConnector ( const QVariantMap& connector );
    /**
    * Get a \ref QVariantMap containing all connectors mapped from id
    * to a \ref QVariantMap containing all the data for the connector.
    *
    * @return all connectors
    */
    virtual QVariantMap connectors() const;
    /**
    * Get a \ref QVariantMap containing all the data for the connector specified
    * by the given key.
    *
    * @param key - the id of the connector
    * @return a \ref QVariantMap containing the data
    */
    virtual QVariantMap connector( const QString &key ) const;

    /**
     * Add an individual node to the model, represented by a \ref QVariantMap.
     * Nodes are used as endpoints for connectors, and they don't belong
     * to any component.
     * A node must at least have
     * a field "id" which has to be unique. Adding a connector with an id that
     * already is in the model is not possible.
     *
     * @param node - A \ref QVariantMap containing all data for the node
     */
    virtual void addNode( const QVariantMap& node );
    /**
    * Get a \ref QVariantMap containing all nodes mapped from id
    * to a \ref QVariantMap containing all the data for the node.
    *
    * @return all nodes
    */
    virtual QVariantMap nodes() const;
    /**
    * Get a \ref QVariantMap containing all the data for the node specified
    * by the given key.
    *
    * @param key - the id of the node
    * @return a \ref QVariantMap containing the data
    */
    virtual QVariantMap node( const QString& id );

    /**
     * Get the data at a given field in the matrix.
     *
     * @param index - the \ref QModelIndex representing the field in the matrix
     * @param role - the role of the data
     * @return the data at the given index
     */
    virtual QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    /**
     * Get the number of columns of the adjacent matrix. This equals the number of rows and
     * the number of components.
     *
     * @return the number of columns of the matrix
     */
    int columnCount ( const QModelIndex& parent = QModelIndex() ) const;
    /**
     * Get the number of rows of the adjacent matrix. This equals the number of columns and
     * the number of components.
     *
     * @return the number of rows of the matrix
     */
    int rowCount ( const QModelIndex& parent = QModelIndex() ) const;
    /**
     * Set data for a given field in the matrix.
     *
     * @param index - the \ref QModelIndex referencing the field of the matrix
     * @param value - the value of the field as \ref QVariant
     * @param role - the role which is responsible for setting the data
     * @return true, if the data was set successful
     */
    virtual bool setData ( const QModelIndex& index, const QVariant& value,
                           int role = Qt::EditRole );

    Qt::ItemFlags flags( const QModelIndex &index ) const;

    QVariant headerData( int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole ) const;


    QModelIndex index( int row, int column,
                       const QModelIndex &parent = QModelIndex() ) const;

    /**
     * Get an index for the given item. This method is provided by convenience
     * and will look the item up in the internal data and return a QModelIndex
     * to that data.
     *
     * The item will be looked up by id, so the
     * \param item should contain a (string) field "id".
     *
     * \returns a valid QModelIndex if the item is found
     */
    QModelIndex index( const QVariantMap& item ) const;

    QModelIndex parent( const QModelIndex &child ) const;

    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

    /**
     * Get a QTextDocument representation of the internal data. It will contain
     * all data of the model encoded as XML text. It can be used to provide
     * undo/redo functionality and be saved to disk.
     */
    QTextDocument* textDocument() const;

    /**
     * Generate a unique id for a component in the document, this model repesents.
     */
    virtual QString generateUid( const QString& name );

public slots:
    /**
     * \sa QAbstractItemModel
     */
    virtual bool submit();
    /**
     * \sa QAbstractItemModel
     */
    virtual void revert();
    virtual void updateData( const QString& id, const QVariantMap& data );

signals:
    void dataUpdated( const QString &name, const QVariantList &data );

protected:
    const QDomNode domNode( const QModelIndex& index ) const;

private:
    IDocumentModelPrivate* d;
    friend class IDocumentModelPrivate;
};

}
#endif // IDOCUMENTMODEL_H
