/* This file is part of KDevelop
Copyright 2008 Anreas Pakulat <apaku@gmx.de>

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

#ifndef SESSION_H
#define SESSION_H

#include "shellexport.h"
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <interfaces/isession.h>

namespace KDevelop
{

class KDEVPLATFORMSHELL_EXPORT Session : public ISession
{
    Q_OBJECT
public:
    static const QString cfgSessionNameEntry;
    Session( const QUuid& );
    virtual ~Session();

    virtual KUrl pluginDataArea( const IPlugin* );
    virtual KSharedConfig::Ptr config();

    void deleteFromDisk();

    virtual QString name() const;
    void setName( const QString& );
    QUuid id() const;
Q_SIGNALS:
    void nameChanged( const QString& newname, const QString& oldname );
private:
    class SessionPrivate* const d;

};

}
#endif

