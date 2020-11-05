/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "languagemanager.h"
#include "docmanager.h"
#include "ktechlab.h"
#include "logview.h"
// #include "k3tempfile.h"
#include "outputmethoddlg.h"
#include "processchain.h"
#include "projectmanager.h"

#include "gpasm.h"
#include "gpdasm.h"
#include "microbe.h"

// #include <k3dockwidget.h>
#include <KLocalizedString>

#include <cassert>

#include <ktlconfig.h>

LanguageManager *LanguageManager::m_pSelf = nullptr;

LanguageManager *LanguageManager::self(KateMDI::ToolView *parent)
{
    if (!m_pSelf) {
        assert(parent);
        m_pSelf = new LanguageManager(parent);
    }
    return m_pSelf;
}

LanguageManager::LanguageManager(KateMDI::ToolView *parent)
    : QObject(KTechlab::self())
{
    m_logView = new LogView(parent);
    m_logView->setObjectName("LanguageManager LogView");

    m_logView->setWhatsThis(
        i18n("These messages show the output of language-related functionality such as compiling and assembling.<br><br>For error messages, clicking on the line will automatically open up the file at the position of the error."));
    connect(m_logView, SIGNAL(paraClicked(const QString &, MessageInfo)), this, SLOT(slotParaClicked(const QString &, MessageInfo)));
    reset();
}

LanguageManager::~LanguageManager()
{
}

void LanguageManager::reset()
{
    m_logView->clear();
}

ProcessChain *LanguageManager::compile(ProcessOptions options)
{
    if (KTLConfig::raiseMessagesLog())
        KTechlab::self()->showToolView(KTechlab::self()->toolView(toolViewIdentifier()));

    return new ProcessChain(options);
}

ProcessListChain *LanguageManager::compile(ProcessOptionsList pol)
{
    if (KTLConfig::raiseMessagesLog())
        KTechlab::self()->showToolView(KTechlab::self()->toolView(toolViewIdentifier()));

    return new ProcessListChain(pol);
}

void LanguageManager::slotError(const QString &error, MessageInfo messageInfo)
{
    m_logView->addOutput(error, LogView::ot_error, messageInfo);
}
void LanguageManager::slotWarning(const QString &error, MessageInfo messageInfo)
{
    m_logView->addOutput(error, LogView::ot_warning, messageInfo);
}
void LanguageManager::slotMessage(const QString &error, MessageInfo messageInfo)
{
    m_logView->addOutput(error, LogView::ot_message, messageInfo);
}

void LanguageManager::slotParaClicked(const QString &message, MessageInfo messageInfo)
{
    Q_UNUSED(message);
    DocManager::self()->gotoTextLine(QUrl::fromUserInput(messageInfo.fileURL()), messageInfo.fileLine());
}
