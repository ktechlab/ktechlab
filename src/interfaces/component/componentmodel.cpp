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

#if KDE_ENABLED
#include <KDebug>
#endif

#include <QStringList>
#include <QDebug>

using namespace KTechLab;
//
// BEGIN class ModelComponentItem
//

class KTechLab::ModelComponentItem
{
    public:
        ModelComponentItem();
        ~ModelComponentItem();

        void addChild( ModelComponentItem * child );
        void removeChild(ModelComponentItem* child);
        void setParent( ModelComponentItem * parent );
        ModelComponentItem * parent();

        ModelComponentItem * child( int row );
        ModelComponentItem * child( const QString &key );

        void setMetaData( const ComponentMetaData & data );
        void setFactory( IComponentItemFactory * factory );

        int row();
        int rowCount();
        QList<ModelComponentItem*> children();

        ComponentMetaData metaData() const;
        IComponentItemFactory * factory() const;
        IComponentItemFactory * factoryForName(QString name);

    private:
        IComponentItemFactory * m_factory;
        ComponentMetaData m_metaData;

        QMultiMap<QString,ModelComponentItem*> m_children;
        ModelComponentItem * m_parent;
};

ModelComponentItem::ModelComponentItem()
    :   m_parent( 0 )
{
}

ModelComponentItem::~ModelComponentItem()
{
    qDeleteAll( m_children );
}

void ModelComponentItem::addChild( ModelComponentItem *child )
{
    m_children.insert( child->metaData().category, child );
    child->setParent(this);
}

void ModelComponentItem::removeChild(ModelComponentItem* item)
{
    m_children.remove(item->metaData().category, item);
    delete item;
    item = 0;
}

void ModelComponentItem::setParent( ModelComponentItem *parent )
{
    m_parent = parent;
}
ModelComponentItem * ModelComponentItem::parent()
{
    return m_parent;
}

ModelComponentItem * ModelComponentItem::child( int row )
{
    return children().at( row );
}

ModelComponentItem * ModelComponentItem::child( const QString &key )
{
    if (m_children.keys().contains( key )) {
        return m_children.values( key ).back();
    }
    return 0;
}

void ModelComponentItem::setMetaData( const ComponentMetaData & data )
{
    m_metaData = data;
}

void ModelComponentItem::setFactory( IComponentItemFactory * factory )
{
    m_factory = factory;
}

int ModelComponentItem::row()
{
    if (m_parent) {
        return m_parent->children().indexOf(const_cast<ModelComponentItem*>(this));
    }
    return 0;
}

int ModelComponentItem::rowCount()
{
    return m_children.values().count();
}

QList<ModelComponentItem*> ModelComponentItem::children()
{
    return m_children.values();
}

ComponentMetaData ModelComponentItem::metaData() const
{
    return m_metaData;
}

IComponentItemFactory * ModelComponentItem::factory() const
{
    return m_factory;
}

IComponentItemFactory* ModelComponentItem::factoryForName(QString name)
{
    foreach(ModelComponentItem* categories, children()){
        foreach(ModelComponentItem* child, categories->children()){
            if (child->metaData().name == name)
                return child->factory();
        }
    }
    return 0;
}

//
// END class ModelComponentItem
//

//
// BEGIN class ComponentModel
//
ComponentModel::ComponentModel()
{
    m_rootItem = new ModelComponentItem();
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

    ModelComponentItem *parentItem;
    if (!parent.isValid()) {
        parentItem = m_rootItem;
    } else {
        parentItem = static_cast<ModelComponentItem*>(parent.internalPointer());
    }

    ModelComponentItem *childItem = parentItem->child(row);
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

    ModelComponentItem *childItem = static_cast<ModelComponentItem*>(index.internalPointer());
    ModelComponentItem *parentItem = childItem->parent();

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
        ModelComponentItem *item = static_cast<ModelComponentItem*>(parent.internalPointer());
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

    ModelComponentItem *item = static_cast<ModelComponentItem*>(index.internalPointer());
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

    ModelComponentItem *item = static_cast<ModelComponentItem*>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        return QVariant(item->metaData().title);
    } else if (role == Qt::DecorationRole) {
        return QVariant(item->metaData().icon);
    }

    return QVariant();
}

QMimeData *ComponentModel::mimeData( const QModelIndexList & indexes ) const
{
    ComponentMimeData *componentData = 0;

    //we only want to drag one item at a time
    QModelIndex index;
    if ( indexes.size() == 1 ) {
        index = indexes.first();
    }
    if (index.isValid()) {
        ModelComponentItem *item = static_cast<ModelComponentItem*>(index.internalPointer());
        componentData = new ComponentMimeData( item->metaData().name, item->factory() );

        //register our mimeType
        componentData->setData( "ktechlab/x-icomponent", item->metaData().name );
    }
    return componentData;
}

QStringList ComponentModel::mimeTypes() const
{
    return QStringList()<<"ktechlab/x-icomponent";
}

void ComponentModel::insertComponentData( const ComponentMetaData & data, IComponentItemFactory * factory )
{
    ModelComponentItem *item = new ModelComponentItem();
    item->setMetaData( data );
    item->setFactory( factory );

    ModelComponentItem *parent = m_rootItem->child( data.category );
    if (!parent) {
        parent = new ModelComponentItem();
        /* Use fakeData to store entries for different categories
         * that can't be dragged but are needed to bring some
         * structure into the component selector widget
         */
        ComponentMetaData fakeData
        // TODO FIXME hack
#if KDE_ENABLED
            = {
                "",
                data.category,
                data.category,
                KIcon(),
                ""
            }
#endif
        ;

        parent->setMetaData( fakeData );
        //we are about to insert a new row, tell the model about it so views can be updated
        beginInsertRows( index( rowCount(), 0, QModelIndex() ), 0, 0 );
        m_rootItem->addChild( parent );
        endInsertRows();
#if KDE_ENABLED
        kDebug() << "added category: " << data.category << " at " << index( parent->row(), 0, QModelIndex() );
#else
        qDebug() << "added category: " << data.category << " at " << index( parent->row(), 0, QModelIndex() );
#endif
    }
    const QModelIndex parentIndex = index( parent->row(), 0, QModelIndex() );
    //we are about to insert a new row, tell the model about it so views can be updated
    beginInsertRows( parentIndex, rowCount( parentIndex ), 0 );
    parent->addChild( item );
    endInsertRows();
}

void ComponentModel::removeComponentData(const KTechLab::ComponentMetaData& data, IComponentItemFactory* factory)
{
    ModelComponentItem* parent = m_rootItem->child(data.category);
    if (!parent) {
#if KDE_ENABLED
        kWarning() << "Category not found: " << data.category << " - can't remove data";
#else
        qWarning() << "Category not found: " << data.category << " - can't remove data";
#endif
        return;
    }
    // this is an O(n) operation, may be it should be changed in the future
    foreach (ModelComponentItem* child, parent->children()) {
        if ( factory != child->factory() )
            continue;
        if ( child->metaData().name != data.name )
            continue;

        const QModelIndex parentIndex = index( parent->row(), 0, QModelIndex() );
        beginRemoveRows(parentIndex,child->row(),child->row());
        parent->removeChild(child);
        endRemoveRows();
        return;
    }
}

IComponentItemFactory* ComponentModel::factoryForComponent(const QString& name) const
{
    IComponentItemFactory* factory = m_rootItem->factoryForName(name);
    if (!factory) {
#if KDE_ENABLED
        kWarning() << "Factory not found for component: " << name;
#else
        qWarning() << "Factory not found for component: " << name;
#endif
        return 0;
    }
    return factory;
}

// vim: sw=4 sts=4 et tw=100
