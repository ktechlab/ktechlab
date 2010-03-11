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

#include "probe.h"
#include "probepositioner.h"
#include "simulator.h"
#include "ktechlab.h"

#include <cmath>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h> 
#include <knuminput.h>
#include <qbutton.h>
#include <qlabel.h>
#include <qscrollbar.h>
#include <qslider.h>
#include <qtimer.h>
#include <qtoolbutton.h>

#include <cassert>

ScopeScreen::ScopeScreen( KateMDI::ToolView * parent)
        : ScopeScreenWidget(parent)
{
	
}


ScopeScreen::~ScopeScreen()
{}

ScopeScreen * ScopeScreen::self( KateMDI::ToolView * parent )
{
	static ScopeScreen * pSelf = 0L;
	if(pSelf)
		return pSelf;
	assert(parent);
	pSelf = new ScopeScreen(parent);
	return pSelf;
}


#include "scopescreen.moc"
