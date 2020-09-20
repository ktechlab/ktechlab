/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "pinmapping.h"
#include "canvasmanipulator.h"
#include "cnitemgroup.h"
#include "eckeypad.h"
#include "ecsevensegment.h"
#include "libraryitem.h"
#include "microinfo.h"
#include "micropackage.h"
#include "node.h"
#include "viewcontainer.h"

#include <KLocalizedString>
#include <KStandardShortcut>

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>

// BEGIN class PinMapping
PinMapping::PinMapping(Type type)
{
    m_type = type;
}

PinMapping::PinMapping()
{
    m_type = Invalid;
}

PinMapping::~PinMapping()
{
}
// END class PinMapping

// BEGIN class PinMapEditor
PinMapEditor::PinMapEditor(PinMapping *pinMapping, MicroInfo *picInfo, QWidget *parent, const char *name)
    : QDialog(parent)
{
    setObjectName(name);
    setModal(true);
    setWindowTitle(i18n("Pin Map Editor"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    m_pPinMapping = pinMapping;

    m_pPinMapDocument = new PinMapDocument();

    {
        QAction *actionDelSel = new QAction(this);
        actionDelSel->setShortcut(Qt::Key_Delete);
        connect(actionDelSel, SIGNAL(triggered(bool)), m_pPinMapDocument, SLOT(deleteSelection()));
        addAction(actionDelSel);
    }
    {
        QAction *actionSelAll = new QAction(this);
        actionSelAll->setShortcut(KStandardShortcut::selectAll().first());
        connect(actionSelAll, SIGNAL(triggered(bool)), m_pPinMapDocument, SLOT(selectAll()));
        addAction(actionSelAll);
    }
    {
        QAction *actionUndo = new QAction(this);
        actionUndo->setShortcut(KStandardShortcut::undo().first());
        connect(actionUndo, SIGNAL(triggered(bool)), m_pPinMapDocument, SLOT(undo()));
        addAction(actionUndo);
    }
    {
        QAction *actionRedo = new QAction(this);
        actionRedo->setShortcut(KStandardShortcut::redo().first());
        connect(actionRedo, SIGNAL(triggered(bool)), m_pPinMapDocument, SLOT(redo()));
        addAction(actionRedo);
    }

    QFrame *f = new QFrame(this);
    f->setMinimumWidth(480);
    f->setMinimumHeight(480);

    f->setFrameShape(QFrame::Box);
    f->setFrameShadow(QFrame::Plain);
    QVBoxLayout *fLayout = new QVBoxLayout; // ( f, 1, 0, "fLayout" ); // 2018.08.18 - use non-deprected constructor
    fLayout->setContentsMargins(1, 1, 1, 1);
    fLayout->setSpacing(0);
    fLayout->setObjectName("fLayout");

    ViewContainer *vc = new ViewContainer(nullptr, f);
    fLayout->addWidget(vc);

    m_pPinMapView = static_cast<PinMapView *>(m_pPinMapDocument->createView(vc, 0));

    // qApp->processEvents(); // 2015.07.07 - do not process events, if it is not urgently needed; might generate crashes?

    m_pPinMapDocument->init(*m_pPinMapping, picInfo);

    mainLayout->addWidget(f);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    mainLayout->addWidget(buttonBox);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PinMapEditor::slotOk);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &PinMapEditor::slotApply);
}

void PinMapEditor::slotApply()
{
    savePinMapping();
}

void PinMapEditor::slotOk()
{
    savePinMapping();
    accept();
}

void PinMapEditor::savePinMapping()
{
    *m_pPinMapping = m_pPinMapDocument->pinMapping();
}
// END class PinMapEditor

// BEGIN class PinMapDocument
PinMapDocument::PinMapDocument()
    : CircuitICNDocument(nullptr, nullptr)
{
    m_pPicComponent = nullptr;
    m_pKeypad = nullptr;
    m_pSevenSegment = nullptr;
    m_type = dt_pinMapEditor;

    m_cmManager->addManipulatorInfo(CMSelect::manipulatorInfo());
}

PinMapDocument::~PinMapDocument()
{
}

void PinMapDocument::init(const PinMapping &pinMapping, MicroInfo *microInfo)
{
    m_pinMappingType = pinMapping.type();

    m_pPicComponent = static_cast<PIC_IC *>(addItem("PIC_IC", QPoint(336, 224), true));
    m_pPicComponent->initPackage(microInfo);

    const QStringList pins = pinMapping.pins();
    const QStringList::const_iterator end = pins.end();

    int keypadCols = -1; // -1 means no keypad

    switch (m_pinMappingType) {
    case PinMapping::SevenSegment: {
        m_pSevenSegment = static_cast<ECSevenSegment *>(addItem("ec/seven_segment", QPoint(144, 232), true));

        char ssPin = 'a';
        for (QStringList::const_iterator it = pins.begin(); it != end; ++it) {
            ICNDocument::createConnector(m_pSevenSegment->childNode(QChar(ssPin)), m_pPicComponent->childNode(*it));
            ssPin++;
        }

        break;
    }

    case PinMapping::Keypad_4x3:
        m_pKeypad = static_cast<ECKeyPad *>(addItem("ec/keypad", QPoint(144, 232), true));
        m_pKeypad->property("numCols")->setValue(3);
        keypadCols = 3;
        break;

    case PinMapping::Keypad_4x4:
        m_pKeypad = static_cast<ECKeyPad *>(addItem("ec/keypad", QPoint(144, 232), true));
        m_pKeypad->property("numCols")->setValue(4);
        keypadCols = 4;
        break;

    case PinMapping::Invalid:
        qDebug() << Q_FUNC_INFO << "m_pinMappingType == Invalid" << endl;
        break;
    }

    if (keypadCols != -1) {
        QStringList::const_iterator it = pins.begin();
        for (unsigned row = 0; (row < 4) && (it != end); ++row, ++it)
            ICNDocument::createConnector(m_pKeypad->childNode(QString("row_%1").arg(row)), m_pPicComponent->childNode(*it));

        for (int col = 0; (col < keypadCols) && (it != end); ++col, ++it)
            ICNDocument::createConnector(m_pKeypad->childNode(QString("col_%1").arg(col)), m_pPicComponent->childNode(*it));
    }

    clearHistory(); // Don't allow undoing of initial creation of stuff
}

bool PinMapDocument::isValidItem(Item *item)
{
    return isValidItem(item->type());
}

bool PinMapDocument::isValidItem(const QString &id)
{
    if (!m_pPicComponent && id == "PIC_IC")
        return true;

    switch (m_pinMappingType) {
    case PinMapping::SevenSegment:
        return (!m_pSevenSegment && id == "ec/seven_segment");

    case PinMapping::Keypad_4x3:
        return (!m_pKeypad && id == "ec/keypad");

    case PinMapping::Keypad_4x4:
        return (!m_pKeypad && id == "ec/keypad");

    case PinMapping::Invalid:
        return false;
    }

    return false;
}

void PinMapDocument::deleteSelection()
{
    m_selectList->removeQCanvasItem(m_pPicComponent);
    m_selectList->removeQCanvasItem(m_pSevenSegment);
    m_selectList->removeQCanvasItem(m_pKeypad);

    ICNDocument::deleteSelection();
}

PinMapping PinMapDocument::pinMapping() const
{
    const NodeInfoMap picNodeInfoMap = m_pPicComponent->nodeMap();
    const NodeInfoMap::const_iterator picNodeInfoMapEnd = picNodeInfoMap.end();

    QStringList picPinIDs;
    QStringList attachedIDs;
    Component *attached = nullptr;

    switch (m_pinMappingType) {
    case PinMapping::SevenSegment:
        for (unsigned i = 0; i < 7; ++i)
            attachedIDs << QChar('a' + i);
        attached = m_pSevenSegment;
        break;

    case PinMapping::Keypad_4x3:
        for (unsigned i = 0; i < 4; ++i)
            attachedIDs << QString::fromLatin1("row_%1").arg(i);
        for (unsigned i = 0; i < 3; ++i)
            attachedIDs << QString::fromLatin1("col_%1").arg(i);
        attached = m_pKeypad;
        break;

    case PinMapping::Keypad_4x4:
        for (unsigned i = 0; i < 4; ++i)
            attachedIDs << QString::fromLatin1("row_%1").arg(i);
        for (unsigned i = 0; i < 4; ++i)
            attachedIDs << QString::fromLatin1("col_%1").arg(i);
        attached = m_pKeypad;
        break;

    case PinMapping::Invalid:
        break;
    }

    if (!attached)
        return PinMapping();

    QStringList::iterator end = attachedIDs.end();
    for (QStringList::iterator attachedIt = attachedIDs.begin(); attachedIt != end; ++attachedIt) {
        Node *node = attached->childNode(*attachedIt);
        QString pinID;

        for (NodeInfoMap::const_iterator it = picNodeInfoMap.begin(); it != picNodeInfoMapEnd; ++it) {
            if (it.value().node->isConnected(node)) {
                pinID = it.key();
                break;
            }
        }

        picPinIDs << pinID;
    }

    PinMapping pinMapping(m_pinMappingType);
    pinMapping.setPins(picPinIDs);

    return pinMapping;
}
// END class PinMapDocument

// BEGIN class PinMapView
PinMapView::PinMapView(PinMapDocument *pinMapDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name)
    : ICNView(pinMapDocument, viewContainer, viewAreaId, name)
{
}

PinMapView::~PinMapView()
{
}
// END class PinMapView

// BEGIN class PIC_IC
Item *PIC_IC::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new PIC_IC((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *PIC_IC::libraryItem()
{
    return new LibraryItem(QStringList(QString::fromLatin1("PIC_IC")), nullptr, nullptr, LibraryItem::lit_other, PIC_IC::construct);
}

PIC_IC::PIC_IC(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, id ? id : "PIC_IC")
{
}

PIC_IC::~PIC_IC()
{
}

void PIC_IC::initPackage(MicroInfo *microInfo)
{
    // The code in this function is a stripped down version of that in PICComponent::initPackage

    if (!microInfo)
        return;

    MicroPackage *microPackage = microInfo->package();
    if (!microPackage)
        return;

    // BEGIN Get pin IDs
    QStringList allPinIDs = microPackage->pinIDs();
    QStringList ioPinIDs = microPackage->pinIDs(PicPin::type_bidir | PicPin::type_input | PicPin::type_open);

    // Now, we make the unwanted pin ids blank, so a pin is not created for them
    const QStringList::iterator allPinIDsEnd = allPinIDs.end();
    for (QStringList::iterator it = allPinIDs.begin(); it != allPinIDsEnd; ++it) {
        if (!ioPinIDs.contains(*it))
            *it = "";
    }
    // END Get pin IDs

    // BEGIN Remove old stuff
    // Remove old text
    TextMap textMapCopy = m_textMap;
    const TextMap::iterator textMapEnd = textMapCopy.end();
    for (TextMap::iterator it = textMapCopy.begin(); it != textMapEnd; ++it)
        removeDisplayText(it.key());

    // Remove old nodes
    NodeInfoMap nodeMapCopy = m_nodeMap;
    const NodeInfoMap::iterator nodeMapEnd = nodeMapCopy.end();
    for (NodeInfoMap::iterator it = nodeMapCopy.begin(); it != nodeMapEnd; ++it) {
        if (!ioPinIDs.contains(it.key()))
            removeNode(it.key());
    }
    // END Remove old stuff

    // BEGIN Create new stuff
    initDIPSymbol(allPinIDs, 80);
    initDIP(allPinIDs);
    // END Create new stuff

    addDisplayText("picid", QRect(offsetX(), offsetY() - 16, width(), 16), microInfo->id());
}
// END class PIC_IC
