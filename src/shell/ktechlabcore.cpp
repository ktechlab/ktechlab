 /**************************************************************************
 *   Copyright (C) 2008 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ktechlabcore.h"

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

KTechlabCore *KTechlabCore::m_self = 0;

void KTechlabCore::initialize( KDevelop::Core::Setup mode )
{
    if ( m_self )
        return;
    
    m_self = new KTechlabCore();
    KDevelop::Core::m_self = m_self;
    m_self->d->initialize( mode );
}

KTechlabCore::KTechlabCore( QObject* parent )
: KDevelop::Core( new KDevelop::CorePrivate( this ), parent )
{
    d = KDevelop::Core::d;
}

KDevelop::Core::Setup KTechlabCore::setupFlags() const
{
    return d->m_mode;
}

