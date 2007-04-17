/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef TEXTDOCUMENT_H
#define TEXTDOCUMENT_H

#include "config.h"
#include "document.h"

#include <qguardedptr.h>

#include <kate/document.h>

class GpsimDebugger;
class SourceLine;
class TextView;

typedef QValueList<int> IntList;

/**
@author David Saxton
*/
class TextDocument : public Document
{
Q_OBJECT
public:
	~TextDocument();

	enum CodeType
	{
		ct_unknown,
		ct_asm,
		ct_c,
		ct_hex,
		ct_microbe
	};
	
	enum MarkType {
		Bookmark           = KTextEditor::MarkInterface::markType01,
		Breakpoint         = KTextEditor::MarkInterface::markType02,
		ActiveBreakpoint   = KTextEditor::MarkInterface::markType03,
		ReachedBreakpoint  = KTextEditor::MarkInterface::markType04,
		DisabledBreakpoint = KTextEditor::MarkInterface::markType05,
		ExecutionPoint     = KTextEditor::MarkInterface::markType06
	};
	
	virtual View *createView( ViewContainer *viewContainer, uint viewAreaId, const char *name = 0l );

	/**
	 * Attempts to construct a new TextDocument object and returns a pointer to
	 * it if successful, or 0 if it failed.
	 * @returns pointer to constructed object, or 0 if there was a problem
	 */
	static TextDocument *constructTextDocument( const QString& caption, const char *name = 0L );
	/**
	 * @returns the guessed code type that this file is
	 */
	CodeType guessedCodeType() const { return m_guessedCodeType; }
	/**
	 * Set the given lines as all bookmarks
	 */
	void setBookmarks( const IntList &lines );
	/**
	 * Set the given line to a bookmark (or not)
	 */
	void setBookmark( uint line, bool isBookmark );
	/**
	 * @return List of bookmarks
	 */
	IntList bookmarkList() const;
	
	/**
	 * Set the given lines as all breakpoints
	 */
	void setBreakpoints( const IntList &lines );
	/**
	 * Set the given line to a breakpoint (or not )
	 */
	void setBreakpoint( uint line, bool isBreakpoint );
	/**
	 * @return List of breakpoints
	 */
	IntList breakpointList() const;
	
#ifndef NO_GPSIM
	/**
	 * Attach ourselves to the given debugger.
	 * @param ownDebugger whether we have permission to delete it.
	 */
	void setDebugger( GpsimDebugger * debugger, bool ownDebugger );
	GpsimDebugger * debugger() const { return m_pDebugger; }
	bool ownDebugger() const { return m_bOwnDebugger; }
	/**
	 * Returns true if the debugger is running (this includes when we are in
	 * stepping mode)
	 */
	bool debuggerIsRunning() const;
	/**
	 * Returns true if we are in stepping more
	 */
	bool debuggerIsStepping() const;
	QString debugFile() const { return m_debugFile; }
	virtual void clearBreakpoints();
#endif
	
	virtual bool openURL(const KURL& url);
	void fileSave(const KURL& url);
	/**
	 * Set the document to the given text, making the document unmodified, and
	 * reseting the undo/redo history/
	 * @param asInitial whether the next should be treated as if we had just
	 * opened the file (no undo/redo history, and unmodified).
	 */
	void setText( const QString & text, bool asInitial );
	/**
	 * Attempts to guess the filetype from the file extension, and load an
	 * appropriate highlighting/etc
	 * @param allowDisable If false, will simply keep the old scheme if nothing
	 *					   appropriate is found
	 */
	void guessScheme( bool allowDisable = true );
    
	virtual void fileSave() { fileSave(url()); }
	virtual void fileSaveAs();
	virtual void print();
	virtual void setModified( bool modified );
	
	Kate::View* createKateView( QWidget *parent, const char *name = 0l );
	
	virtual void undo();
	virtual void redo();
	virtual void cut();
	virtual void copy();
	virtual void paste();
	
	virtual bool isModified() const { return m_doc->isModified(); }
	virtual bool isUndoAvailable() const { return (m_doc->undoCount() != 0); }
	virtual bool isRedoAvailable() const { return (m_doc->redoCount() != 0); }

	void clearBookmarks();
	virtual bool fileClose();

	static const QPixmap* inactiveBreakpointPixmap();
	static const QPixmap* activeBreakpointPixmap();
	static const QPixmap* reachedBreakpointPixmap();
	static const QPixmap* disabledBreakpointPixmap();
	static const QPixmap* executionPointPixmap();
	/**
	 * Returns a TextView pointer to the active TextView (if there is one)
	 */
	TextView *textView() const;

	enum ConvertToTarget
	{
		MicrobeOutput, // (not used)
		AssemblyOutput,
		HexOutput,
		PICOutput
	};
	
	Kate::Document * kateDocument() const { return m_doc; }
	
public slots:
	/**
	 * @param target as ConvertToTarget
	 */
	void slotConvertTo( int target );
	void convertToAssembly();
	void convertToHex();
	void convertToPIC();
	void formatAssembly();
	void debugRun();
	void debugInterrupt();
	void debugStep();
	void debugStepOver();
	void debugStepOut();
	void debugStop();
	void slotInitLanguage( CodeType type );
	/**
	 * Called when change line / toggle marks
	 */
	void slotUpdateMarksInfo();

	void slotDebugSetCurrentLine( const SourceLine & line );
	/**
	 * Initialize the actions appropriate for when the debugger is running
	 * or stepping
	 */
	void slotInitDebugActions();

protected:
	/**
	 * Returns a filepath with the editor's contents in. If the url of this file
	 * is non-empty (i.e. the user has already saved the file), then that url is
	 * returned. Otherwise, a temporary file with the given extension (ext) is
	 * created, and the location of this file is returned.
	 */
	QString outputFilePath( const QString &ext );
	void saveDone();
#ifndef NO_GPSIM
	/**
	 * Looks at the list of marks returned by Kate, and syncs them with the
	 * marks that we know about
	 */
	void syncBreakpoints();
	
	int m_lastDebugLineAt; // Last line with a debug point reached mark
	bool m_bLoadDebuggerAsHLL;
#endif

	Kate::Document *m_doc;
	QGuardedPtr<TextDocument> m_pLastTextOutputTarget;
	
private slots:
	void setLastTextOutputTarget( TextDocument * target );
	void slotSyncModifiedStates();
	void slotCODCreationSucceeded();
	void slotCODCreationFailed();
	void slotDebuggerDestroyed();
	void slotBookmarkRequested();
	void slotSelectionmChanged();

private:
	TextDocument( const QString& caption, const char *name = 0L );
	bool m_constructorSuccessful;
	CodeType m_guessedCodeType;
	QPtrList<KAction> m_bookmarkActions;
	
#ifndef NO_GPSIM
	bool b_lockSyncBreakpoints; // Used to avoid calling syncMarks() when we are currently doing so
	bool m_bOwnDebugger;
	QGuardedPtr<GpsimDebugger> m_pDebugger;
	QString m_symbolFile;
	QString m_debugFile;
#endif
};

#endif
