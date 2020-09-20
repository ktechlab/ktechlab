/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DOCUMENTIFACE_H
#define DOCUMENTIFACE_H

#include "config.h"

//#include <dcopobject.h>
//#include <dcopref.h>

#include "dcop_stub.h"

#include <QStringList>

class CircuitDocument;
class Document;
class FlowCodeDocument;
class ICNDocument;
class ItemDocument;
class MechanicsDocument;
class TextDocument;
class View;

/**
@author David Saxton
*/
class DocumentIface : public DCOPObject // TODO port to dbus
{
    K_DCOP // TODO port to dbus

        public : DocumentIface(Document *document);
    virtual ~DocumentIface();

    k_dcop : // TODO port to dbus
             QString
             caption() const;
    DCOPRef activeView();
    uint numberOfViews();
    // 		View *createView( ViewContainer *viewContainer, uint viewAreaId, const char *name = nullptr );
    QString url();
    bool openURL(const QString &url);
    bool isModified();
    bool isUndoAvailable();
    bool isRedoAvailable();
    void save();
    void saveAs();
    bool close();
    void print();
    void cut();
    void copy();
    void paste();
    void undo();
    void redo();
    void selectAll();

protected:
    DCOPRef viewToRef(View *view);

    Document *m_pDocument;
};

class TextDocumentIface : public DocumentIface
{
    // K_DCOP TODO port to dbus

public:
    TextDocumentIface(TextDocument *document);

    // k_dcop: TODO port to dbus
    void formatAssembly();
    void convertToMicrobe();
    void convertToHex();
    void convertToPIC();
    void convertToAssembly();
    void clearBookmarks();
    bool isDebugging();
    void debugRun();
    void debugInterrupt();
    void debugStop();
    void debugStep();
    void debugStepOver();
    void debugStepOut();

protected:
    TextDocument *m_pTextDocument;
};

class ItemDocumentIface : public DocumentIface
{
    // K_DCOP TODO port to dbus

public:
    ItemDocumentIface(ItemDocument *document);

    // k_dcop: TODO port to dbus
    QStringList validItemIDs();
    /**
     * Create an item with the given id (e.g. "ec/resistor") at the given
     * position.
     * @return name of item (assigned to it by KTechlab)
     */
    QString addItem(const QString &id, int x, int y);
    void selectItem(const QString &id);
    void unselectItem(const QString &id);
    void clearHistory();
    void unselectAll();
    void alignHorizontally();
    void alignVertically();
    void distributeHorizontally();
    void distributeVertically();
    void deleteSelection();

protected:
    ItemDocument *m_pItemDocument;
};

class MechanicsDocumentIface : public ItemDocumentIface
{
    // K_DCOP TODO port to dbus

public:
    MechanicsDocumentIface(MechanicsDocument *document);

protected:
    MechanicsDocument *m_pMechanicsDocument;
};

class ICNDocumentIface : public ItemDocumentIface
{
    // K_DCOP TODO port to dbus

public:
    ICNDocumentIface(ICNDocument *document);

    // k_dcop:  TODO port to dbus
    void exportToImage();
    QStringList nodeIDs(const QString &id);
    /**
     * Makes a connection from node1 on item1 to node2 on item2
     */
    QString makeConnection(const QString &item1, const QString &node1, const QString &item2, const QString &node2);
    void selectConnector(const QString &id);
    void unselectConnector(const QString &id);

protected:
    ICNDocument *m_pICNDocument;
};

// FIXME: move to separate file and put in same path as circuitdocument.*
class CircuitDocumentIface : public ICNDocumentIface
{
    // K_DCOP TODO port to dbus

public:
    CircuitDocumentIface(CircuitDocument *document);

    // k_dcop: TODO port to dbus
    void setOrientation0();
    void setOrientation90();
    void setOrientation180();
    void setOrientation270();
    void rotateCounterClockwise();
    void rotateClockwise();
    void flipHorizontally();
    void flipVertically();
    void displayEquations();
    void createSubcircuit();

protected:
    CircuitDocument *m_pCircuitDocument;
};

class FlowCodeDocumentIface : public ICNDocumentIface
{
    // K_DCOP TODO port to dbus

public:
    FlowCodeDocumentIface(FlowCodeDocument *document);
    void convertToMicrobe();
    void convertToHex();
    void convertToPIC();
    void convertToAssembly();

    // k_dcop: TODO port to dbus
    void setPicType(const QString &id);

protected:
    FlowCodeDocument *m_pFlowCodeDocument;
};

#endif
