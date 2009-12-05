/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef COMPONENTMODEL_H
#define COMPONENTMODEL_H

#include "icomponent.h"
#include "icomponentplugin.h"
#include "../ktlinterfacesexport.h"

#include <QAbstractItemModel>

namespace KTechLab
{

class IComponentFactory;

class ComponentItem
{
public:
    ComponentItem();
    ~ComponentItem();

    void addChild( ComponentItem * child );
    void setParent( ComponentItem * parent );
    ComponentItem * parent();

    ComponentItem * child( int row );
    ComponentItem * child( const QString &key );

    void setMetaData( const ComponentMetaData & data );
    void setFactory( IComponentFactory * factory );

    int row();
    int rowCount();
    QList<ComponentItem*> children();

    ComponentMetaData metaData() const;
    IComponentFactory * factory() const;

private:
    IComponentFactory * m_factory;
    ComponentMetaData m_metaData;

    QMultiMap<QString,ComponentItem*> m_children;
    ComponentItem * m_parent;
};

/**
 * This class implements QAbstractItemModel to provide a tree of compoments sorted into groups like
 * logical compoments, discrete components and so on. Different component libraries/plugins should add the
 * compoments they provide to this list to make them available to the user.
 */
class KTLINTERFACES_EXPORT ComponentModel: public QAbstractItemModel
{
Q_OBJECT
public:
    ComponentModel();
    virtual ~ComponentModel();

    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent( const QModelIndex & index ) const;

    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;

    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    virtual QMimeData *mimeData( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes() const;

    /**
     * Add the ComponentMetaData and the corresponding factory to the model
     */
    virtual void insertComponentData( const KTechLab::ComponentMetaData & data, KTechLab::IComponentFactory * factory );

private:
    ComponentItem *m_rootItem;
};

}
#endif

