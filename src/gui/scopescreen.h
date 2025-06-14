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

#include <ui_scopescreenwidget.h>

namespace KateMDI
{
class ToolView;
}

/**
    @author John Myers
*/
class ScopeScreen : public QWidget, public Ui::ScopeScreenWidget
{
    Q_OBJECT

private:
    ScopeScreen(KateMDI::ToolView *parent);

public:
    static ScopeScreen *self(KateMDI::ToolView *parent = nullptr);
    static QString toolViewIdentifier()
    {
        return QLatin1StringView("ScopeScreen");
    }
    ~ScopeScreen() override;
};

#endif
