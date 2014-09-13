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

class IComponentItemFactory;
class ModelComponentItem;

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
    virtual void insertComponentData( const ComponentMetaData & data, IComponentItemFactory * factory );
    /**
     * Remove the ComponentMetaData and the corresponding factory from the model
     */
    void removeComponentData( const ComponentMetaData & data, IComponentItemFactory* factory);

    /**
     * Retrieve a KTechLab::IComponentFactory that is able to produce components with the
     * given \param name
     */
    virtual IComponentItemFactory* factoryForComponent( const QString & name ) const;

private:
    ModelComponentItem *m_rootItem;
};

}
#endif

