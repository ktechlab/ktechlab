/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "asmformatter.h"
#include "asminfo.h"
#include "asmparser.h"
#include "debugmanager.h"
#include "docmanager.h"
#include "documentiface.h"
#include "filemetainfo.h"
#include "gpsimprocessor.h"
#include "ktechlab.h"
#include "language.h"
#include "languagemanager.h"
#include "microselectwidget.h"
#include "programmerdlg.h"
#include "symbolviewer.h"
#include "textdocument.h"
#include "textview.h"

// #include <kate/katedocument.h>

#include <kdebug.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktempfile.h>


TextDocument *TextDocument::constructTextDocument( const QString& caption, KTechlab *parent, const char *name )
{
	TextDocument *textDocument = new TextDocument( caption, parent, name);
	if( textDocument->m_constructorSuccessful )
		return textDocument;
	delete textDocument;
	return 0;
}


TextDocument::TextDocument( const QString &caption, KTechlab *ktechlab, const char *name )
	: Document( caption, ktechlab, name ),
	  m_doc(0)
{
	m_constructorSuccessful = false;
	
#ifndef NO_GPSIM
	m_bOwnDebugger = false;
	b_lockSyncBreakpoints = false;
	m_lastDebugLineAt = -1;
	m_pDebugger = 0;
#endif

	m_pLastTextOutputTarget = 0;
	m_guessedCodeType = TextDocument::ct_unknown;
	m_type = Document::dt_text;
	m_bookmarkActions.setAutoDelete(true);
	m_pDocumentIface = new TextDocumentIface(this);
	
	KLibFactory *factory = KLibLoader::self()->factory("libkatepart");
	if(!factory) {
		KMessageBox::sorry( ktechlab, i18n("Libkatepart not available for constructing editor") );
		return;
	}
	m_doc = (Kate::Document*)(KTextEditor::Document *)factory->create( 0, "kate", "KTextEditor::Document");
	
	guessScheme();
	
	connect( m_doc, SIGNAL(undoChanged()),		this, SIGNAL(undoRedoStateChanged()) );
	connect( m_doc, SIGNAL(undoChanged()),		this, SLOT(slotSyncModifiedStates()) );
	connect( m_doc, SIGNAL(textChanged()),		this, SLOT(slotSyncModifiedStates()) );
	connect( m_doc, SIGNAL(marksChanged()),		this, SLOT(slotUpdateMarksInfo()) );
	connect( m_doc,	SIGNAL(selectionChanged()),	this, SLOT(slotSelectionmChanged()) );
	
	m_doc->setDescription((KTextEditor::MarkInterface::MarkTypes)Breakpoint, i18n("Breakpoint"));
	m_doc->setPixmap((KTextEditor::MarkInterface::MarkTypes)Breakpoint, *inactiveBreakpointPixmap());
	m_doc->setPixmap((KTextEditor::MarkInterface::MarkTypes)ActiveBreakpoint, *activeBreakpointPixmap());
	m_doc->setPixmap((KTextEditor::MarkInterface::MarkTypes)ReachedBreakpoint, *reachedBreakpointPixmap());
	m_doc->setPixmap((KTextEditor::MarkInterface::MarkTypes)DisabledBreakpoint, *disabledBreakpointPixmap());
	m_doc->setPixmap((KTextEditor::MarkInterface::MarkTypes)ExecutionPoint, *executionPointPixmap());
	m_doc->setMarksUserChangable( Bookmark | Breakpoint );

	m_constructorSuccessful = true;
}


TextDocument::~TextDocument()
{
	if(!m_constructorSuccessful) return;

	debugStop();

	ViewList::iterator end = m_viewList.end();
	for(ViewList::iterator it = m_viewList.begin(); it != end; ++it) {
		if(TextView * tv = dynamic_cast<TextView*>( (View*)*it)) {
			Kate::View * kv = tv->kateView();
			p_ktechlab->factory()->removeClient( kv );
		}
	}
	
	delete m_doc;
	delete m_pDocumentIface;
}

bool TextDocument::fileClose()
{
	const QString path = url().prettyURL();
	if ( !path.isEmpty() )
		fileMetaInfo()->grabMetaInfo( path, this );
	
	return Document::fileClose();
}

TextView* TextDocument::textView() const
{
	return static_cast<TextView*>(activeView());
}

View * TextDocument::createView( ViewContainer *viewContainer, uint viewAreaId, const char *name )
{
	TextView * textView = new TextView( this, viewContainer, viewAreaId, name );
	
	fileMetaInfo()->initializeFromMetaInfo( url().prettyURL(), textView );
	
	handleNewView(textView);
	return textView;
}

Kate::View* TextDocument::createKateView( QWidget *parent, const char *name )
{
	return static_cast<Kate::View*>((m_doc->createView( parent, name ))->qt_cast("Kate::View"));
}


void TextDocument::cut()
{
	if (textView()) textView()->cut();
}

void TextDocument::copy()
{
	if (textView()) textView()->copy();
}

void TextDocument::paste()
{
	if (textView()) textView()->paste();
}

void TextDocument::setText( const QString & text, bool asInitial )
{
	if(asInitial) {
		disconnect( m_doc, SIGNAL(undoChanged()), this, SIGNAL(undoRedoStateChanged()) );
		disconnect( m_doc, SIGNAL(undoChanged()), this, SLOT(slotSyncModifiedStates()) );
		disconnect( m_doc, SIGNAL(textChanged()), this, SLOT(slotSyncModifiedStates()) );
	}

	const ViewList::iterator end = m_viewList.end();
	for ( ViewList::iterator it = m_viewList.begin(); it != end; ++it )
		(static_cast<TextView*>((View*)*it))->saveCursorPosition();

	m_doc->setText(text);
	
	for ( ViewList::iterator it = m_viewList.begin(); it != end; ++it )
		(static_cast<TextView*>((View*)*it))->restoreCursorPosition();

	if ( asInitial ) {
		m_doc->clearUndo();
		m_doc->clearRedo();
		setModified(false);
	
		connect( m_doc, SIGNAL(undoChanged()), this, SIGNAL(undoRedoStateChanged()) );
		connect( m_doc, SIGNAL(undoChanged()), this, SLOT(slotSyncModifiedStates()) );
		connect( m_doc, SIGNAL(textChanged()), this, SLOT(slotSyncModifiedStates()) );
	}
}

void TextDocument::undo()
{
	m_doc->undo();
	slotSyncModifiedStates();
}

void TextDocument::redo()
{
	m_doc->redo();
	slotSyncModifiedStates();
}

void TextDocument::slotSyncModifiedStates()
{
	setModified( m_doc->isModified() );
}
void TextDocument::setModified( bool modified )
{
	if ( (modified == b_modified) && (modified == isModified()) ) {
		return;
	}
	m_doc->setModified(modified);
	b_modified = modified;
	
	emit modifiedStateChanged();
}

void TextDocument::guessScheme( bool allowDisable )
{
	// And specific file actions depending on the current type of file
	QString fileName = url().fileName();
	QString extension = fileName.right( fileName.length() - fileName.findRev('.') - 1 );
	
	if ( extension == "asm" || extension == "src" || extension == "inc" )
		slotInitLanguage(ct_asm);
	else if ( extension == "hex" )
		slotInitLanguage(ct_hex);
	else if ( extension == "basic" || extension == "microbe" )
		slotInitLanguage(ct_microbe);
	else if ( extension == "c" )
		slotInitLanguage(ct_c);
	else if ( m_guessedCodeType != TextDocument::ct_unknown )
		slotInitLanguage(m_guessedCodeType);
	else if ( allowDisable && activeView() )
		textView()->disableActions();
}

void TextDocument::slotInitLanguage( CodeType type )
{
	QString hlName;
	
	switch (type)
	{
		case ct_asm:
			hlName = "PicAsm";
			break;
			
		case ct_c:
			hlName = "C";
			break;
			
		case ct_hex:
			break;
			
		case ct_microbe:
			hlName = "Microbe";
			break;
			
		case ct_unknown:
			break;
	}
	
	if ( !hlName.isEmpty() ) {
		int i = 0;
		int hlModeCount = m_doc->hlModeCount();
		while ( i<hlModeCount && m_doc->hlModeName(i) != hlName )
			i++;
		
		m_doc->setHlMode(i);
	}
	
	m_guessedCodeType = type;
	
	ViewList::iterator end = m_viewList.end();
	for ( ViewList::iterator it = m_viewList.begin(); it != end; ++it )
	{
		if ( TextView * tv = dynamic_cast<TextView*>( (View*)*it ) )
			tv->initCodeActions();
	}
}

void TextDocument::formatAssembly()
{
	AsmFormatter formatter;
	QStringList lines = QStringList::split( "\n", m_doc->text(), true );
	setText( formatter.tidyAsm(lines), false );
	setModified(true);
}

void TextDocument::fileSave( const KURL& url )
{
	if ( m_doc->url().path() != url.path() )
	{
		kdError() << k_funcinfo << "Error: Kate::View url and passed url do not match; cannot save." << endl;
		return;
	}
	
	if ( activeView() && (textView()->save() == Kate::View::SAVE_OK ) )
		saveDone();
}

void TextDocument::fileSaveAs()
{
	if (  activeView() && (textView()->saveAs() == Kate::View::SAVE_OK) )
		saveDone();
	
	// Our modified state may not have changed, but we emit this to force the
	// main window to update our caption.
	emit modifiedStateChanged();
}

void TextDocument::saveDone()
{
	setURL( m_doc->url() );
	guessScheme(false);
	setModified(false);
	emit modifiedStateChanged();
}

bool TextDocument::openURL( const KURL& url )
{
	m_doc->openURL(url);
	setURL(url);
	
	fileMetaInfo()->initializeFromMetaInfo( url.prettyURL(), this );
	guessScheme();

#ifndef NO_GPSIM
	DebugManager::self()->urlOpened( this );
#endif
	
	return true;
}

void TextDocument::setLastTextOutputTarget( TextDocument * target )
{
	m_pLastTextOutputTarget = target;
}

QString TextDocument::outputFilePath( const QString &ext )
{
	QString filePath = url().path();
	if(filePath.isEmpty()) {
		KTempFile f( QString::null, ext );
		(*f.textStream()) <<  m_doc->text();
		f.close();
		DocManager::self()->associateDocument( f.name(), this );
		return f.name();
	}

	if(isModified()) fileSave();

	return filePath;
}


void TextDocument::slotConvertTo( int target )
{
	switch ( (ConvertToTarget)target )
	{
		case TextDocument::MicrobeOutput:
			break;
		case TextDocument::AssemblyOutput:
			convertToAssembly();
			break;
		case TextDocument::HexOutput:
			convertToHex();
			break;
		case TextDocument::PICOutput:
			convertToPIC();
			break;
	}
}

void TextDocument::convertToAssembly()
{
	QString filePath;
	bool showPICSelect = false;
	ProcessOptions::ProcessPath::MediaType toType;

	if ( m_guessedCodeType == TextDocument::ct_microbe ) {
		toType = ProcessOptions::ProcessPath::AssemblyAbsolute;
		filePath = outputFilePath(".microbe");
	} else if ( m_guessedCodeType == TextDocument::ct_hex ) {
		toType = ProcessOptions::ProcessPath::Disassembly;
		filePath = outputFilePath(".hex");
	} else if ( m_guessedCodeType == TextDocument::ct_c ) {
		toType = ProcessOptions::ProcessPath::AssemblyRelocatable;
		filePath = outputFilePath(".c");
		showPICSelect = true;
	} else {
		kdError() << "Could not get file type for converting to assembly!"<<endl;
		return;
	}
	
	OutputMethodDlg dlg( i18n("Assembly Code Output"), url(), showPICSelect, p_ktechlab );
	
	if ( m_guessedCodeType == TextDocument::ct_c )
		dlg.microSelect()->setAllowedAsmSet( AsmInfo::PIC14 | AsmInfo::PIC16 );
	
	dlg.setOutputExtension(".asm");
	dlg.setFilter("*.asm *.src *.inc|Assembly Code (*.asm, *.src, *.inc)\n*|All Files");
	dlg.exec();
	if (!dlg.isAccepted()) return;
	
	ProcessOptions o( dlg.info() );
	o.setTextOutputTarget( m_pLastTextOutputTarget, this, SLOT(setLastTextOutputTarget( TextDocument* )) );
	o.setInputFiles(filePath);
	o.setProcessPath( ProcessOptions::ProcessPath::path( ProcessOptions::guessMediaType(filePath), toType ) );
	LanguageManager::self()->compile(o);
}


void TextDocument::convertToHex()
{
	QString filePath;
	bool showPICSelect = false;

	if ( m_guessedCodeType == TextDocument::ct_microbe )
		filePath = outputFilePath(".microbe");
	else if ( m_guessedCodeType == TextDocument::ct_asm )
		filePath = outputFilePath(".asm");
	else if ( m_guessedCodeType == TextDocument::ct_c ) {
		filePath = outputFilePath(".c");
		showPICSelect = true;
	} else {
		kdError() << "Could not get file type for converting to hex!"<<endl;
		return;
	}
	
	OutputMethodDlg dlg( i18n("Hex Code Output"), url(), showPICSelect, p_ktechlab );
	dlg.setOutputExtension(".hex");
	dlg.setFilter("*.hex|Hex (*.hex)\n*|All Files");
	
	if ( m_guessedCodeType == TextDocument::ct_c )
		dlg.microSelect()->setAllowedAsmSet( AsmInfo::PIC14 | AsmInfo::PIC16 );
	
	dlg.exec();
	if (!dlg.isAccepted())
		return;
	
	ProcessOptions o( dlg.info() );
	o.setTextOutputTarget( m_pLastTextOutputTarget, this, SLOT(setLastTextOutputTarget( TextDocument* )) );
	o.setInputFiles(filePath);
	o.setProcessPath( ProcessOptions::ProcessPath::path( ProcessOptions::guessMediaType(filePath), ProcessOptions::ProcessPath::Program ) );
	LanguageManager::self()->compile(o);
}

void TextDocument::convertToPIC()
{
	QString filePath;
	
	QString picID;
	
	switch ( m_guessedCodeType )
	{
		case ct_microbe:
			filePath = outputFilePath(".microbe");
			break;
			
		case ct_asm:
		{
			filePath = outputFilePath(".asm");
			AsmParser p( filePath );
			p.parse();
			picID = p.picID();
			break;
		}
			
		case ct_c:
			filePath = outputFilePath(".c");
			break;
	
		case ct_hex:
			filePath = outputFilePath(".hex");
			break;
			
		case ct_unknown:
			kdError() << "Could not get file type for converting to hex!"<<endl;
			return;
	}
	
	ProgrammerDlg * dlg = new ProgrammerDlg( picID, (QWidget*)p_ktechlab, "Programmer Dlg" );
	
	if ( m_guessedCodeType == TextDocument::ct_c )
		dlg->microSelect()->setAllowedAsmSet( AsmInfo::PIC14 | AsmInfo::PIC16 );	

	dlg->exec();
	if ( !dlg->isAccepted() ) {
		dlg->deleteLater();
		return;
	}
	
	ProcessOptions o;
	dlg->initOptions( & o );
	o.setInputFiles( filePath );
	o.setProcessPath( ProcessOptions::ProcessPath::path( ProcessOptions::guessMediaType(filePath), ProcessOptions::ProcessPath::Pic ) );
	LanguageManager::self()->compile( o );
	
	dlg->deleteLater();
}

void TextDocument::print()
{
	KTextEditor::printInterface(m_doc)->print ();
}

void TextDocument::slotSelectionmChanged()
{
	p_ktechlab->action( "edit_cut" )->setEnabled( m_doc->hasSelection () );
	p_ktechlab->action( "edit_copy" )->setEnabled( m_doc->hasSelection () );
}

IntList TextDocument::bookmarkList() const
{
	IntList bookmarkList;
	
	typedef QPtrList<KTextEditor::Mark> MarkList;
	MarkList markList = m_doc->marks();
	
	// Find out what marks need adding to our internal lists
	for ( KTextEditor::Mark * mark = markList.first(); mark; mark = markList.next() ) {
		if ( mark->type & Bookmark )
			bookmarkList += mark->line;
	}
	return bookmarkList;
}

void TextDocument::slotUpdateMarksInfo()
{
	if ( activeView() )
		textView()->slotUpdateMarksInfo();

#ifndef NO_GPSIM
	syncBreakpoints();
#endif
	
	// Update our list of bookmarks in the menu
	p_ktechlab->unplugActionList("bookmark_actionlist");
	m_bookmarkActions.clear();
	
	QPtrList<KTextEditor::Mark> markList = m_doc->marks();
	
	// Find out what marks need adding to our internal lists
	for ( KTextEditor::Mark * mark = markList.first(); mark; mark = markList.next() ) {
		if ( mark->type & Bookmark ) {
			KAction * a = new KAction( i18n("%1 - %2").arg( QString::number( mark->line+1 ) ).arg( m_doc->textLine(mark->line) ),
									   0, this, SLOT(slotBookmarkRequested()), this,
									   QString("bookmark_%1").arg(QString::number(mark->line).ascii()) );
			m_bookmarkActions.append(a);
		}
	}

	p_ktechlab->plugActionList( "bookmark_actionlist", m_bookmarkActions );
}

void TextDocument::slotBookmarkRequested()
{
	const QObject * s = sender();
	if (!s) return;

	QString name = s->name();
	if(!name.startsWith("bookmark_")) return;

	name.remove("bookmark_");
	int line = -1;
	bool ok;
	line = name.toInt(&ok);
	if ( ok && line >= 0 && activeView() )
		(static_cast<TextView*>(activeView()))->gotoLine(line);
}

void TextDocument::setBookmarks( const IntList &lines )
{
	clearBookmarks();
	const IntList::const_iterator end = lines.end();
	for ( IntList::const_iterator it = lines.begin(); it != end; ++it )
		setBookmark( *it, true );
}

void TextDocument::clearBookmarks()
{
	QPtrList<KTextEditor::Mark> markList = m_doc->marks();
	
	// Find out what marks need adding to our internal lists
	for ( KTextEditor::Mark * mark = markList.first(); mark; mark = markList.next() ) {
		if ( mark->type & Bookmark )
			m_doc->removeMark( mark->line, Bookmark );
	}

	slotUpdateMarksInfo();
}

void TextDocument::setBookmark( uint line, bool isBookmark )
{
	if (isBookmark)
		m_doc->addMark( line, Bookmark );
	else m_doc->removeMark( line, Bookmark );
}

void TextDocument::setBreakpoints( const IntList &lines )
{
#ifndef NO_GPSIM
	clearBreakpoints();
	const IntList::const_iterator end = lines.end();
	for ( IntList::const_iterator it = lines.begin(); it != end; ++it )
		setBreakpoint( *it, true );
#endif // !NO_GPSIM
}

IntList TextDocument::breakpointList() const
{
	IntList breakpointList;
#ifndef NO_GPSIM
	typedef QPtrList<KTextEditor::Mark> MarkList;
	MarkList markList = m_doc->marks();

	// Find out what marks need adding to our internal lists
	for ( KTextEditor::Mark * mark = markList.first(); mark; mark = markList.next() ) {
		if ( mark->type & Breakpoint )
			breakpointList += mark->line;
	}
#endif // !NO_GPSIM

	return breakpointList;
}


void TextDocument::setBreakpoint( uint line, bool isBreakpoint )
{
#ifndef NO_GPSIM
	if (isBreakpoint) {
		m_doc->addMark( line, Breakpoint );
		if (m_pDebugger)
			m_pDebugger->setBreakpoint( m_debugFile, line, true );
	} else {
		m_doc->removeMark( line, Breakpoint );
		if (m_pDebugger)
			m_pDebugger->setBreakpoint( m_debugFile, line, false );
	}
#endif // !NO_GPSIM
}


void TextDocument::debugRun()
{
#ifndef NO_GPSIM
	if (m_pDebugger) {
		m_pDebugger->gpsim()->setRunning(true);
		slotInitDebugActions();
		return;
	}
	
	switch ( guessedCodeType() )
	{
		case ct_unknown:
			KMessageBox::sorry( 0, i18n("Unknown code type."), i18n("Cannot debug") );
			return;
			
		case ct_hex:
			KMessageBox::sorry( 0, i18n("Cannot debug hex."), i18n("Cannot debug") );
			return;
			
		case ct_microbe:
			m_bLoadDebuggerAsHLL = true;
			m_debugFile = outputFilePath(".microbe");
			break;
			
		case ct_asm:
			m_bLoadDebuggerAsHLL = false;
			m_debugFile = outputFilePath(".asm");
			break;
			
		case ct_c:
			m_bLoadDebuggerAsHLL = true;
			m_debugFile = outputFilePath(".c");
			break;
	}
	
	m_symbolFile = GpsimProcessor::generateSymbolFile( m_debugFile, this, SLOT(slotCODCreationSucceeded()), SLOT(slotCODCreationFailed()) );
#endif // !NO_GPSIM
}

void TextDocument::debugInterrupt()
{
#ifndef NO_GPSIM
	if (!m_pDebugger) return;

	m_pDebugger->gpsim()->setRunning(false);
	slotInitDebugActions();
#endif // !NO_GPSIM
}

void TextDocument::debugStop()
{
#ifndef NO_GPSIM
	if(!m_pDebugger || !m_bOwnDebugger) return;
	
	m_pDebugger->gpsim()->deleteLater();
	m_pDebugger = 0;
	slotDebugSetCurrentLine( SourceLine() );
	slotInitDebugActions();
#endif // !NO_GPSIM
}

void TextDocument::debugStep()
{
#ifndef NO_GPSIM
	if (!m_pDebugger) return;
	
	m_pDebugger->stepInto();
#endif // !NO_GPSIM
}


void TextDocument::debugStepOver()
{
#ifndef NO_GPSIM
	if (!m_pDebugger) return;

	m_pDebugger->stepOver();
#endif // !NO_GPSIM
}


void TextDocument::debugStepOut()
{
#ifndef NO_GPSIM
	if (!m_pDebugger) return;
	
	m_pDebugger->stepOut();
#endif // !NO_GPSIM
}


void TextDocument::slotDebugSetCurrentLine( const SourceLine & line )
{
#ifndef NO_GPSIM
	int textLine = line.line();
	
	if ( DocManager::self()->findDocument( line.fileName() ) != this )
		textLine = -1;
	
	m_doc->removeMark( m_lastDebugLineAt, ExecutionPoint );
	m_doc->addMark( textLine, ExecutionPoint );
	
	if ( activeView() )
		textView()->setCursorPosition( textLine, 0 );

	m_lastDebugLineAt = textLine;
#endif // !NO_GPSIM
}


void TextDocument::slotInitDebugActions()
{
#ifndef NO_GPSIM
	if ( m_pDebugger )
	{
		if ( m_pDebugger->gpsim()->isRunning() )
			slotDebugSetCurrentLine( SourceLine() );
		else slotDebugSetCurrentLine( m_pDebugger->currentLine() );
	}
	
	if ( activeView() )
		textView()->slotInitDebugActions();
#endif // !NO_GPSIM
}

void TextDocument::slotCODCreationSucceeded()
{
#ifndef NO_GPSIM
	GpsimProcessor * gpsim = new GpsimProcessor( m_symbolFile, this );
	
	if (m_bLoadDebuggerAsHLL)
		gpsim->setDebugMode( GpsimDebugger::HLLDebugger );
	else gpsim->setDebugMode( GpsimDebugger::AsmDebugger );
	
	setDebugger( gpsim->currentDebugger(), true );
#endif // !NO_GPSIM
}

void TextDocument::slotCODCreationFailed()
{
#ifndef NO_GPSIM
	m_debugFile = QString::null;
	m_symbolFile = QString::null;
#endif // !NO_GPSIM
}

void TextDocument::slotDebuggerDestroyed()
{
#ifndef NO_GPSIM
	slotDebugSetCurrentLine( SourceLine() );
	m_pDebugger = 0;
	m_debugFile = QString::null;
	slotInitDebugActions();
#endif // !NO_GPSIM
}

#ifndef NO_GPSIM
void TextDocument::clearBreakpoints()
{
	QPtrList<KTextEditor::Mark> markList = m_doc->marks();

	// Find out what marks need adding to our internal lists
	for(KTextEditor::Mark * mark = markList.first(); mark; mark = markList.next()) {
		if ( mark->type & Bookmark )
			m_doc->removeMark( mark->line, Breakpoint );
	}

	slotUpdateMarksInfo();
}

void TextDocument::syncBreakpoints()
{
	if (b_lockSyncBreakpoints) return;

	// We don't really care about synching marks if we aren't debugging / aren't able to take use of the marks
	if (!m_pDebugger) return;

	b_lockSyncBreakpoints = true;

	typedef QPtrList<KTextEditor::Mark> MarkList;
	MarkList markList = m_doc->marks();
	IntList bpList;

	// Find out what marks need adding to our internal lists
	for ( KTextEditor::Mark * mark = markList.first(); mark; mark = markList.next() ) {
		const int line = mark->line;
		
		if ( mark->type & Breakpoint )
			bpList.append(line);
		
		if ( mark->type == ExecutionPoint )
			m_lastDebugLineAt = line;
	}

	m_pDebugger->setBreakpoints( m_debugFile, bpList );
	b_lockSyncBreakpoints = false;
}

bool TextDocument::debuggerIsRunning() const
{
	return m_pDebugger;
}

bool TextDocument::debuggerIsStepping() const
{
	return m_pDebugger && !m_pDebugger->gpsim()->isRunning();
}

void TextDocument::setDebugger(GpsimDebugger *debugger, bool ownDebugger )
{
	if(debugger == m_pDebugger) return;
	
	// If we create a gpsim, then we may get called by DebugManager, which will
	// try to claim we don't own it. So if we have a symbol file waiting, thne
	// wait until we are called from its successful creation
	if ( !m_symbolFile.isEmpty() && !ownDebugger ) return;
	
	// Reset it for use next time
	m_symbolFile = QString::null;
	
	if(m_bOwnDebugger) delete m_pDebugger;
	m_pDebugger = debugger;
	m_bOwnDebugger = ownDebugger;

	if(!m_pDebugger) return;

	if(m_debugFile.isEmpty()) m_debugFile = url().path();
	
	connect( m_pDebugger,			SIGNAL(destroyed()),						this, SLOT(slotDebuggerDestroyed()) );
	connect( m_pDebugger->gpsim(),	SIGNAL(runningStatusChanged(bool )), 		this, SLOT(slotInitDebugActions()) );
	connect( m_pDebugger,			SIGNAL(lineReached(const SourceLine &)),	this, SLOT(slotDebugSetCurrentLine(const SourceLine &)) );
	m_pDebugger->setBreakpoints( m_debugFile, breakpointList() );
	
	slotInitDebugActions();
	if ( !m_pDebugger->gpsim()->isRunning() )
		slotDebugSetCurrentLine( m_pDebugger->currentLine() );
	
	if ( this == dynamic_cast<TextDocument*>(DocManager::self()->getFocusedDocument()) )
		SymbolViewer::self()->setContext( m_pDebugger->gpsim() );
}
#endif // !NO_GPSIM


const QPixmap* TextDocument::inactiveBreakpointPixmap()
{
	const char*breakpoint_gr_xpm[]={
		"11 16 6 1",
		"c c #c6c6c6",
		"d c #2c2c2c",
		"# c #000000",
		". c None",
		"a c #ffffff",
		"b c #555555",
		"...........",
		"...........",
		"...#####...",
		"..#aaaaa#..",
		".#abbbbbb#.",
		"#abbbbbbbb#",
		"#abcacacbd#",
		"#abbbbbbbb#",
		"#abcacacbd#",
		"#abbbbbbbb#",
		".#bbbbbbb#.",
		"..#bdbdb#..",
		"...#####...",
		"...........",
		"...........",
		"..........."};
		static QPixmap pixmap( breakpoint_gr_xpm );
		return &pixmap;
}


const QPixmap* TextDocument::activeBreakpointPixmap()
{
	const char* breakpoint_xpm[]={
		"11 16 6 1",
		"c c #c6c6c6",
		". c None",
		"# c #000000",
		"d c #840000",
		"a c #ffffff",
		"b c #ff0000",
		"...........",
		"...........",
		"...#####...",
		"..#aaaaa#..",
		".#abbbbbb#.",
		"#abbbbbbbb#",
		"#abcacacbd#",
		"#abbbbbbbb#",
		"#abcacacbd#",
		"#abbbbbbbb#",
		".#bbbbbbb#.",
		"..#bdbdb#..",
		"...#####...",
		"...........",
		"...........",
		"..........."};
		static QPixmap pixmap( breakpoint_xpm );
		return &pixmap;
}



const QPixmap* TextDocument::reachedBreakpointPixmap()
{
	const char*breakpoint_bl_xpm[]={
		"11 16 7 1",
		"a c #c0c0ff",
		"# c #000000",
		"c c #0000c0",
		"e c #0000ff",
		"b c #dcdcdc",
		"d c #ffffff",
		". c None",
		"...........",
		"...........",
		"...#####...",
		"..#ababa#..",
		".#bcccccc#.",
		"#acccccccc#",
		"#bcadadace#",
		"#acccccccc#",
		"#bcadadace#",
		"#acccccccc#",
		".#ccccccc#.",
		"..#cecec#..",
		"...#####...",
		"...........",
		"...........",
		"..........."};
		static QPixmap pixmap( breakpoint_bl_xpm );
		return &pixmap;
}


const QPixmap* TextDocument::disabledBreakpointPixmap()
{
	const char*breakpoint_wh_xpm[]={
		"11 16 7 1",
		"a c #c0c0ff",
		"# c #000000",
		"c c #0000c0",
		"e c #0000ff",
		"b c #dcdcdc",
		"d c #ffffff",
		". c None",
		"...........",
		"...........",
		"...#####...",
		"..#ddddd#..",
		".#ddddddd#.",
		"#ddddddddd#",
		"#ddddddddd#",
		"#ddddddddd#",
		"#ddddddddd#",
		"#ddddddddd#",
		".#ddddddd#.",
		"..#ddddd#..",
		"...#####...",
		"...........",
		"...........",
		"..........."};
		static QPixmap pixmap( breakpoint_wh_xpm );
		return &pixmap;
}


const QPixmap* TextDocument::executionPointPixmap()
{
	const char*exec_xpm[]={
		"11 16 4 1",
		"a c #00ff00",
		"b c #000000",
		". c None",
		"# c #00c000",
		"...........",
		"...........",
		"...........",
		"#a.........",
		"#aaa.......",
		"#aaaaa.....",
		"#aaaaaaa...",
		"#aaaaaaaaa.",
		"#aaaaaaa#b.",
		"#aaaaa#b...",
		"#aaa#b.....",
		"#a#b.......",
		"#b.........",
		"...........",
		"...........",
		"..........."};
		static QPixmap pixmap( exec_xpm );
		return &pixmap;
}

#include "textdocument.moc"
