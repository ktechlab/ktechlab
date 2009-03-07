/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "componentmodel.h"

QModelIndex ComponentModel::index( int row, int column, const QModelIndex & parent ) const
{
    return parent;
}

QModelIndex ComponentModel::parent( const QModelIndex & index ) const
{
    return index;
}

int ComponentModel::rowCount( const QModelIndex & parent ) const
{
    return 0;
}

int ComponentModel::columnCount( const QModelIndex & parent ) const
{
    Q_UNUSED( parent )

    return 1;
}

QVariant ComponentModel::data( const QModelIndex & index, int role ) const
{
    return QVariant();
}

void ComponentModel::setComponentData( const KTechLab::ComponentMetaData & data, KTechLab::IComponentFactory * factory )
{
    // TODO: implement me
}

// vim: sw=4 sts=4 et tw=100
