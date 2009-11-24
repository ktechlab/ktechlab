/*
    Manage KTechLab project files.
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

#include "ktlprojectmanager.h"

#include <KGenericFactory>
#include <KAboutData>

using namespace KDevelop;

K_PLUGIN_FACTORY(KTLProjectManagerFactory, registerPlugin<KTLProjectManager>(); )
K_EXPORT_PLUGIN(KTLProjectManagerFactory(KAboutData("ktlprojectmanager","ktlprojectmanager",ki18n("KTechLab Project Manager"), "0.1", ki18n("A plugin to support KTechLab project management"), KAboutData::License_GPL)))

KTLProjectManager::KTLProjectManager( QObject *parent, const QVariantList &args )
  : IPlugin( KTLProjectManagerFactory::componentData(), parent ), IProjectFileManager( )
{
}

KTLProjectManager::~KTLProjectManager()
{
}

ProjectFileItem* KTLProjectManager::addFile( const KUrl& folder, ProjectFolderItem* parent )
{
}

ProjectFolderItem* KTLProjectManager::addFolder( const KUrl& folder, ProjectFolderItem* parent )
{
}

IProjectFileManager::Features KTLProjectManager::features() const
{
}

ProjectFolderItem* KTLProjectManager::import( IProject* project )
{
}

QList< ProjectFolderItem* > KTLProjectManager::parse( ProjectFolderItem* dom )
{
}

bool KTLProjectManager::reload( ProjectBaseItem* item )
{
}

bool KTLProjectManager::removeFile( ProjectFileItem* file )
{
}

bool KTLProjectManager::removeFolder( ProjectFolderItem* folder )
{
}

bool KTLProjectManager::renameFile( ProjectFileItem* oldFile, const KUrl& newFile )
{
}

bool KTLProjectManager::renameFolder( ProjectFolderItem* oldFolder, const KUrl& newFolder )
{
}

void KTLProjectManager::createActionsForMainWindow( Sublime::MainWindow* window, QString& xmlFile, KActionCollection& actions )
{
    IPlugin::createActionsForMainWindow( window, xmlFile, actions );
}

#include "ktlprojectmanager.moc"