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

/**
 * This class implements QAbstractItemModel to provide a tree of compoments sorted into groups like
 * logical compoments, discrete components and so on. Different component libraries/plugins should add the
 * compoments they provide to this list to make them available to the user.
 */
class KTLINTERFACES_EXPORT ComponentModel: public QAbstractItemModel
{
Q_OBJECT
public:

    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent( const QModelIndex & index ) const;

    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    virtual void setComponentData( const KTechLab::ComponentMetaData & data, KTechLab::IComponentFactory * factory );

};

#endif

