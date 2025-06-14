/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "flowcodedocument.h"
#include "canvasmanipulator.h"
#include "documentiface.h"
#include "drawpart.h"
#include "flowcode.h"
#include "flowcodeview.h"
#include "flowpart.h"
#include "itemdocumentdata.h"
#include "ktechlab.h"
#include "languagemanager.h"
#include "microinfo.h"
#include "microlibrary.h"
#include "outputmethoddlg.h"
#include "picitem.h"
#include "programmerdlg.h"
#include "textdocument.h"
#include "filefilters.h"

#include <KLocalizedString>

#include <ktechlab_debug.h>

FlowCodeDocument::FlowCodeDocument(const QString &caption)
    : FlowICNDocument(caption)
{
    m_pDocumentIface = new FlowCodeDocumentIface(this);
    m_type = Document::dt_flowcode;
    m_microInfo = nullptr;
    m_microSettings = nullptr;
    m_picItem = nullptr;
    m_pLastTextOutputTarget = nullptr;

    m_cmManager->addManipulatorInfo(CMSelect::manipulatorInfo());
    m_cmManager->addManipulatorInfo(CMDraw::manipulatorInfo());
    m_cmManager->addManipulatorInfo(CMRightClick::manipulatorInfo());
    m_cmManager->addManipulatorInfo(CMRepeatedItemAdd::manipulatorInfo());
    m_cmManager->addManipulatorInfo(CMItemResize::manipulatorInfo());
    m_cmManager->addManipulatorInfo(CMItemDrag::manipulatorInfo());

    m_fileExtensionInfo = QString("*.flowcode|FlowCode (*.flowcode)\n*|%1").arg(i18n("All Files"));
    m_fileExtensionValue = QString(".flowcode");
    requestStateSave();
}

FlowCodeDocument::~FlowCodeDocument()
{
    m_bDeleted = true;
    if (m_picItem)
        m_picItem->removeItem();

    delete m_microSettings;
    delete m_pDocumentIface;
}

View *FlowCodeDocument::createView(ViewContainer *viewContainer, uint viewAreaId)
{
    View *view = new FlowCodeView(this, viewContainer, viewAreaId);
    handleNewView(view);
    return view;
}

void FlowCodeDocument::setPicType(const QString &id)
{
    if (m_microSettings && m_microSettings->microInfo() && m_microSettings->microInfo()->id() == id)
        return;

    MicroInfo *microInfo = MicroLibrary::self()->microInfoWithID(id);

    if (!microInfo) {
        qCWarning(KTL_LOG) << "FlowCodeDocument::setPicType: Could not set the pic type to PIC \"" << id << "\"";
        return;
    }

    m_microInfo = microInfo;

    if (m_microSettings) {
        // TODO write the pic settings to somewhere temporary and then restore them
        delete m_microSettings;
    }

    m_microSettings = new MicroSettings(m_microInfo);
    connect(m_microSettings, &MicroSettings::pinMappingsChanged, this, &FlowCodeDocument::pinMappingsChangedFlowCode);
    // TODO restore pic settings from temporary location if appropriate

    delete m_picItem;
    m_picItem = new PicItem(this, true, "picItem", m_microSettings);
    m_picItem->show();

    /* emit */ picTypeChanged();
}

bool FlowCodeDocument::isValidItem(const QString &itemId)
{
    return itemId.startsWith("flow/") || itemId.startsWith("dp/");
}

bool FlowCodeDocument::isValidItem(Item *item)
{
    if (!dynamic_cast<FlowPart *>(item) && !dynamic_cast<DrawPart *>(item))
        return false;

    if (!item->id().startsWith("START") && !item->id().startsWith("PPEND"))
        return true;

    const ItemMap::iterator ciEnd = m_itemList.end();

    if (item->id().startsWith("START")) {
        int count = 0;

        for (ItemMap::iterator it = m_itemList.begin(); it != ciEnd; ++it) {
            if ((*it)->id().startsWith("START"))
                count++;
        }
        if (count > 1)
            return false;
    } else if (item->id().startsWith("PPEND")) {
        int count = 0;
        for (ItemMap::iterator it = m_itemList.begin(); it != ciEnd; ++it) {
            if ((*it)->id().startsWith("PPEND"))
                count++;
        }

        if (count > 1)
            return false;
    }

    return true;
}

void FlowCodeDocument::setLastTextOutputTarget(TextDocument *target)
{
    m_pLastTextOutputTarget = target;
}

void FlowCodeDocument::slotConvertTo(QAction *action)
{
    int target = action->data().toInt();
    switch (ConvertToTarget(target)) {
    case FlowCodeDocument::MicrobeOutput:
        convertToMicrobe();
        break;

    case FlowCodeDocument::AssemblyOutput:
        convertToAssembly();
        break;

    case FlowCodeDocument::HexOutput:
        convertToHex();
        break;

    case FlowCodeDocument::PICOutput:
        convertToPIC();
        break;
    }
}

void FlowCodeDocument::convertToMicrobe()
{
    OutputMethodDlg *dlg = new OutputMethodDlg(i18n("Microbe Code Output"), url(), false, activeView());
    dlg->setOutputExtension(".microbe");
    dlg->setFileFilters({
        {i18n("Microbe") + QLatin1String(" (*.microbe)"), QStringLiteral("*.microbe")},
        {i18n("All Files"), QStringLiteral("*")},
    });

    const int accepted = dlg->exec();
    if (accepted != QDialog::Accepted) {
        delete dlg;
        return;
    }

    ProcessOptions o(dlg->info());
    o.setTextOutputTarget(m_pLastTextOutputTarget, this, SLOT(setLastTextOutputTarget(TextDocument *)));
    o.p_flowCodeDocument = this;
    o.setProcessPath(ProcessOptions::ProcessPath::FlowCode_Microbe);
    LanguageManager::self()->compile(o);

    delete dlg;
}

void FlowCodeDocument::convertToAssembly()
{
    OutputMethodDlg *dlg = new OutputMethodDlg(i18n("Assembly Code Output"), url(), false, activeView());
    dlg->setOutputExtension(".asm");
    dlg->setFileFilters({
        {i18n("Assembly Code") + QLatin1String(" (*.asm, *.src, *.inc)"), QStringLiteral("*.asm *.src *.inc")},
        {i18n("All Files"), QStringLiteral("*")},
    });

    const int accepted = dlg->exec();
    if (accepted != QDialog::Accepted) {
        delete dlg;
        return;
    }

    ProcessOptions o(dlg->info());
    o.setTextOutputTarget(m_pLastTextOutputTarget, this, SLOT(setLastTextOutputTarget(TextDocument *)));
    o.p_flowCodeDocument = this;
    o.setProcessPath(ProcessOptions::ProcessPath::FlowCode_AssemblyAbsolute);
    LanguageManager::self()->compile(o);

    delete dlg;
}

void FlowCodeDocument::convertToHex()
{
    OutputMethodDlg *dlg = new OutputMethodDlg(i18n("Hex Code Output"), url(), false, KTechlab::self());
    dlg->setOutputExtension(".hex");
    dlg->setFileFilters({
        {i18n("Hex Code") + QLatin1String(" (*.hex)"), QStringLiteral("*.hex")},
        {i18n("All Files"), QStringLiteral("*")},
    });

    const int accepted = dlg->exec();
    if (accepted != QDialog::Accepted) {
        delete dlg;
        return;
    }

    ProcessOptions o(dlg->info());
    o.setTextOutputTarget(m_pLastTextOutputTarget, this, SLOT(setLastTextOutputTarget(TextDocument *)));
    o.p_flowCodeDocument = this;
    o.setProcessPath(ProcessOptions::ProcessPath::FlowCode_Program);
    LanguageManager::self()->compile(o);

    delete dlg;
}

void FlowCodeDocument::convertToPIC()
{
    ProgrammerDlg *dlg = new ProgrammerDlg(microSettings()->microInfo()->id(), static_cast<QWidget *>(KTechlab::self()));
    dlg->setObjectName("Programmer Dlg");

    const int accepted = dlg->exec();
    if (accepted != QDialog::Accepted) {
        dlg->deleteLater();
        return;
    }

    ProcessOptions o;
    dlg->initOptions(&o);
    o.p_flowCodeDocument = this;
    o.setProcessPath(ProcessOptions::ProcessPath::FlowCode_PIC);
    LanguageManager::self()->compile(o);

    dlg->deleteLater();
}

void FlowCodeDocument::varNameChanged(const QString &newValue, const QString &oldValue)
{
    if (m_bDeleted)
        return;

    // Decrease the old variable count
    // If none are left after, remove it from microsettings
    StringIntMap::iterator it = m_varNames.find(oldValue);
    if (it != m_varNames.end()) {
        --(it.value());
        if (it.value() <= 0) {
            VariableInfo *info = microSettings()->variableInfo(it.key());
            if (info && !info->permanent)
                microSettings()->deleteVariable(oldValue);
            m_varNames.erase(it);
        }
    }

    // Add the new variable to a count, tell microsettings about it if it is new
    if (!newValue.isEmpty()) {
        it = m_varNames.find(newValue);
        if (it != m_varNames.end()) {
            ++it.value();
        } else {
            m_varNames[newValue] = 1;
            microSettings()->setVariable(newValue, QVariant(), false);
        }
    }

    // Tell all FlowParts to update their variable lists
    const ItemMap::iterator end = m_itemList.end();
    for (ItemMap::iterator it = m_itemList.begin(); it != end; ++it) {
        if (FlowPart *flowPart = dynamic_cast<FlowPart *>(*it))
            flowPart->updateVarNames();
    }
}

#include "moc_flowcodedocument.cpp"
