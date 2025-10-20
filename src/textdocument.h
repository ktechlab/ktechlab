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
#include "gpsimprocessor.h"

#include <QPointer>
// #include <q3ptrlist.h>

#include <KTextEditor/Document>

class GpsimDebugger;
class SourceLine;
class TextView;

namespace KTextEditor
{
class View;
class Document;
}

typedef QList<int> IntList;

/**
@author David Saxton
*/
class TextDocument : public Document
{
    Q_OBJECT
public:
    ~TextDocument() override;

    enum CodeType { ct_unknown, ct_asm, ct_c, ct_hex, ct_microbe };

    enum MarkType {
        Breakpoint = KTextEditor::Document:: markType10,
    };

    View *createView(ViewContainer *viewContainer, uint viewAreaId) override;

    /**
     * Attempts to construct a new TextDocument object and returns a pointer to
     * it if successful, or 0 if it failed.
     * @returns pointer to constructed object, or 0 if there was a problem
     */
    static TextDocument *constructTextDocument(const QString &caption);
    /**
     * @returns the guessed code type that this file is
     */
    CodeType guessedCodeType() const
    {
        return m_guessedCodeType;
    }
    /**
     * Set the given lines as all bookmarks
     */
    void setBookmarks(const IntList &lines);
    /**
     * Set the given line to a bookmark (or not)
     */
    void setBookmark(uint line, bool isBookmark);
    /**
     * @return List of bookmarks
     */
    IntList bookmarkList() const;

    /**
     * Set the given lines as all breakpoints
     */
    void setBreakpoints(const IntList &lines);
    /**
     * Set the given line to a breakpoint (or not )
     */
    void setBreakpoint(uint line, bool isBreakpoint);
    /**
     * @return List of breakpoints
     */
    IntList breakpointList() const;

#if HAVE_GPSIM
    /**
     * Attach ourselves to the given debugger.
     * @param ownDebugger whether we have permission to delete it.
     */
    void setDebugger(GpsimDebugger *debugger, bool ownDebugger);
    GpsimDebugger *debugger() const
    {
        return m_pDebugger;
    }
    bool ownDebugger() const
    {
        return m_bOwnDebugger;
    }
    /**
     * Returns true if the debugger is running (this includes when we are in
     * stepping mode)
     */
    bool debuggerIsRunning() const;
    /**
     * Returns true if we are in stepping more
     */
    bool debuggerIsStepping() const;
    QString debugFile() const
    {
        return m_debugFile;
    }
    virtual void clearBreakpoints();
#endif

    bool openURL(const QUrl &url) override;

    void fileSave(const QUrl &url);
    /**
     * Set the document to the given text, making the document unmodified, and
     * resetting the undo/redo history/
     * @param asInitial whether the next should be treated as if we had just
     * opened the file (no undo/redo history, and unmodified).
     */
    void setText(const QString &text, bool asInitial);
    /**
     * Attempts to guess the filetype from the file extension, and load an
     * appropriate highlighting/etc
     * @param allowDisable If false, will simply keep the old scheme if nothing
     *					   appropriate is found
     */
    void guessScheme(bool allowDisable = true);

    void fileSave() override
    {
        fileSave(url());
    }
    void fileSaveAs() override;
    void print() override;
    void setModified(bool modified) override;

    KTextEditor::View *createKateView(QWidget *parent);

    void undo() override;
    void redo() override;
    void cut() override;
    void copy() override;
    void paste() override;

    bool isModified() const override
    {
        return m_doc->isModified();
    }
    bool isUndoAvailable() const override; // { return (m_doc->undoCount() != 0); }
    bool isRedoAvailable() const override; // { return (m_doc->redoCount() != 0); }

    void clearBookmarks();
    bool fileClose() override;

    static const QPixmap *inactiveBreakpointPixmap();
    static const QPixmap *activeBreakpointPixmap();
    static const QPixmap *reachedBreakpointPixmap();
    static const QPixmap *disabledBreakpointPixmap();
    static const QPixmap *executionPointPixmap();
    /**
     * Returns a TextView pointer to the active TextView (if there is one)
     */
    TextView *textView() const;

    enum ConvertToTarget {
        MicrobeOutput, // (not used)
        AssemblyOutput,
        HexOutput,
        PICOutput
    };

    KTextEditor::Document *kateDocument() const
    {
        return m_doc;
    }

public Q_SLOTS:
    /**
     * @param target as ConvertToTarget
     */
    void slotConvertTo(QAction *action);
    void convertToAssembly() override;
    void convertToHex() override;
    void convertToPIC() override;
    void formatAssembly();
    void debugRun() override;
    void debugInterrupt() override;
    void debugStep() override;
    void debugStepOver();
    void debugStepOut();
    void debugStop() override;
    void slotInitLanguage(CodeType type);
    /**
     * Called when change line / toggle marks
     */
    void slotUpdateMarksInfo();

    void slotDebugSetCurrentLine(const SourceLine &line);
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
    QString outputFilePath(const QString &ext);
    void saveDone();
#if HAVE_GPSIM
    /**
     * Looks at the list of marks returned by Kate, and syncs them with the
     * marks that we know about
     */
    void syncBreakpoints();

    int m_lastDebugLineAt; // Last line with a debug point reached mark
    bool m_bLoadDebuggerAsHLL;
#endif

    KTextEditor::Document *m_doc;
    QPointer<TextDocument> m_pLastTextOutputTarget;

private Q_SLOTS:
    void setLastTextOutputTarget(TextDocument *target);
    void slotSyncModifiedStates();
    void slotCODCreationSucceeded();
    void slotCODCreationFailed();
    void slotDebuggerDestroyed();
    void slotBookmarkRequested();
    // 	void slotSelectionmChanged(); // 2016.09.08 - moved to TextView

private:
    TextDocument(const QString &caption);
    bool m_constructorSuccessful;
    CodeType m_guessedCodeType;
    QList<QAction *> m_bookmarkActions;

#if HAVE_GPSIM
    bool b_lockSyncBreakpoints; // Used to avoid calling syncMarks() when we are currently doing so
    bool m_bOwnDebugger;
    QPointer<GpsimDebugger> m_pDebugger;
    QString m_symbolFile;
    QString m_debugFile;
#endif
};

#endif
