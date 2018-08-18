/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "docmanager.h"
#include "document.h"
#include "language.h"
#include "languagemanager.h"
#include "ktechlab.h"
#include "microselectwidget.h"
#include "programmerdlg.h"
#include "projectdlgs.h"
#include "projectmanager.h"
#include "recentfilesaction.h"

#include <kdebug.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kio/netaccess.h>
#include <klocalizedstring.h>
#include <kmessagebox.h> 
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kxmlguifactory.h>

#include <qdom.h>
// #include <q3popupmenu.h>
#include <qmenu.h>

#include <cassert>

#include <ktlconfig.h>

//BEGIN class LinkerOptions
LinkerOptions::LinkerOptions()
{
	m_hexFormat = HexFormat::inhx32;
	m_bOutputMapFile = false;
}


QDomElement LinkerOptions::toDomElement( QDomDocument & doc, const KUrl & baseURL ) const
{
	QDomElement node = doc.createElement("linker");
	
	node.setAttribute( "hex-format", hexFormatToString(hexFormat()) );
	node.setAttribute( "output-map-file", outputMapFile() );
	node.setAttribute( "library-dir", libraryDir() );
	node.setAttribute( "linker-script", linkerScript() );
	node.setAttribute( "other", linkerOther() );
	
	QStringList::const_iterator end = m_linkedInternal.end();
	for ( QStringList::const_iterator it = m_linkedInternal.begin(); it != end; ++it )
	{
		QDomElement child = doc.createElement("linked-internal");
		node.appendChild(child);
		child.setAttribute( "url", KUrl::relativeUrl( baseURL, *it ) );
	}
	
	end = m_linkedExternal.end();
	for ( QStringList::const_iterator it = m_linkedExternal.begin(); it != end; ++it )
	{
		QDomElement child = doc.createElement("linked-external");
		node.appendChild(child);
		child.setAttribute( "url", *it );
	}
	
	return node;
}


void LinkerOptions::domElementToLinkerOptions( const QDomElement & element, const KUrl & baseURL )
{
	setHexFormat( stringToHexFormat( element.attribute( "hex-format", QString::null ) ) );
	setOutputMapFile( element.attribute( "output-map-file", "0" ).toInt() );
	setLibraryDir( element.attribute( "library-dir", QString::null ) );
	setLinkerScript( element.attribute( "linker-script", QString::null ) );
	setLinkerOther( element.attribute( "other", QString::null ) );
	
	m_linkedInternal.clear();
	m_linkedExternal.clear();
	
	QDomNode node = element.firstChild();
	while ( !node.isNull() )
	{
		QDomElement childElement = node.toElement();
		if ( !childElement.isNull() )
		{
			const QString tagName = childElement.tagName();
			
			if ( tagName == "linked-internal" )
				m_linkedInternal << KUrl( baseURL, childElement.attribute( "url", QString::null ) ).url();
			
			else if ( tagName == "linked-external" )
				m_linkedExternal << childElement.attribute( "url", QString::null );
			
			else
				kError() << k_funcinfo << "Unrecognised element tag name: "<<tagName<<endl;
		}
		
		node = node.nextSibling();
	}
}


QString LinkerOptions::hexFormatToString( HexFormat::type hexFormat )
{
	switch ( hexFormat )
	{
		case HexFormat::inhx32:
			return "inhx32";
			
		case HexFormat::inhx8m:
			return "inhx8m";
			
		case HexFormat::inhx8s:
			return "inhx8s";
			
		case HexFormat::inhx16:
			return "inhx16";
	}
	
	// Default hex format is inhx32
	return "inhx32";
}


LinkerOptions::HexFormat::type LinkerOptions::stringToHexFormat( const QString & hexFormat )
{
	if ( hexFormat == "inhx8m" )
		return HexFormat::inhx8m;
	
	if ( hexFormat == "inhx8s" )
		return HexFormat::inhx8s;
	
	if ( hexFormat == "inhx16" )
		return HexFormat::inhx16;
	
	return HexFormat::inhx32;
}
//END class LinkerOptions



//BEGIN class ProcessingOptions
ProcessingOptions::ProcessingOptions()
{
	m_bUseParentMicroID = false;
	m_microID = "P16F84";
}


ProcessingOptions::~ProcessingOptions()
{
}


QDomElement ProcessingOptions::toDomElement( QDomDocument & doc, const KUrl & baseURL ) const
{
	QDomElement node = doc.createElement("processing");
	
	node.setAttribute( "output", KUrl::relativeUrl( baseURL, outputURL().url() ) );
	node.setAttribute( "micro", m_microID );
	
	return node;
}


void ProcessingOptions::domElementToProcessingOptions( const QDomElement & element, const KUrl & baseURL )
{
	setOutputURL( KUrl( baseURL, element.attribute( "output", QString::null ) ) );
	setMicroID( element.attribute("micro", QString::null ) );
}
//END class ProcessingOptions



//BEGIN class ProjectItem
ProjectItem::ProjectItem( ProjectItem * parent, Type type, ProjectManager * projectManager )
	: QObject()
{
	m_pParent = parent;
	m_pILVItem = 0l;
	m_pProjectManager = projectManager;
	m_type = type;
}


ProjectItem::~ProjectItem()
{
	m_children.remove( (ProjectItem*)0l );
	ProjectItemList::iterator end = m_children.end();
	for ( ProjectItemList::iterator it = m_children.begin(); it != end; ++it )
		(*it)->deleteLater();
	m_children.clear();
	
	delete m_pILVItem;
}


void ProjectItem::setILVItem( ILVItem * ilvItem )
{
	m_pILVItem = ilvItem;
	ilvItem->setExpanded(true);
	ilvItem->setText( 0, name() );
	ilvItem->setProjectItem(this);
	updateILVItemPixmap();
}


void ProjectItem::updateILVItemPixmap()
{
	if ( !m_pILVItem )
		return;
	
	switch ( type() )
	{
		case ProjectType:
		{
			// ?! - We shouldn't have an ilvitem for this.
			break;
		}
		
		case ProgramType:
		{
			QPixmap pm;
			pm.load( KStandardDirs:: locate( "appdata", "icons/project_program.png" ) );
			m_pILVItem->setIcon( 0, QIcon( pm ) );
			break;
		}
		
		case LibraryType:
		{
			QPixmap pm;
			pm.load( KStandardDirs:: locate( "appdata", "icons/project_library.png" ) );
			m_pILVItem->setIcon( 0, QIcon( pm ) );
			break;
		}
		
		case FileType:
		{
			KMimeType::Ptr m = KMimeType::findByPath( url().path() );
			//m_pILVItem->setPixmap( 0, m->pixmap( KIconLoader::Small ) );
            m_pILVItem->setIcon( 0, QIcon( KIconLoader::global()->loadMimeTypeIcon( m->iconName(), KIconLoader::Small ) ) );
			break;
		}
	}
}


void ProjectItem::addChild( ProjectItem * child )
{
	if ( !child || m_children.contains(child) )
		return;
	
	m_children << child;
	
	child->setILVItem( m_pILVItem ?
			new ILVItem( m_pILVItem, child->name() ) :
			new ILVItem( m_pProjectManager, name() ) );
	
	updateControlChildMicroIDs();
}


void ProjectItem::updateControlChildMicroIDs()
{
	bool control = false;
	switch ( type() )
	{
		case ProjectItem::ProjectType:
		case ProjectItem::LibraryType:
		case ProjectItem::ProgramType:
			control = !microID().isEmpty();
			break;
			
		case ProjectItem::FileType:
			control = true;
			break;
	}
	
	m_children.remove( (ProjectItem*)0l );
	ProjectItemList::iterator end = m_children.end();
	for ( ProjectItemList::iterator it = m_children.begin(); it != end; ++it )
		(*it)->setUseParentMicroID( control );
}


void ProjectItem::setObjectName( const QString & name )
{
	m_name = name;
	if (m_pILVItem)
		m_pILVItem->setText( 0, name );
}


void ProjectItem::setURL( const KUrl & url )
{
	m_url = url;
	
	if ( m_name.isEmpty() )
		setObjectName( url.fileName() );
	
	if ( type() != FileType )
	{
		// The output url *is* our url
		setOutputURL(url);
	}
	else if ( outputURL().isEmpty() )
	{
		// Try and guess what the output url should be...
		QString newExtension;
		
		switch ( outputType() )
		{
			case ProgramOutput:
				newExtension = ".hex";
				break;
				
			case ObjectOutput:
				newExtension = ".o";
				break;
				
			case LibraryOutput:
				newExtension = ".o";
				break;
				
			case UnknownOutput:
				break;
		}
		
		if ( !newExtension.isEmpty() )
		{
			const QString fileName = url.url();
			QString extension = fileName.right( fileName.length() - fileName.findRev('.') );
			setOutputURL( QString(fileName).replace( extension, newExtension ) );
		}
	}
	
	updateILVItemPixmap();
}


QString ProjectItem::microID() const
{
	if ( !m_bUseParentMicroID )
		return m_microID;
	
	return m_pParent ? m_pParent->microID() : QString::null;
}


void ProjectItem::setMicroID( const QString & id )
{
	ProcessingOptions::setMicroID(id);
	updateControlChildMicroIDs();
}


ProjectItem::OutputType ProjectItem::outputType() const
{
	if ( !m_pParent )
		return UnknownOutput;
	
	switch ( m_pParent->type() )
	{
		case ProjectItem::ProjectType:
		{
			// We're a top level build target, so look at our own type
			switch ( type() )
			{
				case ProjectItem::ProjectType:
					kWarning() << k_funcinfo << "Parent item and this item are both project items" << endl;
					return UnknownOutput;
						
				case ProjectItem::FileType:
				case ProjectItem::ProgramType:
					return ProgramOutput;
						
				case ProjectItem::LibraryType:
					return LibraryOutput;
			}
			return UnknownOutput;
		}
				
		case ProjectItem::FileType:
		{
			kWarning() << k_funcinfo << "Don't know how to handle parent item being a file" << endl;
			return UnknownOutput;
		}
				
		case ProjectItem::ProgramType:
		case ProjectItem::LibraryType:
			return ObjectOutput;
	}
	
	return UnknownOutput;
}


bool ProjectItem::build( ProcessOptionsList * pol )
{
	if ( !pol )
		return false;
	
	// Check to see that we aren't already in the ProcessOptionstList;
	ProcessOptionsList::iterator polEnd = pol->end();
	for ( ProcessOptionsList::iterator it = pol->begin(); it != polEnd; ++it )
	{
		if ( (*it).targetFile() == outputURL().path() )
			return true;
	}
	
	ProjectInfo * projectInfo = ProjectManager::self()->currentProject();
	assert(projectInfo);
	
	if ( outputURL().isEmpty() )
	{
		KMessageBox::sorry( 0l, i18n("Do not know how to build \"%1\" (output URL is empty).", name()) );
		return false;
	}
	
	// Build all internal libraries that we depend on
	QStringList::iterator send = m_linkedInternal.end();
	for ( QStringList::iterator it = m_linkedInternal.begin(); it != send; ++it )
	{
		ProjectItem * lib = projectInfo->findItem( projectInfo->directory() + *it );
		if ( !lib )
		{
			KMessageBox::sorry( 0l, i18n("Do not know how to build \"%1\" (library does not exist in project).", *it) );
			return false;
		}
		
		if ( !lib->build(pol) )
			return false;
	}
	
	
	// Build all children
	m_children.remove( (ProjectItem*)0l );
	ProjectItemList::iterator cend = m_children.end();
	for ( ProjectItemList::iterator it = m_children.begin(); it != cend; ++it )
	{
		if ( ! (*it)->build(pol) )
			return false;
	}
	
	
	// Now build ourself
	ProcessOptions po;
	po.b_addToProject = false;
	po.setTargetFile( outputURL().path() );
	po.m_picID = microID();
	
	ProcessOptions::ProcessPath::MediaType typeTo = ProcessOptions::ProcessPath::Unknown;
	
	switch ( outputType() )
	{
		case UnknownOutput:
			KMessageBox::sorry( 0l, i18n("Do not know how to build \"%1\" (unknown output type).", name()) );
			return false;
			
		case ProgramOutput:
			typeTo = ProcessOptions::ProcessPath::Program;
			break;
			
		case ObjectOutput:
			typeTo = ProcessOptions::ProcessPath::Object;
			break;
			
		case LibraryOutput:
			typeTo = ProcessOptions::ProcessPath::Library;
			break;
	}
	
	switch ( type() )
	{
		case ProjectType:
			// Nothing to do
			return true;
			
		case FileType:
			po.setInputFiles( QStringList( url().path() ) );
			po.setProcessPath( ProcessOptions::ProcessPath::path( ProcessOptions::guessMediaType( url().url() ), typeTo ) );
			break;
			
		case ProgramType:
		case LibraryType:
			// Build up a list of input urls
			QStringList inputFiles;
			
			// Link child objects
			m_children.remove( (ProjectItem*)0l );
			ProjectItemList::iterator cend = m_children.end();
			for ( ProjectItemList::iterator it = m_children.begin(); it != cend; ++it )
				inputFiles << (*it)->outputURL().path();
			
			po.setInputFiles(inputFiles);
			po.setProcessPath( ProcessOptions::ProcessPath::path( ProcessOptions::ProcessPath::Object, typeTo ) );
			break;
	}
	
	po.m_hexFormat = hexFormatToString( hexFormat() );
	po.m_bOutputMapFile = outputMapFile();
	po.m_libraryDir = libraryDir();
	po.m_linkerScript = linkerScript();
	po.m_linkOther = linkerOther();
	
	// Link against libraries	
	QStringList::iterator lend = m_linkedInternal.end();
	for ( QStringList::iterator it = m_linkedInternal.begin(); it != lend; ++it )
		po.m_linkLibraries += projectInfo->directory() + *it;
	lend = m_linkedExternal.end();
	for ( QStringList::iterator it = m_linkedExternal.begin(); it != lend; ++it )
		po.m_linkLibraries += *it;
	
	// Save our working file (if open) and append to the build list
	Document * currentDoc = DocManager::self()->findDocument( url() );
	if (currentDoc)
		currentDoc->fileSave();
	pol->append(po);
	
	return true;
}


void ProjectItem::upload( ProcessOptionsList * pol )
{
	build( pol );
	
	ProgrammerDlg * dlg = new ProgrammerDlg( microID(), (QWidget*)KTechlab::self(), "Programmer Dlg" );
	
	dlg->exec();
	if ( !dlg->isAccepted() )
	{
		dlg->deleteLater();
		return;
	}
	
	ProcessOptions po;
	dlg->initOptions( & po );
	po.b_addToProject = false;
	po.setInputFiles( QStringList( outputURL().path() ) );
	po.setProcessPath( ProcessOptions::ProcessPath::Program_PIC );
	
	pol->append( po );
	
	dlg->deleteLater();
}


QDomElement ProjectItem::toDomElement( QDomDocument & doc, const KUrl & baseURL ) const
{
	QDomElement node = doc.createElement("item");
	
	node.setAttribute( "type", typeToString() );
	node.setAttribute( "name", m_name );
	node.setAttribute( "url", KUrl::relativeUrl( baseURL, m_url.url() ) );
	
	node.appendChild( LinkerOptions::toDomElement( doc, baseURL ) );
	node.appendChild( ProcessingOptions::toDomElement( doc, baseURL ) );
	
	
	ProjectItemList::const_iterator end = m_children.end();
	for ( ProjectItemList::const_iterator it = m_children.begin(); it != end; ++it )
	{
		if (*it)
			node.appendChild( (*it)->toDomElement( doc, baseURL ) );
	}
	
	return node;
}


KUrl::List ProjectItem::childOutputURLs( unsigned types, unsigned outputTypes ) const
{
	KUrl::List urls;
	
	ProjectItemList::const_iterator end = m_children.end();
	for ( ProjectItemList::const_iterator it = m_children.begin(); it != end; ++it )
	{
		if (!*it)
			continue;
		
		if ( ((*it)->type() & types) && ((*it)->outputType() & outputTypes) )
			urls += (*it)->outputURL().prettyUrl();
		
		urls += (*it)->childOutputURLs(types);
	}
	
	return urls;
}


ProjectItem * ProjectItem::findItem( const KUrl & url )
{
	if ( this->url() == url )
		return this;
	
	ProjectItemList::const_iterator end = m_children.end();
	for ( ProjectItemList::const_iterator it = m_children.begin(); it != end; ++it )
	{
		if (!*it)
			continue;
		
		ProjectItem * found = (*it)->findItem(url);
		if (found)
			return found;
	}
	
	return 0l;
}


bool ProjectItem::closeOpenFiles()
{
	Document * doc = DocManager::self()->findDocument(m_url);
	if ( doc && !doc->fileClose() )
		return false;
	
	m_children.remove( (ProjectItem*)0l );
	ProjectItemList::iterator end = m_children.end();
	for ( ProjectItemList::iterator it = m_children.begin(); it != end; ++it )
	{
		if ( !(*it)->closeOpenFiles() )
			return false;
	}
	
	return true;
}


void ProjectItem::addFiles()
{
	KUrl::List urls = KTechlab::self()->getFileURLs();
	const KUrl::List::iterator end = urls.end();
	for ( KUrl::List::iterator it = urls.begin(); it != end; ++ it)
		addFile(*it);
}


void ProjectItem::addCurrentFile()
{
	Document *document = DocManager::self()->getFocusedDocument();
	if (!document)
		return;
	
	// If the file isn't saved yet, we must do that
	// before it is added to the project.
	if( document->url().isEmpty() )
	{
		document->fileSaveAs();
		// If the user pressed cancel then just give up,
		// otherwise the file can now be added.
	}
	
	if( !document->url().isEmpty() )
		addFile( document->url() );
}


void ProjectItem::addFile( const KUrl & url )
{
	if ( url.isEmpty() )
		return;
	
	m_children.remove( (ProjectItem*)0l );
	ProjectItemList::iterator end = m_children.end();
	for ( ProjectItemList::iterator it = m_children.begin(); it != end; ++it )
	{
		if ( (*it)->type() == FileType && (*it)->url() == url )
			return;
	}
	
	ProjectItem * item = new ProjectItem( this, FileType, m_pProjectManager );
	item->setURL(url);
	addChild(item);
}


QString ProjectItem::typeToString() const
{
	switch (m_type)
	{
		case ProjectType:
			return "Project";
			
		case FileType:
			return "File";
			
		case ProgramType:
			return "Program";
			
		case LibraryType:
			return "Library";
	}
	return QString::null;
}


ProjectItem::Type ProjectItem::stringToType( const QString & type )
{
	if ( type == "Project" )
		return ProjectType;
	
	if ( type == "File" )
		return FileType;
	
	if ( type == "Program" )
		return ProgramType;
	
	if ( type == "Library" )
		return LibraryType;
	
	return FileType;
}


void ProjectItem::domElementToItem( const QDomElement & element, const KUrl & baseURL )
{
	Type type = stringToType( element.attribute( "type", QString::null ) );
	QString name = element.attribute( "name", QString::null );
	KUrl url( baseURL, element.attribute( "url", QString::null ) );
	
	ProjectItem * createdItem = new ProjectItem( this, type, m_pProjectManager );
	createdItem->setObjectName( name );
	createdItem->setURL( url );
	
	addChild( createdItem );
	
	QDomNode node = element.firstChild();
	while ( !node.isNull() )
	{
		QDomElement childElement = node.toElement();
		if ( !childElement.isNull() )
		{
			const QString tagName = childElement.tagName();
			
			if ( tagName == "linker" )
				createdItem->domElementToLinkerOptions( childElement, baseURL );
			
			else if ( tagName == "processing" )
				createdItem->domElementToProcessingOptions( childElement, baseURL );
			
			else if ( tagName == "item" )
				createdItem->domElementToItem( childElement, baseURL );
			
			else
				kError() << k_funcinfo << "Unrecognised element tag name: "<<tagName<<endl;
		}
		
		node = node.nextSibling();
	}
}
//END class ProjectItem



//BEGIN class ProjectInfo
ProjectInfo::ProjectInfo( ProjectManager * projectManager )
	: ProjectItem( 0l, ProjectItem::ProjectType, projectManager )
{
	m_microID = QString::null;
}


ProjectInfo::~ ProjectInfo()
{
}


bool ProjectInfo::open( const KUrl & url )
{
	QString target;
	if ( !KIO::NetAccess::download( url, target, 0l ) )
	{
		// If the file could not be downloaded, for example does not
		// exist on disk, NetAccess will tell us what error to use
		KMessageBox::error( 0l, KIO::NetAccess::lastErrorString() );
		
		return false;
	}
	
	QFile file(target);
	if ( !file.open( QIODevice::ReadOnly ) )
	{
		KMessageBox::sorry( 0l, i18n("Could not open %1 for reading", target) );
		return false;
	}
	
	m_url = url;
	
	QString xml;
	QTextStream textStream( &file );
	while ( !textStream.atEnd() ) //was: eof()
		xml += textStream.readLine() + '\n';
	
	file.close();
	
	QDomDocument doc( "KTechlab" );
	QString errorMessage;
	if ( !doc.setContent( xml, &errorMessage ) )
	{
		KMessageBox::sorry( 0l, i18n("Could not parse XML:\n%1", errorMessage) );
		return false;
	}
	
	QDomElement root = doc.documentElement();
	
	QDomNode node = root.firstChild();
	while ( !node.isNull() )
	{
		QDomElement element = node.toElement();
		if ( !element.isNull() )
		{
			const QString tagName = element.tagName();
			
			if ( tagName == "linker" )
				domElementToLinkerOptions( element, m_url );
			
			else if ( tagName == "processing" )
				domElementToProcessingOptions( element, m_url );
			
			else if ( tagName == "file" || tagName == "item" )
				domElementToItem( element, m_url );
			
			else
				kWarning() << k_funcinfo << "Unrecognised element tag name: "<<tagName<<endl;
		}
		
		node = node.nextSibling();
	}
	
	updateControlChildMicroIDs();
	return true;
}


bool ProjectInfo::save()
{
	QFile file( m_url.path() );
	if ( file.open(QIODevice::WriteOnly) == false )
	{
		KMessageBox::sorry( NULL, i18n("Project could not be saved to \"%1\"", m_url.path()), i18n("Saving Project") );
		return false;
	}
	
	QDomDocument doc("KTechlab");
	
	QDomElement root = doc.createElement("project");
	doc.appendChild(root);
	
	m_children.remove( (ProjectItem*)0l );
	ProjectItemList::const_iterator end = m_children.end();
	for ( ProjectItemList::const_iterator it = m_children.begin(); it != end; ++it )
		root.appendChild( (*it)->toDomElement( doc, m_url ) );
	
	QTextStream stream(&file);
	stream << doc.toString();
	file.close();
	
    {
        QAction *recentfilesaction = KTechlab::self()->actionByName("project_open_recent");
        if (recentfilesaction) {
            (static_cast<RecentFilesAction*>(recentfilesaction))->addURL(m_url);
        } else {
            qWarning() << "there is no project_open_recent action in KTechLab!";
        }
    }
	
	return true;
}


bool ProjectInfo::saveAndClose()
{
	if (!save())
		return false;
	
	if (!closeOpenFiles())
		return false;
	
	return true;
}
//END class ProjectInfo



//BEGIN class ProjectManager
ProjectManager * ProjectManager::m_pSelf = 0l;

ProjectManager * ProjectManager::self( KateMDI::ToolView * parent )
{
	if ( !m_pSelf )
	{
		assert(parent);
		m_pSelf = new ProjectManager( parent );
	}
	return m_pSelf;
}


ProjectManager::ProjectManager( KateMDI::ToolView * parent )
	: ItemSelector( parent, "Project Manager" ),
	m_pCurrentProject(0l)
{
	setWhatsThis( i18n("Displays the list of files in the project.\nTo open or close a project, use the \"Project\" menu. Right click on a file to remove it from the project") );
	
	setListCaption( i18n("File") );
	setCaption( i18n("Project Manager") );
	
	connect( this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slotItemClicked(QTreeWidgetItem*,int)) );
}


ProjectManager::~ProjectManager()
{
}


void ProjectManager::slotNewProject()
{
	if ( !slotCloseProject() )
		return;
	
	NewProjectDlg *newProjectDlg = new NewProjectDlg(this);
	newProjectDlg->exec();

	if ( newProjectDlg->accepted() )
	{
		m_pCurrentProject = new ProjectInfo( this );
		m_pCurrentProject->setObjectName( newProjectDlg->projectName() );
		m_pCurrentProject->setURL( newProjectDlg->location() + m_pCurrentProject->name().lower() + ".ktechlab" );
		
        QDir dir;
        if ( !dir.mkdir( m_pCurrentProject->directory() ) )
			kDebug() << "Error in creating directory " << m_pCurrentProject->directory() << endl;
		
		m_pCurrentProject->save();
		updateActions();
		
		emit projectCreated();
	}
	
	delete newProjectDlg;
}


void ProjectManager::slotProjectOptions()
{
}


void ProjectManager::slotOpenProject()
{
	QString filter;
	filter = QString("*.ktechlab|%1 (*.ktechlab)\n*|%2").arg( i18n("KTechlab Project") ).arg( i18n("All Files") );
	
    KUrl url = KFileDialog::getOpenUrl( KUrl(), filter, this, i18n("Open Location"));
	
    if ( url.isEmpty() )
		return;
	
	slotOpenProject(url);
}


void ProjectManager::slotOpenProject( const KUrl & url )
{
	if ( m_pCurrentProject && m_pCurrentProject->url() == url )
		return;
	
	if ( !slotCloseProject() )
		return;
	
	m_pCurrentProject = new ProjectInfo( this );
	
	if ( !m_pCurrentProject->open(url) )
	{
		m_pCurrentProject->deleteLater();
		m_pCurrentProject = 0l;
		return;
	}
	{
        RecentFilesAction * rfa = static_cast<RecentFilesAction*>(KTechlab::self()->actionByName("project_open_recent"));
        if (rfa) {
            rfa->addURL( m_pCurrentProject->url() );
        } else {
            qWarning() << "there is no project_open_recent action in application";
        }
    }
	
	if ( KTLConfig::raiseItemSelectors() )
		KTechlab::self()->showToolView( KTechlab::self()->toolView( toolViewIdentifier() ) );
	
	updateActions();
	emit projectOpened();
}


bool ProjectManager::slotCloseProject()
{
	if ( !m_pCurrentProject )
		return true;
	
	if ( !m_pCurrentProject->saveAndClose() )
		return false;
	
	m_pCurrentProject->deleteLater();
	m_pCurrentProject = 0l;
	updateActions();
	emit projectClosed();
	return true;
}


void ProjectManager::slotCreateSubproject()
{
	if ( !currentProject() )
		return;
	
	CreateSubprojectDlg * dlg = new CreateSubprojectDlg(this);
	dlg->exec();
	
	if ( dlg->accepted() )
	{
		ProjectItem::Type type = ProjectItem::ProgramType;
		switch ( dlg->type() )
		{
			case CreateSubprojectDlg::ProgramType:
				type = ProjectItem::ProgramType;
				break;
				
			case CreateSubprojectDlg::LibraryType:
				type = ProjectItem::LibraryType;
				break;
		}
		
		ProjectItem * subproject = new ProjectItem( currentProject(), type, this );
		subproject->setURL( dlg->targetFile() );
		
		currentProject()->addChild(subproject);
		currentProject()->save();
		
		emit subprojectCreated();
	}
	
	delete dlg;
}


void ProjectManager::updateActions()
{
	bool projectIsOpen = m_pCurrentProject;
	
	KTechlab::self()->actionByName("project_create_subproject")->setEnabled( projectIsOpen );
	KTechlab::self()->actionByName("project_export_makefile")->setEnabled( projectIsOpen );
	KTechlab::self()->actionByName("subproject_add_existing_file")->setEnabled( projectIsOpen );
	KTechlab::self()->actionByName("subproject_add_current_file")->setEnabled( projectIsOpen );
// 	KTechlab::self()->actionByName("project_options")->setEnabled( projectIsOpen );
	KTechlab::self()->actionByName("project_close")->setEnabled( projectIsOpen );
	KTechlab::self()->actionByName("project_add_existing_file")->setEnabled( projectIsOpen );
	KTechlab::self()->actionByName("project_add_current_file")->setEnabled( projectIsOpen );
}


void ProjectManager::slotAddFile()
{
	if ( !currentProject() )
		return;
	
	currentProject()->addFiles();
	emit filesAdded();
}


void ProjectManager::slotAddCurrentFile()
{
	if ( !currentProject() )
		return;
	currentProject()->addCurrentFile();
	emit filesAdded();
}


void ProjectManager::slotSubprojectAddExistingFile()
{
	ILVItem * currentItem = dynamic_cast<ILVItem*>(selectedItem());
	if ( !currentItem || !currentItem->projectItem() )
		return;
	
	currentItem->projectItem()->addFiles();
	emit filesAdded();
}


void ProjectManager::slotSubprojectAddCurrentFile()
{
	ILVItem * currentItem = dynamic_cast<ILVItem*>(selectedItem());
	if ( !currentItem || !currentItem->projectItem() )
		return;
	
	currentItem->projectItem()->addCurrentFile();
	emit filesAdded();
}


void ProjectManager::slotItemBuild()
{
	ILVItem * currentItem = dynamic_cast<ILVItem*>(selectedItem());
	if ( !currentItem || !currentItem->projectItem() )
		return;
	
	ProcessOptionsList pol;
	currentItem->projectItem()->build(&pol);
	LanguageManager::self()->compile(pol);
}


void ProjectManager::slotItemUpload()
{
	ILVItem * currentItem = dynamic_cast<ILVItem*>(selectedItem());
	if ( !currentItem || !currentItem->projectItem() )
		return;
	
	ProcessOptionsList pol;
	currentItem->projectItem()->upload(&pol);
	LanguageManager::self()->compile(pol);
}


void ProjectManager::slotRemoveSelected()
{
	ILVItem *currentItem = dynamic_cast<ILVItem*>(selectedItem());
	if ( !currentItem )
		return;
	
	int choice = KMessageBox::questionYesNo( this, i18n("Do you really want to remove \"%1\"?", currentItem->text(0) ), i18n("Remove Project File?"), KGuiItem(i18n("Remove")), KGuiItem(i18n("Cancel")) );
	
	if ( choice == KMessageBox::No )
		return;
	
	currentItem->projectItem()->deleteLater();
	emit filesRemoved();
}


void ProjectManager::slotExportToMakefile()
{
}


void ProjectManager::slotSubprojectLinkerOptions()
{
	ILVItem * currentItem = dynamic_cast<ILVItem*>(selectedItem());
	if ( !currentItem || !currentItem->projectItem() )
		return;
	
	LinkerOptionsDlg * dlg = new LinkerOptionsDlg( currentItem->projectItem(), this );
	dlg->exec();
	currentProject()->save();
	
	// The dialog sets the options for us if it was accepted, so we don't need to do anything
	delete dlg;
}


void ProjectManager::slotItemProcessingOptions()
{
	ILVItem * currentItem = dynamic_cast<ILVItem*>(selectedItem());
	if ( !currentItem || !currentItem->projectItem() )
		return;
	
	ProcessingOptionsDlg * dlg = new ProcessingOptionsDlg( currentItem->projectItem(), this );
	dlg->exec();
	currentProject()->save();
	
	// The dialog sets the options for us if it was accepted, so we don't need to do anything
	delete dlg;
}


void ProjectManager::slotItemClicked( QTreeWidgetItem* item, int )
{
	ILVItem * ilvItem = dynamic_cast<ILVItem*>(item);
	if ( !ilvItem )
		return;
	
	ProjectItem * projectItem = ilvItem->projectItem();
	if ( !projectItem || projectItem->type() != ProjectItem::FileType )
		return;
	
	DocManager::self()->openURL( projectItem->url() );
}


void ProjectManager::slotContextMenuRequested( const QPoint& pos )
{
    QTreeWidgetItem* item = itemAt(pos);
	QString popupName;
	ILVItem * ilvItem = dynamic_cast<ILVItem*>(item);
	QAction * linkerOptionsAct = KTechlab::self()->actionByName("project_item_linker_options");
	linkerOptionsAct->setEnabled(false);
	
	if ( !m_pCurrentProject ) {
		popupName = "project_none_popup";
	
    } else if ( !ilvItem ) {
		popupName = "project_blank_popup";
	
    } else
	{
		ProcessOptions::ProcessPath::MediaType mediaType = ProcessOptions::guessMediaType( ilvItem->projectItem()->url().url() );
		
		switch ( ilvItem->projectItem()->type() )
		{
			case ProjectItem::FileType:
				if ( mediaType == ProcessOptions::ProcessPath::Unknown )
					popupName = "project_file_other_popup";
				else
					popupName = "project_file_popup";
				break;
			
			case ProjectItem::ProgramType:
				popupName = "project_program_popup";
				break;
			
			case ProjectItem::LibraryType:
				popupName = "project_library_popup";
				break;
			
			case ProjectItem::ProjectType:
				return;
		}
		switch ( ilvItem->projectItem()->outputType() )
		{
			case ProjectItem::ProgramOutput:
				linkerOptionsAct->setEnabled(true);
				break;
				
			case ProjectItem::ObjectOutput:
			case ProjectItem::LibraryOutput:
			case ProjectItem::UnknownOutput:
				linkerOptionsAct->setEnabled(false);
				break;
		}
		
		// Only have linking options for SDCC files
		linkerOptionsAct->setEnabled( mediaType == ProcessOptions::ProcessPath::C );
	}
	
	bool haveFocusedDocument = DocManager::self()->getFocusedDocument();
	KTechlab::self()->actionByName("subproject_add_current_file")->setEnabled( haveFocusedDocument );
	KTechlab::self()->actionByName("project_add_current_file")->setEnabled( haveFocusedDocument );
	
	QMenu *pop = static_cast<QMenu*>(KTechlab::self()->factory()->container( popupName, KTechlab::self() ));
	if (pop) {
        QPoint globalPos = mapToGlobal(pos);
		pop->popup(globalPos);
    }
}
//END class ProjectManager

#include "projectmanager.moc"
