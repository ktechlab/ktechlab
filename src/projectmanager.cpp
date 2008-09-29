/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton <david@bluehaze.org>          *
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "core/ktlconfig.h"
#include "docmanager.h"
#include "document.h"
//#include "language.h"
//#include "languagemanager.h"
#include "ktechlab.h"
//#include "microselectwidget.h"
//#include "programmerdlg.h"
//#include "projectdlgs.h"
#include "projectmanager.h"
//#include "recentfilesaction.h"

#include <KDebug>
#include <KFileDialog>
#include <KIconLoader>
#include <KIO/NetAccess>
#include <KLocale>
#include <KMessageBox>
#include <KMimeType>
#include <KStandardDirs>
#include <QDomDocument>
#include <QDomElement>
#include <QMenu>
#include <QWhatsThis>

#include <cassert>

ILVItem::ILVItem( QTreeWidget* parent, const QString &id )
: QTreeWidgetItem( parent, 0 )
{
    m_id = id;
    b_isRemovable = false;
    m_pProjectItem = 0l;
}

ILVItem::ILVItem( QTreeWidgetItem* parent, const QString &id )
: QTreeWidgetItem( parent, 0 )
{
    m_id = id;
    b_isRemovable = false;
    m_pProjectItem = 0l;
}

//BEGIN class LinkerOptions
LinkerOptions::LinkerOptions()
    : Options()
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


bool LinkerOptions::fromDomElement( const QDomElement & element, const KUrl & baseURL )
{
    setHexFormat( stringToHexFormat( element.attribute( "hex-format", QString() ) ) );
    setOutputMapFile( element.attribute( "output-map-file", "0" ).toInt() );
    setLibraryDir( element.attribute( "library-dir", QString() ) );
    setLinkerScript( element.attribute( "linker-script", QString() ) );
    setLinkerOther( element.attribute( "other", QString() ) );

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
                m_linkedInternal << KUrl( baseURL, childElement.attribute( "url", QString() ) ).url();

            else if ( tagName == "linked-external" )
                m_linkedExternal << childElement.attribute( "url", QString() );

            else
                kError() << k_funcinfo << "Unrecognised element tag name: "<<tagName<<endl;
        }

        node = node.nextSibling();
    }
    return true;
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
    : Options()
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


bool ProcessingOptions::fromDomElement( const QDomElement & element, const KUrl & baseURL )
{
    setOutputURL( KUrl( baseURL, element.attribute( "output", QString() ) ) );
    setMicroID( element.attribute("micro", QString() ) );

    return true;
}
//END class ProcessingOptions



//BEGIN class ProjectItem
ProjectItem::ProjectItem( ProjectItem * parent, ProjectManager * projectManager )
//    : QObject()
{
    m_pParent = parent;
    m_pILVItem = 0l;
    m_pProjectManager = projectManager;
    m_processingOptions = 0;
    m_linkerOptions = 0;
}


ProjectItem::~ProjectItem()
{
    m_children.removeAll( (ProjectItem*)0l );

    foreach ( ProjectItem * child, m_children)
        delete child;
    m_children.clear();

    delete m_pILVItem;
    m_pILVItem = 0l;
}


void ProjectItem::setILVItem( ILVItem * ilvItem )
{
    m_pILVItem = ilvItem;
    ilvItem->setExpanded(true);
    ilvItem->setText( 0, name() );
    ilvItem->setProjectItem(this);
    updateILVItemPixmap();
}


void ProjectItem::addChild( ProjectItem * child )
{
    if ( !child || m_children.contains(child) )
        return;

    m_children << child;

    child->setILVItem( m_pILVItem ?
            new ILVItem( m_pILVItem, child->name() ) :
            new ILVItem( m_pProjectManager->treeWidget(), name() ) );

    updateControlChildMicroIDs();
}


void ProjectItem::updateControlChildMicroIDs()
{
    bool control = shouldUpdateControlChildMicroIDs();

    m_children.removeAll( (ProjectItem*)0l );
    ProjectItemList::iterator end = m_children.end();
    for ( ProjectItemList::iterator it = m_children.begin(); it != end; ++it )
        if ( ProcessingOptions * p = (*it)->processingOptions() )
            p->setUseParentMicroID( control );
}


void ProjectItem::setName( const QString & name )
{
    m_name = name;
    if (m_pILVItem)
        m_pILVItem->setText( 0, name );
}


void ProjectItem::setURL( const KUrl & url )
{
    m_url = url;

    if ( m_name.isEmpty() )
        setName( url.fileName() );

    m_processingOptions->setOutputURL(url);
    updateILVItemPixmap();
}


QString ProjectItem::microID() const
{
    if ( !m_processingOptions->useParentMicroID() )
        return m_processingOptions->microID();

    return m_pParent ? m_pParent->microID() : QString();
}


void ProjectItem::setMicroID( const QString & id )
{
    m_processingOptions->setMicroID(id);
    updateControlChildMicroIDs();
}


// ProjectItem::OutputType ProjectItem::outputType() const
// {
// 	if ( !m_pParent )
// 		return UnknownOutput;
//
// 	switch ( m_pParent->type() )
// 	{
// 		case ProjectItem::ProjectType:
// 		{
// 			// We're a top level build target, so look at our own type
// 			switch ( type() )
// 			{
// 				case ProjectItem::ProjectType:
// 					kdWarning() << k_funcinfo << "Parent item and this item are both project items" << endl;
// 					return UnknownOutput;
//
// 				case ProjectItem::FileType:
// 				case ProjectItem::ProgramType:
// 					return ProgramOutput;
//
// 				case ProjectItem::LibraryType:
// 					return LibraryOutput;
// 			}
// 			return UnknownOutput;
// 		}
//
// 		case ProjectItem::FileType:
// 		{
// 			kdWarning() << k_funcinfo << "Don't know how to handle parent item being a file" << endl;
// 			return UnknownOutput;
// 		}
//
// 		case ProjectItem::ProgramType:
// 		case ProjectItem::LibraryType:
// 			return ObjectOutput;
// 	}
//
// 	return UnknownOutput;
// }


//FIXME: port build()
bool ProjectItem::build( ProcessOptionsList * pol )
{
    return true;
}
// bool ProjectItem::build( ProcessOptionsList * pol )
// {
// 	if ( !pol )
// 		return false;
//
// 	// Check to see that we aren't already in the ProcessOptionstList;
// 	ProcessOptionsList::iterator polEnd = pol->end();
// 	for ( ProcessOptionsList::iterator it = pol->begin(); it != polEnd; ++it )
// 	{
// 		if ( (*it).targetFile() == outputURL().path() )
// 			return true;
// 	}
//
// 	ProjectInfo * projectInfo = ProjectManager::self()->currentProject();
// 	assert(projectInfo);
//
// 	if ( outputURL().isEmpty() )
// 	{
// 		KMessageBox::sorry( 0l, i18n("Don't know how to build \"%1\" (output url is empty).").arg(name()) );
// 		return false;
// 	}
//
// 	// Build all internal libraries that we depend on
// 	QStringList::iterator send = m_linkedInternal.end();
// 	for ( QStringList::iterator it = m_linkedInternal.begin(); it != send; ++it )
// 	{
// 		ProjectItem * lib = projectInfo->findItem( projectInfo->directory() + *it );
// 		if ( !lib )
// 		{
// 			KMessageBox::sorry( 0l, i18n("Don't know how to build \"%1\" (library does not exist in project).").arg(*it) );
// 			return false;
// 		}
//
// 		if ( !lib->build(pol) )
// 			return false;
// 	}
//
//
// 	// Build all children
// 	m_children.remove( (ProjectItem*)0l );
// 	ProjectItemList::iterator cend = m_children.end();
// 	for ( ProjectItemList::iterator it = m_children.begin(); it != cend; ++it )
// 	{
// 		if ( ! (*it)->build(pol) )
// 			return false;
// 	}
//
//
// 	// Now build ourself
// 	ProcessOptions po;
// 	po.b_addToProject = false;
// 	po.setTargetFile( outputURL().path() );
// 	po.m_picID = microID();
//
// 	ProcessOptions::ProcessPath::MediaType typeTo;
//
// 	switch ( outputType() )
// 	{
// 		case UnknownOutput:
// 			KMessageBox::sorry( 0l, i18n("Don't know how to build \"%1\" (unknown output type).").arg(name()) );
// 			return false;
//
// 		case ProgramOutput:
// 			typeTo = ProcessOptions::ProcessPath::Program;
// 			break;
//
// 		case ObjectOutput:
// 			typeTo = ProcessOptions::ProcessPath::Object;
// 			break;
//
// 		case LibraryOutput:
// 			typeTo = ProcessOptions::ProcessPath::Library;
// 			break;
// 	}
//
// 	switch ( type() )
// 	{
// 		case ProjectType:
// 			// Nothing to do
// 			return true;
//
// 		case FileType:
// 			po.setInputFiles( url().path() );
// 			po.setProcessPath( ProcessOptions::ProcessPath::path( ProcessOptions::guessMediaType( url().url() ), typeTo ) );
// 			break;
//
// 		case ProgramType:
// 		case LibraryType:
// 			// Build up a list of input urls
// 			QStringList inputFiles;
//
// 			// Link child objects
// 			m_children.remove( (ProjectItem*)0l );
// 			ProjectItemList::iterator cend = m_children.end();
// 			for ( ProjectItemList::iterator it = m_children.begin(); it != cend; ++it )
// 				inputFiles << (*it)->outputURL().path();
//
// 			po.setInputFiles(inputFiles);
// 			po.setProcessPath( ProcessOptions::ProcessPath::path( ProcessOptions::ProcessPath::Object, typeTo ) );
// 			break;
// 	}
//
// 	po.m_hexFormat = hexFormatToString( hexFormat() );
// 	po.m_bOutputMapFile = outputMapFile();
// 	po.m_libraryDir = libraryDir();
// 	po.m_linkerScript = linkerScript();
// 	po.m_linkOther = linkerOther();
//
// 	// Link against libraries
// 	QStringList::iterator lend = m_linkedInternal.end();
// 	for ( QStringList::iterator it = m_linkedInternal.begin(); it != lend; ++it )
// 		po.m_linkLibraries += projectInfo->directory() + *it;
// 	lend = m_linkedExternal.end();
// 	for ( QStringList::iterator it = m_linkedExternal.begin(); it != lend; ++it )
// 		po.m_linkLibraries += *it;
//
// 	// Save our working file (if open) and append to the build list
// 	Document * currentDoc = DocManager::self()->findDocument( url() );
// 	if (currentDoc)
// 		currentDoc->fileSave();
// 	pol->append(po);
//
// 	return true;
// }

//FIXME: port upload()
void ProjectItem::upload( ProcessOptionsList * pol )
{}
// void ProjectItem::upload( ProcessOptionsList * pol )
// {
// 	build( pol );
//
// 	ProgrammerDlg * dlg = new ProgrammerDlg( microID(), (QWidget*)KTechlab::self(), "Programmer Dlg" );
//
// 	dlg->exec();
// 	if ( !dlg->isAccepted() )
// 	{
// 		dlg->deleteLater();
// 		return;
// 	}
//
// 	ProcessOptions po;
// 	dlg->initOptions( & po );
// 	po.b_addToProject = false;
// 	po.setInputFiles( outputURL().path() );
// 	po.setProcessPath( ProcessOptions::ProcessPath::Program_PIC );
//
// 	pol->append( po );
//
// 	dlg->deleteLater();
// }


QDomElement ProjectItem::toDomElement( QDomDocument & doc, const KUrl & baseURL ) const
{
    QDomElement node = doc.createElement("item");

    node.setAttribute( "type", typeToString() );
    node.setAttribute( "name", m_name );
    node.setAttribute( "url", KUrl::relativeUrl( baseURL, m_url.url() ) );

    if ( m_linkerOptions )
        node.appendChild( m_linkerOptions->toDomElement( doc, baseURL ) );
    if ( m_processingOptions )
        node.appendChild( m_processingOptions->toDomElement( doc, baseURL ) );


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

        //FIXME: Port this
//		if ( ((*it)->type() & types) && ((*it)->outputType() & outputTypes) )
//			urls += (*it)->outputURL().prettyUrl();

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

    m_children.removeAll( (ProjectItem*)0l );
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
    //FIXME: Port this
    KUrl::List urls;// = KTechlab::self()->getFileURLs();
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

    m_children.removeAll( (ProjectItem*)0l );
    ProjectItemList::iterator end = m_children.end();
    for ( ProjectItemList::iterator it = m_children.begin(); it != end; ++it )
    {
//FIXME: port this...
        /*        if ( (*it)->type() == FileType && (*it)->url() == url )
            return;*/
    }

    ProjectItem * item = ProjectItem::createProjectItem( this, "File", m_pProjectManager );
    item->setURL(url);
    addChild(item);
}

ProjectItem * ProjectItem::createProjectItem( ProjectItem * parent, QString type, ProjectManager * manager)
{
    if ( type == "Project" )
        return new ProjectItemTypes::Project(parent, manager);
    if ( type == "File" )
        return new ProjectItemTypes::File(parent, manager);
    if ( type == "Program" )
        return new ProjectItemTypes::Program(parent, manager);
    if ( type == "Library" )
        return new ProjectItemTypes::Library(parent, manager);

    kError() << "Couldn't create ProjectItem of type "<<type;
    return 0;
}

/*
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
*/

bool ProjectItem::fromDomElement( const QDomElement & element, const KUrl & baseURL )
{
    QString type = element.attribute( "type", QString() );
    QString name = element.attribute( "name", QString() );
    KUrl url( baseURL, element.attribute( "url", QString() ) );

    ProjectItem * createdItem = ProjectItem::createProjectItem( this, type, m_pProjectManager );
    createdItem->setName( name );
    createdItem->setURL( url );

    addChild( createdItem );

    QDomNode node = element.firstChild();
    while ( !node.isNull() )
    {
        QDomElement childElement = node.toElement();
        if ( !childElement.isNull() )
        {
            const QString tagName = childElement.tagName();

            //TODO: should these decisions be replaced to somewhere else?
            if ( tagName == "linker" )
            {
                LinkerOptions * o = new LinkerOptions();
                if ( o->fromDomElement( childElement, baseURL ) )
                    createdItem->setLinkerOptions( o );
                else
                    delete o;
            } else if ( tagName == "processing" )
            {
                ProcessingOptions * o = new ProcessingOptions();
                if ( o->fromDomElement( childElement, baseURL ) )
                    createdItem->setProcessingOptions( o );
                else
                    delete o;
            } else if ( tagName == "item" )
            {
                createdItem->fromDomElement( childElement, baseURL );
            } else
                kError() << k_funcinfo << "Unrecognised element tag name: "<<tagName<<endl;
        }

        node = node.nextSibling();
    }
    return true;
}
//END class ProjectItem

//BEGIN namespace ProjectItemTypes
namespace ProjectItemTypes {

void File::updateILVItemPixmap()
{
    if ( !m_pILVItem )
        return;

    m_pILVItem->setIcon( 0, KIcon( KMimeType::iconNameForUrl( url() ) ) );
}


void ProjectItemTypes::File::setURL( const KUrl & url )
{
    if ( processingOptions() && processingOptions()->outputURL().isEmpty() )
    {
        // Try and guess what the output url should be...
        QString newExtension = outputExtension();
/*
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
        }*/

        if ( !newExtension.isEmpty() )
        {
            const QString fileName = url.url();
            QString extension = fileName.right( fileName.length() - fileName.lastIndexOf('.') );
            ProjectItem::setURL( QString(fileName).replace( extension, newExtension ) );
        }
    }
}


void Program::updateILVItemPixmap()
{
    if ( !m_pILVItem )
        return;

    m_pILVItem->setIcon( 0, KIcon( "project_program.png" ) );
}

void Library::updateILVItemPixmap()
{
    if ( !m_pILVItem )
        return;

    m_pILVItem->setIcon( 0, KIcon( "project_library.png" ) );
}

}
//END namespace ProjectItemTypes


//BEGIN class ProjectInfo
ProjectInfo::ProjectInfo( ProjectManager * projectManager )
{
    m_project = ProjectItem::createProjectItem( 0l, "Project", projectManager );
}


ProjectInfo::~ ProjectInfo()
{
    delete m_project;
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
        KMessageBox::sorry( 0l, i18n("Could not open %1 for reading").arg(target) );
        return false;
    }

    QString xml;
    QTextStream textStream( &file );
    while ( !textStream.atEnd() )
        xml += textStream.readLine() + '\n';

    file.close();

    QDomDocument doc( "KTechlab" );
    QString errorMessage;
    if ( !doc.setContent( xml, &errorMessage ) )
    {
        KMessageBox::sorry( 0l, i18n("Couldn't parse xml:\n%1").arg(errorMessage) );
        return false;
    }

    QDomElement root = doc.documentElement();

    m_project->fromDomElement( root, url );

    return true;
}


bool ProjectInfo::save()
{
    QFile file( m_project->url().path() );
    if ( file.open(QIODevice::WriteOnly) == false )
    {
        KMessageBox::sorry( NULL, i18n("Project could not be saved to \"%1\"").arg(m_project->url().path()), i18n("Saving Project") );
        return false;
    }

    QDomDocument doc("KTechlab");

    QDomElement root = m_project->toDomElement( doc, m_project->url() );

    QTextStream stream(&file);
    stream << doc.toString();
    file.close();

    //FIXME: get reference from KApplication
    //(static_cast<RecentFilesAction*>(KTechlab::self()->action("project_open_recent")))->addURL(m_url);

    return true;
}


bool ProjectInfo::saveAndClose()
{
    if (!save())
        return false;

    if (!m_project->closeOpenFiles())
        return false;

    return true;
}
//END class ProjectInfo



//BEGIN class ProjectManager
ProjectManager * ProjectManager::m_pSelf = 0l;

ProjectManager * ProjectManager::self( QWidget * parent )
{
	if ( !m_pSelf )
	{
		assert(parent);
		m_pSelf = new ProjectManager( parent );
	}
	return m_pSelf;
}


ProjectManager::ProjectManager( QWidget * parent )
//	: ItemSelector( parent, "Project Manager" ),
    : m_pCurrentProject(0l)
{
    QWhatsThis::add( this, i18n("Displays the list of files in the project.\nTo open or close a project, use the \"Project\" menu. Right click on a file to remove it from the project") );
    m_treeWidget = new QTreeWidget(parent);

    m_treeWidget->setListCaption( i18n("File") );
    m_treeWidget->setCaption( i18n("Project Manager") );

	connect( this, SIGNAL(clicked(QListViewItem*)), this, SLOT(slotItemClicked(QListViewItem*)) );
}


ProjectManager::~ProjectManager()
{
    delete m_treeWidget;
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
		m_pCurrentProject->setName( newProjectDlg->projectName() );
		m_pCurrentProject->setURL( newProjectDlg->location() + m_pCurrentProject->name().lower() + ".ktechlab" );

        QDir dir;
        if ( !dir.mkdir( m_pCurrentProject->directory() ) )
			kdDebug() << "Error in creating directory " << m_pCurrentProject->directory() << endl;

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

    KURL url = KFileDialog::getOpenURL(QString::null, filter, this, i18n("Open Location"));

    if ( url.isEmpty() )
		return;

	slotOpenProject(url);
}


void ProjectManager::slotOpenProject( const KURL & url )
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

	RecentFilesAction * rfa = static_cast<RecentFilesAction*>(KTechlab::self()->action("project_open_recent"));
	rfa->addURL( m_pCurrentProject->url() );

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

	KTechlab::self()->action("project_create_subproject")->setEnabled( projectIsOpen );
	KTechlab::self()->action("project_export_makefile")->setEnabled( projectIsOpen );
	KTechlab::self()->action("subproject_add_existing_file")->setEnabled( projectIsOpen );
	KTechlab::self()->action("subproject_add_current_file")->setEnabled( projectIsOpen );
// 	KTechlab::self()->action("project_options")->setEnabled( projectIsOpen );
	KTechlab::self()->action("project_close")->setEnabled( projectIsOpen );
	KTechlab::self()->action("project_add_existing_file")->setEnabled( projectIsOpen );
	KTechlab::self()->action("project_add_current_file")->setEnabled( projectIsOpen );
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

	int choice = KMessageBox::questionYesNo( this, i18n("Do you really want to remove \"%1\"?").arg( currentItem->text(0) ), i18n("Remove Project File?"), KGuiItem(i18n("Remove")), KGuiItem(i18n("Cancel")) );

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


void ProjectManager::slotItemClicked( QListViewItem * item )
{
	ILVItem * ilvItem = dynamic_cast<ILVItem*>(item);
	if ( !ilvItem )
		return;

	ProjectItem * projectItem = ilvItem->projectItem();
	if ( !projectItem || projectItem->type() != ProjectItem::FileType )
		return;

	DocManager::self()->openURL( projectItem->url() );
}


void ProjectManager::slotContextMenuRequested( QListViewItem * item, const QPoint& pos, int /*col*/ )
{
	QString popupName;
	ILVItem * ilvItem = dynamic_cast<ILVItem*>(item);
	KAction * linkerOptionsAct = KTechlab::self()->action("project_item_linker_options");
	linkerOptionsAct->setEnabled(false);

	if ( !m_pCurrentProject )
		popupName = "project_none_popup";

	else if ( !ilvItem )
		popupName = "project_blank_popup";

	else
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
	KTechlab::self()->action("subproject_add_current_file")->setEnabled( haveFocusedDocument );
	KTechlab::self()->action("project_add_current_file")->setEnabled( haveFocusedDocument );

	QPopupMenu *pop = static_cast<QPopupMenu*>(KTechlab::self()->factory()->container( popupName, KTechlab::self() ));
	if (pop)
		pop->popup(pos);
}
//END class ProjectManager

#include "projectmanager.moc"
