/***************************************************************************
 *   Copyright (C) 2005 by John Myers                                      *
 *   electronerd@electronerdia.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "scopescreen.h"

#include "ktechlab.h"
#include "probe.h"
#include "probepositioner.h"
#include "simulator.h"

#include <cmath>

#include <KLocalizedString>

// #include <q3button.h>
#include <QLabel>
#include <QScrollBar>
#include <QSlider>
#include <QTimer>
#include <QToolButton>

#include <cassert>

#include <ktechlab_debug.h>

ScopeScreen::ScopeScreen(KateMDI::ToolView *parent)
    : QWidget(parent)
{
    if (parent->layout()) {
        parent->layout()->addWidget(this);
        qCDebug(KTL_LOG) << " added item selector to parent's layout " << parent;
    } else {
        qCWarning(KTL_LOG) << " unexpected null layout on parent " << parent;
    }

    setupUi(this);
}

ScopeScreen::~ScopeScreen()
{
}

ScopeScreen *ScopeScreen::self(KateMDI::ToolView *parent)
{
    static ScopeScreen *pSelf = nullptr;
    if (pSelf)
        return pSelf;
    assert(parent);
    pSelf = new ScopeScreen(parent);
    return pSelf;
}

#include "moc_scopescreen.cpp"
