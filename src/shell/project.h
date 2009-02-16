/* This file is part of the KDE project
   Copyright 2001 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
   Copyright 2001-2002 Bernd Gehrmann <bernd@kdevelop.org>
   Copyright 2002-2003 Roberto Raggi <roberto@kdevelop.org>
   Copyright 2002 Simon Hausmann <hausmann@kde.org>
   Copyright 2003 Jens Dagerbo <jens.dagerbo@swipnet.se>
   Copyright 2003 Mario Scalas <mario.scalas@libero.it>
   Copyright 2003-2004 Alexander Dymo <adymo@kdevelop.org>
   Copyright     2006 Matt Rogers <mattr@kde.org>
   Copyright     2007 Andreas Pakulat <apaku@gmx.de>

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
#ifndef KTLPROJECT_H
#define KTLPROJECT_H

#include <kurl.h>

#include <interfaces/iproject.h>
#include <interfaces/istatus.h>

#include "ktlshellexport.h"

template<typename T> class QList;

class KJob;

namespace KDevelop
{
class IProjectFileManager;
class IBuildSystemManager;
class ProjectFileItem;
class ProjectFolderItem;
} // namespace KDevelop

namespace KTechLab
{

class PersistentHash;

/**
 * \brief Object which represents a KDevelop project
 *
 * Provide better descriptions
 */
class KTLSHELL_EXPORT Project : public KDevelop::IProject
{
    Q_OBJECT
public:
    /**
     * Constructs a project.
     *
     * @param parent The parent object for the plugin.
     */
    Project(QObject *parent = 0);

    /// Destructor.
    virtual ~Project();

    /** Get a list of all files in the project */
    virtual int fileCount() const;
    virtual KDevelop::ProjectFileItem* fileAt( int i ) const;

    virtual QList<KDevelop::ProjectFileItem*> files() const;

    virtual QList<KDevelop::ProjectFileItem*> filesForUrl( const KUrl& ) const;
    virtual QList<KDevelop::ProjectFolderItem*> foldersForUrl(const KUrl& ) const;

    QString projectTempFile() const;
    QString developerTempFile() const;
    KUrl developerFileUrl() const;
    virtual void reloadModel();
    virtual KUrl projectFileUrl() const;
    virtual KSharedConfig::Ptr projectConfiguration() const;

    virtual void addToFileSet( const KDevelop::IndexedString& );
    virtual void removeFromFileSet( const KDevelop::IndexedString& );
    virtual QSet<KDevelop::IndexedString> fileSet() const;
public Q_SLOTS:
    /**
     * @brief Open a project
     * This method opens a project and starts the process of loading the
     * data for the project from disk.
     * @param projectFileUrl The url pointing to the location of the project
     * file to load
     * The project name is taken from the Name key in the project file in
     * the 'General' group
     */
    virtual Q_SCRIPTABLE bool open(const KUrl &projectFileUrl);

    /** This method is invoked when the project needs to be closed. */
    virtual Q_SCRIPTABLE void close();

    /**
     * @brief Get the project folder
     * @return The canonical absolute directory of the project.
     */
    virtual Q_SCRIPTABLE const KUrl folder() const;

    /** Returns the name of the project. */
    virtual Q_SCRIPTABLE QString name() const;

    /**
     * Get the file manager for the project
     *
     * @return the file manager for the project, if one exists; otherwise null
     */
    KDevelop::IProjectFileManager* projectFileManager() const;

    /**
     * Get the build system manager for the project
     *
     * @return the build system manager for the project, if one exists; otherwise null
     */
    KDevelop::IBuildSystemManager* buildSystemManager() const;

    KDevelop::IPlugin* versionControlPlugin() const;

    /**
     * Get the plugin that manages the project
     * This can be used to get other interfaces like IBuildSystemManager
     */
    KDevelop::IPlugin* managerPlugin() const;

    /**
     * Set the manager plugin for the project.
     */
    void setManagerPlugin( KDevelop::IPlugin* manager );

    /**
     * With this the top-level project item can be retrieved
     */
    KDevelop::ProjectFolderItem* projectItem() const;

    /**
     * Find the url relative to the project directory equivalent to @a absoluteUrl.
     * This function does not check to see if the file is contained within the
     * project; for that, use inProject().
     *
     * @param absoluteUrl Absolute url to convert
     * @deprecated use KUrl::relativeUrl instead
     * @returns absoluteUrl relative to projectDirectory()
     **/
    KUrl relativeUrl(const KUrl& absoluteUrl) const;

    /**
     * Returns the absolute url corresponding to the given \a relativeUrl and
     * the project directory.
     *
     * @param relativeUrl Relative url to convert
     *
     * @returns the absolute URL relative to projectDirectory()
     **/
    KUrl urlRelativeToProject(const KUrl& relativeUrl) const;

    /**
     * Check if the url specified by @a url is part of the project.
     * @a url can be either a relative url (to the project directory) or
     * an absolute url.
     *
     * @param url the url to check
     *
     * @return true if the url @a url is a part of the project.
     */
    bool inProject(const KUrl &url) const;

private:
    Q_PRIVATE_SLOT(d, void importDone(KJob*))
    Q_PRIVATE_SLOT(d, void reloadDone())

    class ProjectPrivate* const d;
};

} // namespace KTechLab
#endif
