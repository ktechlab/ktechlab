/* This file is part of KDevelop
Copyright 2006 Adam Treat <treat@kde.org>
Copyright 2007 Anreas Pakulat <apaku@gmx.de>

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

#ifndef KDEVPROJECTCONTROLLER_H
#define KDEVPROJECTCONTROLLER_H

#include <interfaces/iprojectcontroller.h>

#include "ktlshellexport.h"

class QModelIndex;

namespace KDevelop { class IProject; }

namespace KTechLab
{

class Core;
class UiController;

class KTLSHELL_EXPORT IProjectDialogProvider : public QObject
{
Q_OBJECT
public:
    IProjectDialogProvider();
    virtual ~IProjectDialogProvider();

public Q_SLOTS:
    virtual KUrl askProjectConfigLocation() = 0;
    virtual bool userWantsReopen() = 0;
};

class KTLSHELL_EXPORT ProjectController : public KDevelop::IProjectController
{
    Q_OBJECT
    friend class Core;
    friend class CorePrivate;
    friend class ProjectPreferences;

public:
    ProjectController( Core* core );
    virtual ~ProjectController();

    virtual KDevelop::IProject* projectAt( int ) const;
    virtual int projectCount() const;
    virtual QList<KDevelop::IProject*> projects() const;

    virtual KDevelop::ProjectModel* projectModel();
    virtual QItemSelectionModel* projectSelectionModel();
    virtual KDevelop::IProject* findProjectByName( const QString& name );
    KDevelop::IProject* findProjectForUrl( const KUrl& ) const;
    void addProject(KDevelop::IProject*);
//     IProject* currentProject() const;

    virtual bool isProjectNameUsed( const QString& name ) const;
    void setDialogProvider(IProjectDialogProvider*);

    KUrl projectsBaseDirectory() const;

public Q_SLOTS:
    virtual bool openProject( const KUrl &KDev4ProjectFile = KUrl() );
    virtual bool projectImportingFinished( KDevelop::IProject* );
    virtual bool closeProject( KDevelop::IProject* );
    virtual void closeAllProjects();
    virtual bool configureProject( KDevelop::IProject* );
//     void changeCurrentProject( ProjectBaseItem* );

protected:
    virtual void loadSettings( bool projectIsLoaded );
    virtual void saveSettings( bool projectIsLoaded );


private:
    //FIXME Do not load all of this just for the project being opened...
    //void legacyLoading();
    void setupActions();
    void cleanup();
    bool loadProjectPart();
    void initialize();

    // helper methods for closeProject()
    void unloadUnusedProjectPlugins(KDevelop::IProject* proj);
    void disableProjectCloseAction();
    void closeAllOpenedFiles(KDevelop::IProject* proj);
    void initializePluginCleanup(KDevelop::IProject* proj);

private:
    Q_PRIVATE_SLOT(d, void projectConfig( QObject* ) )
    Q_PRIVATE_SLOT(d, void unloadAllProjectPlugins() )
    Q_PRIVATE_SLOT(d, void notifyProjectConfigurationChanged() )
    Q_PRIVATE_SLOT(d, void updateActionStates( KDevelop::Context* ) )
    Q_PRIVATE_SLOT(d, void closeSelectedProjects() )
    Q_PRIVATE_SLOT(d, void openProjectConfig() )
    class ProjectControllerPrivate* const d;
    friend class ProjectControllerPrivate;
};

class ProjectDialogProvider : public IProjectDialogProvider
{
Q_OBJECT
public:
    ProjectDialogProvider(ProjectControllerPrivate* const p);
    virtual ~ProjectDialogProvider();
    ProjectControllerPrivate* const d;

public Q_SLOTS:
    virtual KUrl askProjectConfigLocation();
    virtual bool userWantsReopen();
};


}
#endif

