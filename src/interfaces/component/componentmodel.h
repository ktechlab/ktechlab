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
#include "../ktlinterfacesexport.h"

#include <QAbstractItemModel>

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

    void setMetaData( const KTechLab::ComponentMetaData & data );
    void setFactory( KTechLab::IComponentFactory * factory );

    int row();
    int rowCount();
    QList<ComponentItem*> children();

    KTechLab::ComponentMetaData metaData() const;
    KTechLab::IComponentFactory * factory() const;

private:
    KTechLab::IComponentFactory * m_factory;
    KTechLab::ComponentMetaData m_metaData;

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

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    virtual void setComponentData( const KTechLab::ComponentMetaData & data, KTechLab::IComponentFactory * factory );

private:
    ComponentItem *m_rootItem;
};

#endif

