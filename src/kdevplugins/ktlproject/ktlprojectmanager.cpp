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
#include <QStack>
#include <QXmlSimpleReader>

using namespace KDevelop;

class KDevelop::KTLProjectManagerPrivate
{
  public:
    /**
    * Find sub-project represented by @param item in the projects DOM document.
    * This will return the root-node (i.e. documentElement) if sub-project
    * is not found.
    * @param item - the ProjectFolderItem representing the sub-project
    * @param name - the value of the name attribute of the element to look for
    * @return a QDomElement representing @param item in the DOM document or the document element
    */
    QDomElement findElementInDocument( ProjectBaseItem *item, const QString &name )
    {
        QStack<QString> domPath;
        domPath.push( name );
        //TODO: can this be done without dynamic_cast?
        ProjectFolderItem *wItem = dynamic_cast<ProjectFolderItem*>( item->parent() );
        if (!wItem) {
            //this must be the project root
            return projectDomDocument.documentElement();
        }
        while (!wItem->isProjectRoot()){
            domPath.push( wItem->folderName() );
            //TODO: can this be done without dynamic_cast?
            wItem = dynamic_cast<ProjectFolderItem*>( wItem->parent() );
        };

        QDomElement child = projectDomDocument.documentElement();
        while ( !domPath.isEmpty() ){
            QString nextFolder = domPath.pop();
            QDomElement d = child.firstChildElement("item");
            while (!d.isNull() && d.attribute("name") != nextFolder){
                if (d.attribute("name") == ""){
                    kWarning() << "no, this shouldn't happen";
                    return QDomElement();
                }
                d = d.nextSiblingElement("item");
            }
            child = d;
        }
        return child;
    }
    void writeProjectToDisk()
    {
      QFile file( projectFile.toLocalFile() );
      if ( !file.open( QIODevice::ReadWrite ) )
      {
        KMessageBox::sorry( 0l, i18n("Could not open %1 for writing").arg(projectFile.toLocalFile()) );
        return;
      }
      file.resize(0);
      file.write( projectDomDocument.toByteArray() );
      file.close();
    }
    void removeItemFromDocument( ProjectBaseItem *item, const QString &name )
    {
        QDomElement child = findElementInDocument( item, name );
        child.parentNode().removeChild(child);
    }
    void updateItemFromDocument( ProjectBaseItem *item, const QString &name )
    {
        QDomElement child = findElementInDocument( item, name );
        KUrl folder;
        if (item->folder())
            folder = item->folder()->url();
        if (item->file())
            folder = item->file()->url();

        QString relativeFileName =
            KUrl::relativePath( projectFile.directory(), folder.directory() );
        relativeFileName.append(folder.fileName());
        QDomElement newNode = child.cloneNode().toElement();
        newNode.setAttribute("name",folder.fileName());
        newNode.setAttribute("url",relativeFileName);

        child.parentNode().replaceChild( newNode, child );
    }

    QDomDocument projectDomDocument;
    IProject *project;
    KUrl projectFile;
};

K_PLUGIN_FACTORY(KTLProjectManagerFactory, registerPlugin<KTLProjectManager>(); )
K_EXPORT_PLUGIN(KTLProjectManagerFactory(KAboutData("ktlprojectmanager","ktlprojectmanager",ki18n("KTechLab Project Manager"), "0.1", ki18n("A plugin to support KTechLab project management"), KAboutData::License_GPL)))

KTLProjectManager::KTLProjectManager( QObject *parent, const QVariantList &args )
  : IPlugin( KTLProjectManagerFactory::componentData(), parent ), IProjectFileManager( ),
  d( new KTLProjectManagerPrivate() )
{
    KDEV_USE_EXTENSION_INTERFACE( KDevelop::IProjectFileManager )
}

KTLProjectManager::~KTLProjectManager()
{
}

ProjectFileItem* KTLProjectManager::addFile( const KUrl& folder, ProjectFolderItem* parent )
{
  if (!folder.isValid())
    return 0;

  ProjectFileItem *item = new ProjectFileItem( parent->project(), folder, parent );
  QString relativeFileName =
    KUrl::relativePath( d->projectFile.directory(), folder.directory() );
  relativeFileName.append(folder.fileName());

  kDebug() << "adding file with relative url: " << relativeFileName;
  QDomElement itemElement = d->projectDomDocument.createElement("item");
  itemElement.setAttribute("url", relativeFileName );
  itemElement.setAttribute("name", item->fileName() );
  itemElement.setAttribute("type", "File");

  QDomElement folderElement = d->findElementInDocument( parent, parent->folderName() );
  folderElement.appendChild( itemElement );

  d->writeProjectToDisk();
  return item;
}

ProjectFolderItem* KTLProjectManager::addFolder( const KUrl& folder, ProjectFolderItem* parent )
{
  if (!folder.isValid())
    return 0;

  ProjectFolderItem *item = new ProjectFolderItem( parent->project(), folder, parent );
    //TODO: implement me
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

  d->projectFile = rootItem->project()->folder();
  d->projectFile.addPath(rootItem->project()->name()+".ktechlab");

  QFile file( d->projectFile.toLocalFile() );
  if ( !file.open( QIODevice::ReadOnly ) )
  {
    KMessageBox::sorry( 0l, i18n("Could not open %1 for reading").arg(d->projectFile.toLocalFile()) );
    return 0;
  }

  d->projectDomDocument = QDomDocument( "KTechlab" );
  QString errorMessage;
  if ( !d->projectDomDocument.setContent( &file, &errorMessage ) )
  {
    KMessageBox::sorry( 0l, i18n("Couldn't parse xml:\n%1").arg(errorMessage) );
    return 0;
  }
  file.close();
  return rootItem;
}

QList<ProjectFolderItem*> KTLProjectManager::parse( ProjectFolderItem *item )
{
  QList<ProjectFolderItem*> result;

  QDomElement root = d->findElementInDocument( item, item->folderName() );

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
          kDebug() << "Created file:" << child->fileName();
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
    ProjectFolderItem *folder = item->folder();
    if (!folder)
        return false;

    folder->removeRows( 0, folder->rowCount() );
    QList<ProjectFolderItem*> folders = parse(folder);
    //recursively reload subfolders
    foreach(ProjectFolderItem *f, folders){
        reload(f);
    }
    return true;
}

bool KTLProjectManager::removeFile( ProjectFileItem* file )
{
    //TODO: can this be done without dynamic_cast?
    ProjectFolderItem *parent = dynamic_cast<ProjectFolderItem*>(file->parent());
    if (!parent)
        return false;

    kDebug() << "removing:" << file->fileName();
    d->removeItemFromDocument( file, file->fileName() );
    parent->removeRow( file->row() );
    d->writeProjectToDisk();
    return true;
}

bool KTLProjectManager::removeFolder( ProjectFolderItem* folder )
{
    //remove all children, is this needed?
    foreach(ProjectFileItem *item, folder->fileList()){
        removeFile(item);
    }
    foreach(ProjectFolderItem *item, folder->folderList()){
        removeFolder(item);
    }

    d->removeItemFromDocument( folder, folder->folderName() );
    folder->parent()->removeRow( folder->row() );
    d->writeProjectToDisk();
    return true;
}

bool KTLProjectManager::renameFile( ProjectFileItem* oldFile, const KUrl& newFile )
{
    QString oldFileName = oldFile->fileName();
    oldFile->setUrl(newFile);
    d->updateItemFromDocument( oldFile, oldFileName );
    d->writeProjectToDisk();
    return true;
}

bool KTLProjectManager::renameFolder( ProjectFolderItem* oldFolder, const KUrl& newFolder )
{
    QString oldFolderName = oldFolder->folderName();
    oldFolder->setUrl(newFolder);
    d->updateItemFromDocument( oldFolder, oldFolderName );
    d->writeProjectToDisk();
    return true;
}

void KTLProjectManager::createActionsForMainWindow( Sublime::MainWindow* window, QString& xmlFile, KActionCollection& actions )
{
    //TODO: implement me
  IPlugin::createActionsForMainWindow( window, xmlFile, actions );
}

#include "ktlprojectmanager.moc"
