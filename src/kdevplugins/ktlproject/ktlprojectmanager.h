/*
    Manage KTechLab project files.
    Copyright (C) 2009-2010 Julian Bäume <julian@svg4all.de>

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

#ifndef KTLPROJECTMANAGER_H
#define KTLPROJECTMANAGER_H

#include <interfaces/iplugin.h>
#include <project/interfaces/iprojectfilemanager.h>
#include <QVariantList>

namespace KDevelop
{

class KTLProjectManagerPrivate;

class KTLProjectManager : public IPlugin, public IProjectFileManager
{
  Q_OBJECT
  Q_INTERFACES(KDevelop::IProjectFileManager)
  public:
    explicit KTLProjectManager( QObject *parent = 0, const QVariantList &args = QVariantList() );
    ~KTLProjectManager();

    /**
    *
    */
    virtual ProjectFileItem* addFile ( const KUrl& folder, ProjectFolderItem* parent );

    /**
    *
    */
    virtual ProjectFolderItem* addFolder ( const KUrl& folder, ProjectFolderItem* parent );

    /**
    *
    */
    virtual Features features() const;

    /**
    *
    */
    virtual ProjectFolderItem* import ( IProject* project );

    /**
    *
    */
    virtual QList< ProjectFolderItem* > parse ( KDevelop::ProjectFolderItem* item );

    /**
    *
    */
    virtual bool reload ( ProjectBaseItem* item );

    /**
    *
    */
    virtual bool removeFile ( ProjectFileItem* file );

    /**
    *
    */
    virtual bool removeFolder ( ProjectFolderItem* folder );

    /**
    *
    */
    virtual bool renameFile ( ProjectFileItem* oldFile, const KUrl& newFile );

    /**
    *
    */
    virtual bool renameFolder ( ProjectFolderItem* oldFolder, const KUrl& newFolder );

    /**
    *
    */
     virtual bool reload(ProjectFolderItem* item);

    /**
    *
    */
    virtual void createActionsForMainWindow ( Sublime::MainWindow* window, QString& xmlFile, KActionCollection& actions );

  private:
    KTLProjectManagerPrivate *d;
};

}

#endif // KTLPROJECTMANAGER_H
