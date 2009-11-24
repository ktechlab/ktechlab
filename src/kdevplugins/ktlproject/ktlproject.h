/*
    Represent a KTechLab project
    Copyright (C) 2009  Julian Bäume <julian@svg4all.de>

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

#ifndef KTLPROJECT_H
#define KTLPROJECT_H

#include <interfaces/iproject.h>

namespace KDevelop
{

/**
*
*/
class KTLProject: public IProject
{
  Q_OBJECT
  public:
    KTLProject ( QObject* parent = 0 );
    virtual ~KTLProject();

    /**
    *
    */
    virtual IBuildSystemManager* buildSystemManager() const;

    /**
    *
    */
    virtual ProjectFileItem* fileAt ( int pos ) const;

    /**
    *
    */
    virtual int fileCount() const;

    /**
    *
    */
    virtual QList< ProjectFileItem* > files() const;

    /**
    *
    */
    virtual QList< ProjectFileItem* > filesForUrl ( const KUrl& file ) const;

    /**
    *
    */
    virtual QList< ProjectFolderItem* > foldersForUrl ( const KUrl& folder ) const;

    /**
    *
    */
    virtual IPlugin* managerPlugin() const;

    /**
    *
    */
    virtual IProjectFileManager* projectFileManager() const;

    /**
    *
    */
    virtual KUrl projectFileUrl() const;

    /**
    *
    */
    virtual ProjectFolderItem* projectItem() const;

    /**
    *
    */
    virtual void reloadModel();

    /**
    *
    */
    virtual IPlugin* versionControlPlugin() const;

  public slots:
    /**
    *
    */
    virtual const KUrl folder() const;

    /**
    *
    */
    virtual bool inProject ( const KUrl& url ) const;

    /**
    *
    */
    virtual QString name() const;

    /**
    *
    */
    virtual KUrl relativeUrl ( const KUrl& absoluteUrl ) const;
};

}
#endif // KTLPROJECT_H
