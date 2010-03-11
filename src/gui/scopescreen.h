/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SCOPESCREEN_H
#define SCOPESCREEN_H

#include "simulator.h"
#include "src/gui/scopescreenwidget.h"

namespace KateMDI { class ToolView; }

/**
	@author John Myers
*/
class ScopeScreen : public ScopeScreenWidget
{
    Q_OBJECT

private:
	ScopeScreen(KateMDI::ToolView * parent);
	
public:
	static ScopeScreen * self( KateMDI::ToolView * parent = 0L );
	static QString toolViewIdentifier() { return "ScopeScreen"; }
    virtual ~ScopeScreen();

};

#endif
