/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "viewiface.h"
#include "circuitview.h"
#include "document.h"
#include "flowcodeview.h"
#include "mechanicsview.h"
#include "textview.h"

// BEGIN class ViewIface
ViewIface::ViewIface(View *view)
    : DCOPObject(/* "View" TODO */)
{
    m_pView = view;
}

ViewIface::~ViewIface()
{
}

DCOPRef ViewIface::document()
{
    return DCOPRef(); // TODO m_pView->document()->dcopObject() );
}

bool ViewIface::hasFocus()
{
    return m_pView->hasFocus();
}

bool ViewIface::close()
{
    return m_pView->closeView();
}

void ViewIface::zoomIn()
{
    m_pView->viewZoomIn();
}

void ViewIface::zoomOut()
{
    m_pView->viewZoomOut();
}

bool ViewIface::canZoomIn()
{
    return m_pView->canZoomIn();
}

bool ViewIface::canZoomOut()
{
    return m_pView->canZoomOut();
}

void ViewIface::actualSize()
{
    m_pView->actualSize();
}
// END class ViewIface

// BEGIN class TextViewIface
TextViewIface::TextViewIface(TextView *view)
    : ViewIface(view)
{
    m_pTextView = view;
}

void TextViewIface::toggleBreakpoint()
{
    m_pTextView->toggleBreakpoint();
}

bool TextViewIface::gotoLine(const int line)
{
    return m_pTextView->gotoLine(line);
}
// END class TextViewIface

// BEGIN class ItemViewIface
ItemViewIface::ItemViewIface(ItemView *view)
    : ViewIface(view)
{
    m_pItemView = view;
}

double ItemViewIface::zoomLevel()
{
    return m_pItemView->zoomLevel();
}
// END class ItemViewIface

// BEGIN class MechanicsViewIface
MechanicsViewIface::MechanicsViewIface(MechanicsView *view)
    : ItemViewIface(view)
{
    m_pMechanicsView = view;
}
// END class ICNViewIface

// BEGIN class ICNViewIface
ICNViewIface::ICNViewIface(ICNView *view)
    : ItemViewIface(view)
{
    m_pICNView = view;
}
// END class ICNViewIface

// BEGIN class CircuitViewIface
CircuitViewIface::CircuitViewIface(CircuitView *view)
    : ICNViewIface(view)
{
    m_pCircuitView = view;
}
// END class CircuitViewIface

// BEGIN class FlowCodeViewIface
FlowCodeViewIface::FlowCodeViewIface(FlowCodeView *view)
    : ICNViewIface(view)
{
}
// END class FlowCodeViewIface
