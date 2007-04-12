/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#define protected public
#include <kxmlguiclient.h>
#undef protected

#include "asmformatter.h"
#include "config.h"
#include "filemetainfo.h"
#include "gpsimprocessor.h"
#include "ktechlab.h"
#include "symbolviewer.h"
#include "textdocument.h"
#include "textview.h"
#include "variablelabel.h"
#include "viewiface.h"

#include <ktexteditor/editinterface.h>
#include <ktexteditor/texthintinterface.h>

// #include "kateview.h"
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>

#include <qapplication.h> 
#include <qcursor.h>
#include <qobjectlist.h>
#include <qtimer.h>


//BEGIN class TextView
TextView::TextView( TextDocument * textDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name )
	: View( textDocument, viewContainer, viewAreaId, name )
{
	m_view = textDocument->createKateView(this);
	m_view->insertChildClient(this);
	
	KActionCollection * ac = actionCollection();
	
	
	//BEGIN Convert To * Actions
	KToolBarPopupAction * pa = new KToolBarPopupAction( i18n("Convert to"), "fork", 0, 0, 0, ac, "program_convert" );
	pa->setDelayed(false);
	
	KPopupMenu * m = pa->popupMenu();
	
	m->insertTitle( i18n("Convert to ...") );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "convert_to_microbe", KIcon::Small ), i18n("Microbe"), TextDocument::MicrobeOutput );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "convert_to_assembly", KIcon::Small ), i18n("Assembly"), TextDocument::AssemblyOutput );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "convert_to_hex", KIcon::Small ), i18n("Hex"), TextDocument::HexOutput );
	m->insertItem( KGlobal::iconLoader()->loadIcon( "convert_to_pic", KIcon::Small ), i18n("PIC (upload)"), TextDocument::PICOutput );
	connect( m, SIGNAL(activated(int)), textDocument, SLOT(slotConvertTo(int)) );
	
	m->setItemEnabled( TextDocument::MicrobeOutput, false );
	//END Convert To * Actions
	
	
	new KAction( i18n("Format Assembly Code"), "", Qt::Key_F12, textDocument, SLOT(formatAssembly()), ac, "format_asm" );
	
	
#ifndef NO_GPSIM
	//BEGIN Debug Actions
	new KAction( i18n("Set &Breakpoint"), 0, 0, this, SLOT(toggleBreakpoint()), ac, "debug_toggle_breakpoint" );
	new KAction( i18n("Run"), "dbgrun", 0, textDocument, SLOT(debugRun()), ac, "debug_run" );
	new KAction( i18n("Interrupt"), "player_pause", 0, textDocument, SLOT(debugInterrupt()), ac, "debug_interrupt" );
	new KAction( i18n("Stop"), "stop", 0, textDocument, SLOT(debugStop()), ac, "debug_stop" );
	new KAction( i18n("Step"), "dbgstep", Qt::CTRL|Qt::ALT|Qt::Key_Right, textDocument, SLOT(debugStep()), ac, "debug_step" );
	new KAction( i18n("Step Over"), "dbgnext", 0, textDocument, SLOT(debugStepOver()), ac, "debug_step_over" );
	new KAction( i18n("Step Out"), "dbgstepout", 0, textDocument, SLOT(debugStepOut()), ac, "debug_step_out" );
	//END Debug Actions
#endif

	
	setXMLFile( "ktechlabtextui.rc" );
	m_view->setXMLFile( locate( "appdata", "ktechlabkateui.rc" ) );
	
	m_savedCursorLine = 0;
	m_savedCursorColumn = 0;
	m_pViewIface = new TextViewIface(this);
	
	setAcceptDrops(true);
	
	m_statusBar->insertItem( "", ViewStatusBar::LineCol );
	
	m_view->installPopup( static_cast<QPopupMenu*>( p_ktechlab->factory()->container( "ktexteditor_popup", p_ktechlab ) ) );
		
	connect( m_view, SIGNAL(cursorPositionChanged()),	this, SLOT(slotCursorPositionChanged()) );
	connect( m_view, SIGNAL(gotFocus(Kate::View*)),		this, SLOT(setFocused()) );
	
	m_layout->insertWidget( 0, m_view );
	
	slotCursorPositionChanged();
	slotInitDebugActions();
	initCodeActions();
	
#ifndef NO_GPSIM
	m_pTextViewLabel = new VariableLabel( this );
	m_pTextViewLabel->hide();
	
	TextViewEventFilter * eventFilter = new TextViewEventFilter( this );
	connect( eventFilter, SIGNAL(wordHoveredOver( const QString&, int, int )), this, SLOT(slotWordHoveredOver( const QString&, int, int )) );
	connect( eventFilter, SIGNAL(wordUnhovered()), this, SLOT(slotWordUnhovered()) );
	
	QObject * internalView = m_view->child( 0, "KateViewInternal" );
	internalView->installEventFilter( eventFilter );
#endif
}


TextView::~TextView()
{
	if ( p_ktechlab )
	{
		if ( KXMLGUIFactory * f = m_view->factory() )
			f->removeClient( m_view );
		
		p_ktechlab->addNoRemoveGUIClient( m_view );
	}
	
	delete m_pViewIface;
	m_pViewIface = 0;
}


bool TextView::closeView()
{
	if ( textDocument() )
	{
		const QString path = textDocument()->url().prettyURL();
		if ( !path.isEmpty() )
			fileMetaInfo()->grabMetaInfo( path, this );
	}
	
	bool doClose = View::closeView();
	if (doClose)
		p_ktechlab->factory()->removeClient(m_view);
	return View::closeView();
}


TextDocument *TextView::textDocument() const
{
	return static_cast<TextDocument*>(document());
}


void TextView::disableActions()
{
	KPopupMenu * tb = (dynamic_cast<KToolBarPopupAction*>(action("program_convert")))->popupMenu();
	
	tb->setItemEnabled( TextDocument::AssemblyOutput, false );
	tb->setItemEnabled( TextDocument::HexOutput, false );
	tb->setItemEnabled( TextDocument::PICOutput, false );
	action("format_asm")->setEnabled(false);
	
#ifndef NO_GPSIM
	action("debug_toggle_breakpoint")->setEnabled(false);
#endif
}


void TextView::setFocused()
{
	View::setFocused();
	
#ifndef NO_GPSIM
	GpsimDebugger * debugger = textDocument()->debugger();
	if ( !debugger || !debugger->gpsim() )
		return;
	
	SymbolViewer::self()->setContext( debugger->gpsim() );
#endif
}


void TextView::initCodeActions()
{
	disableActions();
	
	KPopupMenu * tb = (dynamic_cast<KToolBarPopupAction*>(action("program_convert")))->popupMenu();
	
	switch ( textDocument()->guessedCodeType() )
	{
		case TextDocument::ct_asm:
		{
			tb->setItemEnabled( TextDocument::HexOutput, true );
			tb->setItemEnabled( TextDocument::PICOutput, true );
			action("format_asm")->setEnabled(true);
#ifndef NO_GPSIM
			action("debug_toggle_breakpoint")->setEnabled(true);
			slotInitDebugActions();
#endif
			break;
		}
		case TextDocument::ct_c:
		{
			tb->setItemEnabled( TextDocument::AssemblyOutput, true );
			tb->setItemEnabled( TextDocument::HexOutput, true );
			tb->setItemEnabled( TextDocument::PICOutput, true );
			break;
		}
		case TextDocument::ct_hex:
		{
			tb->setItemEnabled( TextDocument::AssemblyOutput, true );
			tb->setItemEnabled( TextDocument::PICOutput, true );
			break;
		}
		case TextDocument::ct_microbe:
		{
			tb->setItemEnabled( TextDocument::AssemblyOutput, true );
			tb->setItemEnabled( TextDocument::HexOutput, true );
			tb->setItemEnabled( TextDocument::PICOutput, true );
			break;
		}
		case TextDocument::ct_unknown:
		{
			break;
		}
	}
}


unsigned TextView::currentLine()
{
	unsigned l,c ;
	m_view->cursorPosition( &l, &c );
	return l;
}
unsigned TextView::currentColumn()
{
	unsigned l,c ;
	m_view->cursorPosition( &l, &c );
	return c;
}


void TextView::saveCursorPosition()
{
	m_view->cursorPosition( &m_savedCursorLine, &m_savedCursorColumn );
}


void TextView::restoreCursorPosition()
{
	m_view->setCursorPosition( m_savedCursorLine, m_savedCursorColumn );
}


void TextView::slotCursorPositionChanged()
{
	uint line, column;
	m_view->cursorPosition( &line, &column );
	
	m_statusBar->changeItem( i18n(" Line: %1 Col: %2 ").arg(QString::number(line+1)).arg(QString::number(column+1)), ViewStatusBar::LineCol );
	
	slotUpdateMarksInfo();
}


void TextView::slotUpdateMarksInfo()
{
#ifndef NO_GPSIM
	uint l,c ;
	m_view->cursorPosition( &l, &c );
	
	if ( m_view->getDoc()->mark(l) & TextDocument::Breakpoint )
		action("debug_toggle_breakpoint")->setText( i18n("Clear &Breakpoint") );
	else
		action("debug_toggle_breakpoint")->setText( i18n("Set &Breakpoint") );
#endif
}


void TextView::slotInitDebugActions()
{
#ifndef NO_GPSIM
	bool isRunning = textDocument()->debuggerIsRunning();
	bool isStepping = textDocument()->debuggerIsStepping();
	bool ownDebugger = textDocument()->ownDebugger();
	
	action("debug_run")->setEnabled( !isRunning || isStepping );
	action("debug_interrupt")->setEnabled(isRunning && !isStepping);
	action("debug_stop")->setEnabled(isRunning && ownDebugger);
	action("debug_step")->setEnabled(isRunning && isStepping);
	action("debug_step_over")->setEnabled(isRunning && isStepping);
	action("debug_step_out")->setEnabled(isRunning && isStepping);
#endif // !NO_GPSIM
}


void TextView::toggleBreakpoint()
{
#ifndef NO_GPSIM
	uint l,c ;
	m_view->cursorPosition( &l, &c );
	textDocument()->setBreakpoint( l, !(m_view->getDoc()->mark(l) & TextDocument::Breakpoint) );
#endif // !NO_GPSIM
}


void TextView::slotWordHoveredOver( const QString & word, int line, int col )
{
#ifndef NO_GPSIM
	// We're only interested in popping something up if we currently have a debugger running
	GpsimProcessor * gpsim = textDocument()->debugger() ? textDocument()->debugger()->gpsim() : 0;
	if ( !gpsim )
	{
		m_pTextViewLabel->hide();
		return;
	}
	
	// Find out if the word that we are hovering over is the operand data
	KTextEditor::EditInterface * e = (KTextEditor::EditInterface*)textDocument()->kateDocument()->qt_cast("KTextEditor::EditInterface");
	InstructionParts parts( e->textLine( unsigned(line) ) );
	if ( !parts.operandData().contains( word ) )
		return;
	
	if ( RegisterInfo * info = gpsim->registerMemory()->fromName( word ) )
		m_pTextViewLabel->setRegister( info, info->name() );
	
	else
	{
		int operandAddress = textDocument()->debugger()->programAddress( textDocument()->debugFile(), line );
		if ( operandAddress == -1 )
		{
			m_pTextViewLabel->hide();
			return;
		}
		
		int regAddress = gpsim->operandRegister( operandAddress );
		
		if ( regAddress != -1 )
			m_pTextViewLabel->setRegister( gpsim->registerMemory()->fromAddress( regAddress ), word );
		
		else
		{
			m_pTextViewLabel->hide();
			return;
		}
	}
	
	m_pTextViewLabel->move( mapFromGlobal( QCursor::pos() ) + QPoint( 0, 20 ) );
	m_pTextViewLabel->show();
#endif // !NO_GPSIM
}


void TextView::slotWordUnhovered()
{
#ifndef NO_GPSIM
	m_pTextViewLabel->hide();
#endif // !NO_GPSIM
}
//END class TextView



//BEGIN class TextViewEventFilter
TextViewEventFilter::TextViewEventFilter( TextView * textView )
{
	m_hoverStatus = Sleeping;
	m_pTextView = textView;
	m_lastLine = m_lastCol = -1;
	
	((KTextEditor::TextHintInterface*)textView->kateView()->qt_cast("KTextEditor::TextHintInterface"))->enableTextHints(0);
	connect( textView->kateView(), SIGNAL(needTextHint(int, int, QString &)), this, SLOT(slotNeedTextHint( int, int, QString& )) );
	
	m_pHoverTimer = new QTimer( this );
	connect( m_pHoverTimer, SIGNAL(timeout()), this, SLOT(hoverTimeout()) );
	
	m_pSleepTimer = new QTimer( this );
	connect( m_pSleepTimer, SIGNAL(timeout()), this, SLOT(gotoSleep()) );
	
	m_pNoWordTimer = new QTimer( this );
	connect( m_pNoWordTimer, SIGNAL(timeout()), this, SLOT(slotNoWordTimeout()) );
}


bool TextViewEventFilter::eventFilter( QObject *, QEvent * e )
{
	if ( e->type() == QEvent::MouseMove )
	{
		if ( !m_pNoWordTimer->isActive() )
			m_pNoWordTimer->start( 10 );
		return false;
	}
	
	if ( e->type() == QEvent::FocusOut || e->type() == QEvent::FocusIn || e->type() == QEvent::MouseButtonPress || e->type() == QEvent::Leave || e->type() == QEvent::Wheel )
	{
		// user moved focus somewhere - hide the tip and sleep
		if ( ((QFocusEvent*)e)->reason() != QFocusEvent::Popup )
			updateHovering( 0, -1, -1 );
	}
	
	return false;
}


void TextViewEventFilter::hoverTimeout()
{
	m_pSleepTimer->stop();
	m_hoverStatus = Active;
	emit wordHoveredOver( m_lastWord, m_lastLine, m_lastCol );
}


void TextViewEventFilter::gotoSleep()
{
	m_hoverStatus = Sleeping;
	m_lastWord = QString::null;
	emit wordUnhovered();
	m_pHoverTimer->stop();
}


void TextViewEventFilter::slotNoWordTimeout()
{
	updateHovering( 0, -1, -1 );
}


void TextViewEventFilter::updateHovering( const QString & currentWord, int line, int col )
{
	if ( (currentWord == m_lastWord) && (line == m_lastLine) )
		return;
	
	m_lastWord = currentWord;
	m_lastLine = line;
	m_lastCol = col;
	
	if ( currentWord.isEmpty() )
	{
		if ( m_hoverStatus == Active )
			m_hoverStatus = Hidden;
		
		emit wordUnhovered();
		if ( !m_pSleepTimer->isActive() )
			m_pSleepTimer->start( 2000, true );
		
		return;
	}
	
	if ( m_hoverStatus != Sleeping )
		emit wordHoveredOver( currentWord, line, col );
	else
		m_pHoverTimer->start( 700, true );
}


static inline bool isWordLetter( const QString & s ) { return (s.length() == 1) && (s[0].isLetterOrNumber() || s[0] == '_'); }


void TextViewEventFilter::slotNeedTextHint( int line, int col, QString & )
{
	m_pNoWordTimer->stop();
	
	KTextEditor::EditInterface * e = (KTextEditor::EditInterface*)m_pTextView->textDocument()->kateDocument()->qt_cast("KTextEditor::EditInterface");
	
	// Return if we aren't currently in a word
	if ( !isWordLetter( e->text( line, col, line, col+1 ) ) )
	{
		updateHovering( QString::null, line, col );
		return;
	}
	
	// Find the start of the word
	int wordStart = col;
	do wordStart--;
	while ( wordStart > 0 && isWordLetter( e->text( line, wordStart, line, wordStart+1 ) ) );
	wordStart++;
	
	// Find the end of the word
	int wordEnd = col;
	do wordEnd++;
	while ( isWordLetter( e->text( line, wordEnd, line, wordEnd+1 ) ) );
	
	QString t = e->text( line, wordStart, line, wordEnd );
	
	updateHovering( t, line, col );
}
//END class TextViewEventFilter



#include "textview.moc"
