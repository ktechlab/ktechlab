/* This file is part of KDevelop
Copyright 2007 Andreas Pakulat <apaku@gmx.de>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#ifndef ENVIRONMENTGROUPMODEL_H
#define ENVIRONMENTGROUPMODEL_H

#include <QtCore/QAbstractTableModel>
#include "util/environmentgrouplist.h"

class QVariant;
class QModelIndex;

namespace KDevelop
{


class EnvironmentGroupModel : public QAbstractTableModel, public EnvironmentGroupList
{
    Q_OBJECT
public:
    EnvironmentGroupModel();
    int rowCount( const QModelIndex& ) const;
    int columnCount( const QModelIndex& ) const;
    Qt::ItemFlags flags( const QModelIndex& idx ) const;
    QVariant data( const QModelIndex& idx, int role ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    bool setData( const QModelIndex& idx, const QVariant&, int role );
    void setCurrentGroup( const QString& group );
    void loadFromConfig( KConfig* );
    void saveToConfig( KConfig* );
    void addVariable( const QString& var, const QString& value );
    void removeVariable( const QModelIndex& idx );
    void removeGroup( const QString& grp );
    void changeDefaultGroup( const QString& grp );

private:
    QMap<int, QString> m_variableMap;
    QString m_currentGroup;
};

}

#endif
