/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "componentmodel.h"
#include "componentmimedata.h"

#include <KDebug>

//
// BEGIN class ComponentItem
//

ComponentItem::ComponentItem()
    :   m_parent( 0 )
{
}

ComponentItem::~ComponentItem()
{
    qDeleteAll( m_children );
}

void ComponentItem::addChild( ComponentItem *child )
{
    m_children.insert( child->metaData().category, child );
    child->setParent(this);
}

void ComponentItem::setParent( ComponentItem *parent )
{
    m_parent = parent;
}
ComponentItem * ComponentItem::parent()
{
    return m_parent;
}

ComponentItem * ComponentItem::child( int row )
{
    return children().at( row );
}

ComponentItem * ComponentItem::child( const QString &key )
{
    if (m_children.keys().contains( key )) {
        return m_children.values( key ).back();
    }
    return 0;
}

void ComponentItem::setMetaData( const KTechLab::ComponentMetaData & data )
{
    m_metaData = data;
}

void ComponentItem::setFactory( KTechLab::IComponentFactory * factory )
{
    m_factory = factory;
}

int ComponentItem::row()
{
    if (m_parent) {
        return m_parent->children().indexOf(const_cast<ComponentItem*>(this));
    }
    return 0;
}

int ComponentItem::rowCount()
{
    return m_children.values().count();
}

QList<ComponentItem*> ComponentItem::children()
{
    return m_children.values();
}

KTechLab::ComponentMetaData ComponentItem::metaData() const
{
    return m_metaData;
}

KTechLab::IComponentFactory * ComponentItem::factory() const
{
    return m_factory;
}

//
// END class ComponentItem
//

//
// BEGIN class ComponentModel
//
ComponentModel::ComponentModel()
{
    m_rootItem = new ComponentItem();
}

ComponentModel::~ComponentModel()
{
    delete m_rootItem;
}

QModelIndex ComponentModel::index( int row, int column, const QModelIndex & parent ) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    ComponentItem *parentItem;
    if (!parent.isValid()) {
        parentItem = m_rootItem;
    } else {
        parentItem = static_cast<ComponentItem*>(parent.internalPointer());
    }

    ComponentItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex ComponentModel::parent( const QModelIndex & index ) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    ComponentItem *childItem = static_cast<ComponentItem*>(index.internalPointer());
    ComponentItem *parentItem = childItem->parent();

    if ( parentItem == m_rootItem ) {
        return QModelIndex();
    }
    return createIndex(parentItem->row(), 0, parentItem);
}

int ComponentModel::rowCount( const QModelIndex & parent ) const
{
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        return m_rootItem->rowCount();
    } else {
        ComponentItem *item = static_cast<ComponentItem*>(parent.internalPointer());
        return item->rowCount();
    }
}

int ComponentModel::columnCount( const QModelIndex & parent ) const
{
    Q_UNUSED( parent )
    return 1;
}

Qt::ItemFlags ComponentModel::flags( const QModelIndex & index ) const
{
    if (!index.isValid()) {
        return Qt::ItemFlags( Qt::NoItemFlags );
    }

    ComponentItem *item = static_cast<ComponentItem*>(index.internalPointer());
    if ( item->metaData().name.isEmpty() ) {
        return Qt::ItemFlags( Qt::ItemIsEnabled );
    }
    return Qt::ItemFlags( Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled );
}

QVariant ComponentModel::data( const QModelIndex & index, int role ) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    ComponentItem *item = static_cast<ComponentItem*>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        return QVariant(item->metaData().title);
    } else if (role == Qt::DecorationRole) {
        return QVariant(item->metaData().icon);
    }

    return QVariant();
}

QMimeData *ComponentModel::mimeData( const QModelIndexList & indexes ) const
{
    KTechLab::ComponentMimeData *componentData = 0;

    //we only want to drag one item at a time
    QModelIndex index;
    if ( indexes.size() == 1 ) {
        index = indexes.first();
    }
    if (index.isValid()) {
        ComponentItem *item = static_cast<ComponentItem*>(index.internalPointer());
        componentData = new KTechLab::ComponentMimeData( item->metaData().name, item->factory() );

        //register our mimeType
        componentData->setData( "application/x-icomponent", item->metaData().name.toUtf8() );
    }
    return componentData;
}

QStringList ComponentModel::mimeTypes() const
{
    return QStringList()<<"application/x-icomponent";
}

void ComponentModel::insertComponentData( const KTechLab::ComponentMetaData & data, KTechLab::IComponentFactory * factory )
{
    ComponentItem *item = new ComponentItem();
    item->setMetaData( data );
    item->setFactory( factory );

    ComponentItem *parent = m_rootItem->child( data.category );
    if (!parent) {
        parent = new ComponentItem();
        KTechLab::ComponentMetaData fakeData =
            {
                "",
                data.category,
                "",
                KIcon(),
                ""
            };
        parent->setMetaData( fakeData );
        //we are about to insert a new row, tell the model about it so views can be updated
        beginInsertRows( index( rowCount(), 0, QModelIndex() ), 0, 0 );
        m_rootItem->addChild( parent );
        endInsertRows();
        kDebug() << "added category: " << data.category << " at " << index( parent->row(), 0, QModelIndex() );
    }
    const QModelIndex parentIndex = index( parent->row(), 0, QModelIndex() );
    //we are about to insert a new row, tell the model about it so views can be updated
    beginInsertRows( parentIndex, rowCount( parentIndex ), 0 );
    parent->addChild( item );
    endInsertRows();
}

// vim: sw=4 sts=4 et tw=100
