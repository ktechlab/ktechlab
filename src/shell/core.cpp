 /**************************************************************************
 *   Copyright (C) 2008 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "core.h"

#include "mainwindow.h"
#include "sessioncontroller.h"
#include "uicontroller.h"
#include "plugincontroller.h"
#include "projectcontroller.h"
#include "partcontroller.h"
#include "languagecontroller.h"
#include "documentcontroller.h"
#include "runcontroller.h"
#include "sourceformattercontroller.h"
#include "selectioncontroller.h"
#include "core_p.h"

using namespace KTechLab;

CorePrivate::CorePrivate( Core *core )
: m_componentData(
    KAboutData( "ktechlab", "ktechlab", ki18n("KTechLab Platform"),
                "1.0", ki18n("KDevelop shell implementation for KTechLab"),
                KAboutData::License_LGPL_V2 ) ),
    m_core(core),
    m_cleanedUp(false)
{
}

CorePrivate::~CorePrivate()
{
}

void CorePrivate::initialize()
{
    if ( !sessionController ) {
        sessionController = new SessionController(m_core);
    }
    if ( !uiController )
    {
        uiController = new UiController(m_core);
    }
    if ( !projectController ) {
        projectController = new ProjectController(m_core);
    }
    if ( !documentController ) {
        documentController = new DocumentController(m_core);
    }
    if ( !partController ) {
        partController = new PartController( m_core, uiController->defaultMainWindow() );
    }
    if ( !pluginController ) {
        pluginController = new PluginController(m_core);
    }

    sessionController->initialize();
    uiController->defaultMainWindow()->show();
    pluginController->initialize();
}

Core *Core::m_self = 0;

Core *Core::self()
{
    return m_self;
}

void Core::initialize( )
{
    if ( m_self )
        return;

    m_self = new Core();
    m_self->d->initialize( );
}

Core::Core( QObject* parent )
{
    d = new CorePrivate( this );
}

KDevelop::IUiController* Core::uiController()
{
    return d->uiController;
}

KDevelop::IPluginController* Core::pluginController()
{
    return d->pluginController;
}

KDevelop::IProjectController* Core::projectController()
{
    return d->projectController;
}

KDevelop::ILanguageController* Core::languageController()
{
    return 0;
}

KParts::PartManager* Core::partController()
{
    return d->partController;
}

KDevelop::IDocumentController* Core::documentController()
{
    return d->documentController;
}

KDevelop::IRunController* Core::runController()
{
    return 0;
}

KDevelop::ISourceFormatterController* Core::sourceFormatterController()
{
    return 0;
}

KDevelop::ISelectionController* Core::selectionController()
{
    return 0;
}

SessionController* Core::sessionController()
{
    return d->sessionController;
}

KDevelop::ISession* Core::activeSession()
{
    return sessionController()->activeSession();
}

KComponentData Core::componentData() const
{
    return d->m_componentData;
}

PluginController *Core::pluginControllerInternal()
{
    return d->pluginController;
}

DocumentController *Core::documentControllerInternal()
{
    return d->documentController;
}

UiController *Core::uiControllerInternal()
{
    return d->uiController;
}

PartController *Core::partControllerInternal()
{
    return d->partController;
}

ProjectController *Core::projectControllerInternal()
{
    return d->projectController;
}

void Core::cleanup()
{
    return;
}
