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
#include "ktlfolderitem.h"

#include <project/projectmodel.h>
#include <KGenericFactory>
#include <KAboutData>
#include <KMessageBox>
#include <interfaces/iproject.h>
#include <KActionCollection>
#include <sublime/mainwindow.h>
#include <QFile>
#include <QXmlSimpleReader>

using namespace KDevelop;

class KDevelop::KTLProjectManagerPrivate
{
  public:
  /**
  * Find sub-project represented by @param item in @param document
  * @param document - the QDomDocument representing the project
  * @param item - the ProjectFolderItem representing the sub-project
  * @return a QDomElement representing @param item in @param document
  */
  QDomElement findElementInDocument( ProjectFolderItem *item )
  {
    QDomNodeList items = projectDomDocument.elementsByTagName("item");
    QDomElement child;
    for ( uint i=0;
          i < items.length() && child.attribute( "name", QString() ) != item->folderName();
          ++i )
    {
      child = items.item(i).toElement();
    }
    return child;
  }

  QDomDocument projectDomDocument;
};

K_PLUGIN_FACTORY(KTLProjectManagerFactory, registerPlugin<KTLProjectManager>(); )
K_EXPORT_PLUGIN(KTLProjectManagerFactory(KAboutData("ktlprojectmanager","ktlprojectmanager",ki18n("KTechLab Project Manager"), "0.1", ki18n("A plugin to support KTechLab project management"), KAboutData::License_GPL)))

KTLProjectManager::KTLProjectManager( QObject *parent, const QVariantList &args )
  : IPlugin( KTLProjectManagerFactory::componentData(), parent ), IProjectFileManager( ),
  d( new KTLProjectManagerPrivate() )
{
    KDEV_USE_EXTENSION_INTERFACE( IProjectFileManager )
}

KTLProjectManager::~KTLProjectManager()
{
}

ProjectFileItem* KTLProjectManager::addFile( const KUrl& folder, ProjectFolderItem* parent )
{
  if (!folder.isValid())
    return 0;

  ProjectFileItem *item = new ProjectFileItem( parent->project(), folder, parent );

  return item;
}

ProjectFolderItem* KTLProjectManager::addFolder( const KUrl& folder, ProjectFolderItem* parent )
{
  if (!folder.isValid())
    return 0;

  ProjectFolderItem *item = new ProjectFolderItem( parent->project(), folder, parent );

  return item;
}

IProjectFileManager::Features KTLProjectManager::features() const
{
  return Features( Files | Folders | Targets );
}

ProjectFolderItem* KTLProjectManager::import( IProject* project )
{
  ProjectFolderItem *rootItem = new KTLFolderItem( project, project->folder() );
  rootItem->setProjectRoot(true);

  KUrl projectFile = rootItem->project()->folder();
  projectFile.addPath(rootItem->project()->name()+".ktechlab");

  QFile file( projectFile.toLocalFile() );
  if ( !file.open( QIODevice::ReadOnly ) )
  {
    KMessageBox::sorry( 0l, i18n("Could not open %1 for reading").arg(projectFile.toLocalFile()) );
    return 0;
  }

  d->projectDomDocument = QDomDocument( "KTechlab" );
  QString errorMessage;
  if ( !d->projectDomDocument.setContent( &file, &errorMessage ) )
  {
    KMessageBox::sorry( 0l, i18n("Couldn't parse xml:\n%1").arg(errorMessage) );
    return 0;
  }

  return rootItem;
}

QList<ProjectFolderItem*> KTLProjectManager::parse( ProjectFolderItem *item )
{
  QList<ProjectFolderItem*> result;

  QDomElement root;
  if ( item->isProjectRoot() )
  {
    root = d->projectDomDocument.documentElement();
  } else
  {
    root = d->findElementInDocument( item );
  }

  QDomNode node = root.firstChild();
  while ( !node.isNull() )
  {
    QDomElement childElement = node.toElement();
    if ( !childElement.isNull() )
    {
      const QString tagName = childElement.tagName();
      if ( tagName == "item" )
      {
        QString type = childElement.attribute( "type", QString() );
        QString name = childElement.attribute( "name", QString() );
        KUrl itemUrl( item->project()->folder(), childElement.attribute( "url", QString() ) );
        if ( type == "File" )
        {
          ProjectFileItem *child = new ProjectFileItem(item->project(), itemUrl, item);
        } else if ( !type.isEmpty() )
        {
          itemUrl.addPath(name);
          KTLFolderItem *child = new KTLFolderItem(item->project(), itemUrl, item);
          result.append(child);
        }
      }
    }
    node = node.nextSibling();
  }

  return result;
}

bool KTLProjectManager::reload( ProjectFolderItem* item )
{
  return false;
}

bool KTLProjectManager::removeFile( ProjectFileItem* file )
{
  return file->project()->inProject( file->url() );
}

bool KTLProjectManager::removeFolder( ProjectFolderItem* folder )
{
  return folder->project()->inProject( folder->url() );
}

bool KTLProjectManager::renameFile( ProjectFileItem* oldFile, const KUrl& newFile )
{
  return false;
}

bool KTLProjectManager::renameFolder( ProjectFolderItem* oldFolder, const KUrl& newFolder )
{
  return false;
}

void KTLProjectManager::createActionsForMainWindow( Sublime::MainWindow* window, QString& xmlFile, KActionCollection& actions )
{
  IPlugin::createActionsForMainWindow( window, xmlFile, actions );
}

#include "ktlprojectmanager.moc"
