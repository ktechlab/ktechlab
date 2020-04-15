/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
#ifndef NO_GPSIM

#include "canvasitemparts.h"
#include "circuitdocument.h"
#include "docmanager.h"
#include "gpsimprocessor.h"
#include "libraryitem.h"
#include "logic.h"
#include "ktechlab.h"
#include "micropackage.h"
#include "picinfo.h"
#include "microlibrary.h"
#include "piccomponent.h"
#include "piccomponentpin.h"
#include "projectmanager.h"

#include <qdebug.h>
#include <qicon.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <qpointer.h>
#include <qstringlist.h>

#include "gpsim/ioports.h"
#include "gpsim/pic-processor.h"

QString PICComponent::_def_PICComponent_fileName = QString::null;


Item* PICComponent::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new PICComponent( (ICNDocument*)itemDocument, newItem, id );
}


LibraryItem* PICComponent::libraryItem()
{
	QStringList IDs;
	IDs << "ec/pic" << "ec/picitem" << "ec/picitem_18pin";

	return new LibraryItem(
		IDs,
		"PIC",
		i18n("Integrated Circuits"),
		"ic2.png",
		LibraryItem::lit_component,
		PICComponent::construct );
}

PICComponent::PICComponent( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "pic" )
{
	m_name = i18n("PIC Micro");

	if ( _def_PICComponent_fileName.isEmpty() )
		_def_PICComponent_fileName = i18n("<Enter location of PIC Program>");

	m_bCreatedInitialPackage = false;
	m_bLoadingProgram = false;
	m_pGpsim = nullptr;

	addButton( "run", QRect(), QIcon::fromTheme( "media-playback-start" ) );
	addButton( "pause", QRect(), QIcon::fromTheme( "media-playback-pause" ) );
	addButton( "reset", QRect(), QIcon::fromTheme( "process-stop" ) );
	addButton( "reload", QRect(), QIcon::fromTheme( "view-refresh" ) );

	connect( KTechlab::self(), SIGNAL(recentFileAdded(const KUrl &)), this, SLOT(slotUpdateFileList()) );

	connect( ProjectManager::self(),	SIGNAL(projectOpened()),		this, SLOT(slotUpdateFileList()) );
	connect( ProjectManager::self(),	SIGNAL(projectClosed()),		this, SLOT(slotUpdateFileList()) );
	connect( ProjectManager::self(),	SIGNAL(projectCreated()),		this, SLOT(slotUpdateFileList()) );
	connect( ProjectManager::self(),	SIGNAL(subprojectCreated()),	this, SLOT(slotUpdateFileList()) );
	connect( ProjectManager::self(),	SIGNAL(filesAdded()),			this, SLOT(slotUpdateFileList()) );
	connect( ProjectManager::self(),	SIGNAL(filesRemoved()),			this, SLOT(slotUpdateFileList()) );

	createProperty( "program", Variant::Type::FileName );
	property("program")->setCaption( i18n("Program") );
	QString filter;
	filter = QString("*.flowcode *.cod *.asm *.basic *.c|%1").arg(i18n("All Supported Files"));
	filter += QString("\n*.flowcode|FlowCode (*.flowcode)");
	filter += QString("\n*.cod|%1 (*.cod)").arg(i18n("Symbol File"));
	filter += QString("\n*.asm|%1 (*.asm)").arg(i18n("Assembly Code"));
	filter += QString("\n*.basic *.microbe|Microbe (*.basic, *.microbe)");
	filter += QString("\n*.c|C (*.c)");
	filter += QString("\n*|%1").arg(i18n("All Files"));
	property("program")->setFilter( filter );

	// Used for restoring the pins on file loading before we have had a change
	// to compile the PIC program
	createProperty( "lastPackage", Variant::Type::String );
	property("lastPackage")->setHidden( true );

// 	//HACK This is to enable loading with pre-0.3 files (which didn't set a "lastPackage"
// 	// property). This will allow a P16F84 PIC to be initialized (which agrees with pre-0.3
// 	// behaviour), but it will also load it if

	// This to allow loading of the PIC component from pre-0.3 files (which didn't set a
	// "lastPackage" property).
	if ( !newItem )
		property("lastPackage")->setValue("P16F84");

	slotUpdateFileList();
	slotUpdateBtns();

	initPackage( nullptr );
}


PICComponent::~PICComponent()
{
	deletePICComponentPins();
	delete m_pGpsim;
}


void PICComponent::dataChanged()
{
    qDebug() << Q_FUNC_INFO;
	initPIC(false);
}


void PICComponent::initPIC( bool forceReload )
{
	if ( !m_bCreatedInitialPackage )
	{
        qDebug() << Q_FUNC_INFO << " creating initial package";
		// We are still being created, so other connectors will be expecting us to
		// have grown pins soonish.
		MicroInfo * microInfo = MicroLibrary::self()->microInfoWithID( dataString("lastPackage") );
		if ( microInfo ) {
			initPackage( microInfo );
        } else {
            qDebug() << Q_FUNC_INFO << " unknown last package: " << dataString("lastPackage");
        }
	}

	QString newProgram = KUrl( dataString("program") ).path();
    qDebug() << Q_FUNC_INFO << "newProgram=" << newProgram;
	bool newFile = (m_picFile != newProgram);
	if ( !newFile && !forceReload ) {
        qDebug() << Q_FUNC_INFO << "not new program, not force reload, exiting";
		return;
    }

	delete m_pGpsim;
	m_pGpsim = nullptr;

	switch ( GpsimProcessor::isValidProgramFile(newProgram) )
	{
		case GpsimProcessor::DoesntExist:
			if ( newProgram == _def_PICComponent_fileName && !newProgram.isEmpty() )
				break;
			KMessageBox::sorry( nullptr, i18n("The file \"%1\" does not exist.", newProgram ) );
			m_picFile = QString::null;
			break;

		case GpsimProcessor::IncorrectType:
			if ( newProgram == _def_PICComponent_fileName && !newProgram.isEmpty() )
				break;
			KMessageBox::sorry( nullptr, i18n("\"%1\" is not a valid PIC program.\nThe file must exist, and the extension should be \".cod\", \".asm\", \".flowcode\", \".basic\", \".microbe\" or \".c\".\n\".hex\" is allowed, provided that there is a corresponding \".cod\" file.", newProgram) );
			m_picFile = QString::null;
			break;

		case GpsimProcessor::Valid:
			m_picFile = newProgram;
			m_symbolFile = createSymbolFile();
			break;
	}

	slotUpdateBtns();
}


void PICComponent::deletePICComponentPins()
{
	const PICComponentPinMap::iterator picComponentMapEnd = m_picComponentPinMap.end();
	for ( PICComponentPinMap::iterator it = m_picComponentPinMap.begin(); it != picComponentMapEnd; ++it )
		delete it.value();
	m_picComponentPinMap.clear();
}


void PICComponent::initPackage( MicroInfo * microInfo )
{
	MicroPackage * microPackage = microInfo ? microInfo->package() : nullptr;

	if ( microPackage )
	{
		m_bCreatedInitialPackage = true;

		//BEGIN Get pin IDs
		QStringList allPinIDs = microPackage->pinIDs();
		QStringList ioPinIDs = microPackage->pinIDs( PicPin::type_bidir | PicPin::type_input | PicPin::type_open );

		// Now, we make the unwanted pin ids blank, so a pin is not created for them
		const QStringList::iterator allPinIDsEnd = allPinIDs.end();
		for ( QStringList::iterator it = allPinIDs.begin(); it != allPinIDsEnd; ++it )
		{
			if ( !ioPinIDs.contains(*it) )
				*it = "";
		}
		//END Get pin IDs


		//BEGIN Remove old stuff
		// Remove old text
		TextMap textMapCopy = m_textMap;
		const TextMap::iterator textMapEnd = textMapCopy.end();
		for ( TextMap::iterator it = textMapCopy.begin(); it != textMapEnd; ++it )
			removeDisplayText(it.key());

		// Remove the old pins
		deletePICComponentPins();

		// Remove old nodes
		NodeInfoMap nodeMapCopy = m_nodeMap;
		const NodeInfoMap::iterator nodeMapEnd = nodeMapCopy.end();
		for ( NodeInfoMap::iterator it = nodeMapCopy.begin(); it != nodeMapEnd; ++it )
		{
			if ( !ioPinIDs.contains(it.key()) )
				removeNode( it.key() );
		}

		removeElements();
		//END Remove old stuff



		//BEGIN Create new stuff
		initDIPSymbol( allPinIDs, 80 );
		initDIP(allPinIDs);

		PicPinMap picPinMap = microPackage->pins( PicPin::type_bidir | PicPin::type_input | PicPin::type_open );
		const PicPinMap::iterator picPinMapEnd = picPinMap.end();
		for ( PicPinMap::iterator it = picPinMap.begin(); it != picPinMapEnd; ++it )
			m_picComponentPinMap[it.key()] = new PICComponentPin( this, it.value() );
		//END Create new stuff


		removeDisplayText( "no_file" );
		addDisplayText( "picid", QRect(offsetX(), offsetY()-16, width(), 16), microInfo->id() );
	}
	else
	{
		setSize( -48, -72, 96, 144 );
		removeDisplayText( "picid" );
		addDisplayText( "no_file", sizeRect(), i18n("(No\nprogram\nloaded)") );
	}


	//BEGIN Update button positions
	int leftpos = (width()-88)/2+offsetX();
	button("run")->setOriginalRect( QRect( leftpos, height()+4+offsetY(), 20, 20 ) );
	button("pause")->setOriginalRect( QRect( leftpos+23, height()+4+offsetY(), 20, 20 ) );
	button("reset")->setOriginalRect( QRect( leftpos+46, height()+4+offsetY(), 20, 20 ) );
	button("reload")->setOriginalRect( QRect( leftpos+69, height()+4+offsetY(), 20, 20 ) );
	updateAttachedPositioning();
	//END Update button positions
}


void PICComponent::attachPICComponentPins()
{
	if ( !m_pGpsim || !m_pGpsim->picProcessor() )
		return;

	pic_processor * picProcessor = m_pGpsim->picProcessor();

	const PICComponentPinMap::iterator end = m_picComponentPinMap.end();
	for ( PICComponentPinMap::iterator it = m_picComponentPinMap.begin(); it != end; ++it )
		it.value()->attach( picProcessor->get_pin( it.key() ) );
}


void PICComponent::slotUpdateFileList()
{
	QStringList preFileList = KTechlab::self()->recentFiles();

	QStringList fileList;

	if ( ProjectInfo * info = ProjectManager::self()->currentProject() )
	{
		const QList<QUrl> urls = info->childOutputURLs( ProjectItem::AllTypes, ProjectItem::ProgramOutput );
		for (const QUrl &url : urls)
			fileList << url.toLocalFile();
	}

	const QStringList::iterator end = preFileList.end();
	for ( QStringList::iterator it = preFileList.begin(); it != end; ++it )
	{
		QString file = KUrl(*it).path();
		if ( (file.endsWith(".flowcode") || file.endsWith(".asm") || file.endsWith(".cod") || file.endsWith(".basic") || file.endsWith(".microbe") ) && !fileList.contains(file) ) {
			fileList.append(file);
		}
	}

	QString fileName = dataString("program");

	property("program")->setAllowed(fileList);
	property("program")->setValue( fileName.isEmpty() ? _def_PICComponent_fileName : fileName );
}


void PICComponent::buttonStateChanged( const QString &id, bool state )
{
	if (!state)
		return;

	if ( id == "reload" )
	{
		programReload();
		return;
	}

	if (!m_pGpsim)
		return;

	if ( id == "run" )
		m_pGpsim->setRunning(true);

	else if ( id == "pause" )
		m_pGpsim->setRunning(false);

	else if ( id == "reset" )
	{
		m_pGpsim->reset();

		// Set all pin outputs to low
		const PICComponentPinMap::iterator end = m_picComponentPinMap.end();
		for ( PICComponentPinMap::iterator it = m_picComponentPinMap.begin(); it != end; ++it )
			it.value()->resetOutput();
	}

	slotUpdateBtns();
}


bool PICComponent::mouseDoubleClickEvent ( const EventInfo &eventInfo )
{
	Q_UNUSED(eventInfo);
	if ( m_picFile.isEmpty() || (m_picFile == _def_PICComponent_fileName) )
		return false;

	(void) DocManager::self()->openURL(m_picFile);

	return true;
}


QString PICComponent::createSymbolFile()
{
    qDebug() << Q_FUNC_INFO;
	m_bLoadingProgram = true;
	slotUpdateBtns();

	return GpsimProcessor::generateSymbolFile( dataString("program"), this, SLOT(slotCODCreationSucceeded()), SLOT(slotCODCreationFailed()) );
}


void PICComponent::slotCODCreationSucceeded()
{
    qDebug() << Q_FUNC_INFO << " m_symbolFile=" << m_symbolFile;
	m_bLoadingProgram = false;

	delete m_pGpsim;
	m_pGpsim = new GpsimProcessor(m_symbolFile);

	if ( m_pGpsim->codLoadStatus() == GpsimProcessor::CodSuccess )
	{
		MicroInfo * microInfo = m_pGpsim->microInfo();
		if(!microInfo){
			// FIXME we should be select somehow the type of the PIC. this is only a stability hack.
			qWarning() << Q_FUNC_INFO << "cannot identify the PIC, defaulting to P16F84" << endl;
			microInfo = MicroLibrary::self()->microInfoWithID("P16F84");
		}
		property("lastPackage")->setValue( microInfo->id() );
		initPackage( microInfo );

		connect( m_pGpsim, SIGNAL(runningStatusChanged(bool )), this, SLOT(slotUpdateBtns()) );
		attachPICComponentPins();
	}

	else
	{
		m_pGpsim->displayCodLoadStatus();
		delete m_pGpsim;
		m_pGpsim = nullptr;
	}

	slotUpdateBtns();
}


void PICComponent::slotCODCreationFailed()
{
	m_bLoadingProgram = false;
	slotUpdateBtns();
}


void PICComponent::programReload()
{
    qDebug() << Q_FUNC_INFO;

	delete m_pGpsim;
	m_pGpsim = nullptr;

	initPIC(true);

	slotUpdateBtns();
}


void PICComponent::slotUpdateBtns()
{
	// We can get called by the destruction of gpsim after our canvas has been set to nullptr
	if (!canvas()) {
        qDebug() << Q_FUNC_INFO << " no canvas, exiting";
		return;
    }

	button("run")->setEnabled( m_pGpsim && !m_pGpsim->isRunning() );
	button("pause")->setEnabled( m_pGpsim && m_pGpsim->isRunning() );
	button("reset")->setEnabled( m_pGpsim );
	button("reload")->setEnabled( !m_bLoadingProgram && (dataString("program") != _def_PICComponent_fileName) );

	canvas()->setChanged( button("run")->boundingRect() );
	canvas()->setChanged( button("pause")->boundingRect() );
	canvas()->setChanged( button("reset")->boundingRect() );
	canvas()->setChanged( button("reload")->boundingRect() );
}

#endif
