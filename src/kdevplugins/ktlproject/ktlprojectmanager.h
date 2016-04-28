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
#if KDEV_PLUGIN_VERSION < 17
    virtual ProjectFileItem* addFile ( const KUrl& folder, ProjectFolderItem* parent );
#else
    virtual ProjectFileItem* addFile ( const Path& folder, ProjectFolderItem* parent );
#endif

    /**
    *
    */
#if KDEV_PLUGIN_VERSION < 17
    virtual ProjectFolderItem* addFolder ( const KUrl& folder, ProjectFolderItem* parent );
#else
    virtual ProjectFolderItem* addFolder ( const Path& folder, ProjectFolderItem* parent );
#endif

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

#if (KDEV_PLUGIN_VERSION >= 11) && (KDEV_PLUGIN_VERSION < 13)
    /**
     *
     */
    virtual bool removeFilesAndFolders(QList< ProjectBaseItem* > items);
#endif

#if (KDEV_PLUGIN_VERSION >= 13)
    /**
     *
     */
    virtual bool removeFilesAndFolders(const QList< ProjectBaseItem* > & items);
#endif

    /**
     *
     */ 
#if (KDEV_PLUGIN_VERSION < 13)
#else
    virtual bool moveFilesAndFolders(const QList<KDevelop::ProjectBaseItem*>&, KDevelop::ProjectFolderItem*);
#endif

#if KDEV_PLUGIN_VERSION < 17
    virtual bool copyFilesAndFolders(const KUrl::List &, KDevelop::ProjectFolderItem*);
#else
    virtual bool copyFilesAndFolders(const Path::List &, KDevelop::ProjectFolderItem*);
#endif
    
#if KDEV_PLUGIN_VERSION < 17
    /**
    *
    */
    virtual bool renameFile ( ProjectFileItem* oldFile, const KUrl& newFile );
#else
    virtual bool renameFile ( ProjectFileItem* oldFile, const Path& newFile );
#endif

    /**
    *
    */
#if KDEV_PLUGIN_VERSION < 17
    virtual bool renameFolder ( ProjectFolderItem* oldFolder, const KUrl& newFolder );
#else
    virtual bool renameFolder ( ProjectFolderItem* oldFolder, const Path& newFolder );
#endif

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
