/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
#if HAVE_GPSIM

#include "debugmanager.h"
#include "docmanager.h"
#include "gpsimprocessor.h"
#include "textdocument.h"

#include <QDebug>
// #include <k3staticdeleter.h>
#include <QGlobalStatic>

// BEGIN class DebugManager
// DebugManager * DebugManager::m_pSelf = nullptr; // 2017.10.10 - use K_GLOBAL_STATIC
// static K3StaticDeleter<DebugManager> staticDebugManagerDeleter;

Q_GLOBAL_STATIC(DebugManager, globalDebugManager);

DebugManager *DebugManager::self()
{
    return globalDebugManager;
}

DebugManager::DebugManager()
    : QObject()
{
}

DebugManager::~DebugManager()
{
}

void DebugManager::registerGpsim(GpsimProcessor *gpsim)
{
    if (!gpsim)
        return;

    m_processors << gpsim;

    const QStringList files = gpsim->sourceFileList();
    QStringList::const_iterator end = files.end();
    for (QStringList::const_iterator it = files.begin(); it != end; ++it) {
        const QUrl url = QUrl::fromLocalFile(*it);
        if (TextDocument *doc = dynamic_cast<TextDocument *>(DocManager::self()->findDocument(url))) {
            if (!doc->debuggerIsRunning())
                doc->setDebugger(gpsim->currentDebugger(), false);
        }
    }
}

void DebugManager::urlOpened(TextDocument *td)
{
    if (td->debuggerIsRunning())
        return;

    m_processors.removeAll(static_cast<GpsimProcessor *>(nullptr));
    GpsimProcessorList::iterator end = m_processors.end();
    for (GpsimProcessorList::iterator it = m_processors.begin(); it != end; ++it) {
        if (!(*it)->sourceFileList().contains(td->url().path()))
            continue;

        (*it)->setDebugMode((td->guessedCodeType() == TextDocument::ct_asm) ? GpsimDebugger::AsmDebugger : GpsimDebugger::HLLDebugger);

        td->setDebugger((*it)->currentDebugger(), false);
        return;
    }
}
// END class DebugManager

#include "moc_debugmanager.cpp"

#endif
