/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "view.h"

#include <QPointer>
#include <QUrl>

class DCOPObject;
class Document;
class DocumentIface;
class KTechlab;
class View;
class ViewContainer;

typedef QList<QPointer<View>> ViewList;

/**
@author David Saxton
*/
class Document : public QObject
{
    Q_OBJECT
public:
    enum DocumentType {
        dt_none, // Used to denote document type not known / specified / etc, when appropriate
        dt_flowcode,
        dt_circuit,
        dt_mechanics,
        dt_text,
        dt_pinMapEditor
    };
    Document(const QString &caption);
    ~Document() override;
    /**
     * If the user has created a new document from the new file dialog, and
     * wants to add it to the project, then this must wait until this file is
     * given a url. Set this to true to add the file to the active project when
     * it is first saved.
     */
    void setAddToProjectOnSave(bool add)
    {
        m_bAddToProjectOnSave = add;
    }
    /**
     * Caption of document, e.g. "Untitled 2"
     */
    QString caption() const
    {
        return m_caption;
    }
    /**
     * Set the caption of the document, to be displayed in the tab bar when
     * active
     */
    void setCaption(const QString &caption);
    /**
     * Return the dcop object for this document
     */
    DCOPObject *dcopObject() const;
    /**
     * Returns the dcop suffix for this document - a unique ID for the current
     * app session. DCOP name will be "Document#dcopID()"
     */
    unsigned dcopID() const
    {
        return m_dcopID;
    }
    /**
     * Sets the dcop suffix. The DCOP object for this document will be renamed.
     * @see dcopID
     */
    void setDCOPID(unsigned id);
    /**
     * Returns the active view, which is the last view to be used to edit in
     */
    View *activeView() const
    {
        return m_pActiveView;
    }
    ViewList viewList() const
    {
        return m_viewList;
    }
    /**
     * Returns the type of document.
     * @see Document::DocumentType
     */
    DocumentType type() const
    {
        return m_type;
    }
    /**
     * Returns the number of open views.
     */
    uint numberOfViews() const
    {
        return m_viewList.size();
    }
    /**
     * Create a view that will display the document data. In all reimplemented
     * functions, you must call handleNewView after creating the view, so that
     * the appropriate slots, pointers, etc can all be initialised.
     */
    virtual View *createView(ViewContainer *viewContainer, uint viewAreaId) = 0;
    /**
     * Returns the url of the file that the Document refers to
     */
    const QUrl &url() const
    {
        return m_url;
    }
    /**
     * Prompts the user for a url, with the given types for the filter.
     * If user accepts, returns true, and set the url to the new url.
     */
    bool getURL(const QString &types, const QString &fileExtToEnforce);
    /**
     * Attempts to open a url, and returns true if successful.
     * You must reinherit this function.
     */
    virtual bool openURL(const QUrl &url) = 0;
    /**
     * Sets the url of the file that this Document refers to
     */
    void setURL(const QUrl &url);
    /**
     * Sets whether the file is modified or not. Will emit modifiedStateChanged
     * if state changes. You must emit this signal if you reinherit this
     */
    virtual void setModified(bool modified);
    /**
     * Returns the modification state since last-save.
     */
    virtual bool isModified() const
    {
        return b_modified;
    }
    /**
     * Returns true if undo is available.
     */
    virtual bool isUndoAvailable() const
    {
        return false;
    }
    /**
     * Returns true if redo is available.
     */
    virtual bool isRedoAvailable() const
    {
        return false;
    }
    /**
     * Saves the file to a new name.
     */
    virtual void fileSaveAs() = 0;
    /**
     * Attempts to close the file without saving, prompting the user if the
     * file has been modified. If successful, calls QObject::deleteLater(), and
     * returns true (otherwise returns false).
     */
    virtual bool fileClose();
    /**
     * Saves the file.
     */
    virtual void fileSave() = 0;
    /**
     * Prints the file.
     */
    virtual void print() {};
    /**
     * Cuts whatever is selected.
     */
    virtual void cut() {};
    /**
     * Copies whatever is selected.
     */
    virtual void copy() {};
    /**
     * Attempts to paste whatever is in the clipboard.
     */
    virtual void paste() {};
    /**
     * Undo the last operation. You should reinherit this function.
     */
    virtual void undo() {};
    /**
     * Redo the undone last operation. You should reinherit this function.
     */
    virtual void redo() {};
    /**
     * Selects everything in the view.
     */
    virtual void selectAll() {};

    virtual void convertToMicrobe() {};
    virtual void convertToHex() {};
    virtual void convertToPIC() {};
    virtual void convertToAssembly() {};
    virtual void debugRun() {};
    virtual void debugInterrupt() {};
    virtual void debugStop() {};
    virtual void debugStep() {};
    bool isDeleted() const
    {
        return m_bDeleted;
    }

protected slots:
    /**
     * Called when the user changes the configuration.
     */
    virtual void slotUpdateConfiguration() {};

#define protected public
signals:
    /**
     * Emitted when an operation has been performed that
     * has caused the stack of available undo/redo operations to
     * have changed
     */
    void undoRedoStateChanged();
#undef protected

signals:
    /**
     * Emitted when the Document goes from modified to unmodified,
     * or vice-versa
     */
    void modifiedStateChanged();
    /**
     * Emitted when the name of the file that the Document refers to
     * is changed.
     */
    void fileNameChanged(const QUrl &url);

    void viewFocused(View *view);
    void viewUnfocused();

private slots:
    void slotViewDestroyed(QObject *obj);
    void slotViewFocused(View *view);

protected:
    /**
     * You must call this function after creating a new view
     */
    virtual void handleNewView(View *view);

    bool b_modified;

    // TODO: refactor this out.
    DocumentType m_type;
    // XXXX

    ViewList m_viewList;
    DocumentIface *m_pDocumentIface;

    // Set to true by the document et subclasses destructors, used to avoid
    // doing stuff that might lead to crash when being deleted.
    bool m_bDeleted;

private:
    QUrl m_url;
    QPointer<View> m_pActiveView;
    QString m_caption;
    bool m_bAddToProjectOnSave;
    unsigned m_dcopID;
    unsigned m_nextViewID;
};

#endif
