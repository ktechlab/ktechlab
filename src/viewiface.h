/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VIEWIFACE_H
#define VIEWIFACE_H

// #include <dcopobject.h>
// #include <dcopref.h>
#include "dcop_stub.h"

class CircuitView;
class FlowCodeView;
class ICNView;
class ItemView;
class MechanicsView;
class TextView;
class View;

/**
@author David Saxton
*/
class ViewIface : public DCOPObject
{
    K_DCOP

public:
    ViewIface(View *view);
    virtual ~ViewIface();

    k_dcop : DCOPRef document();
    bool hasFocus();
    bool close();
    void zoomIn();
    void zoomOut();
    bool canZoomIn();
    bool canZoomOut();
    void actualSize();

protected:
    View *m_pView;
};

class TextViewIface : public ViewIface
{
    K_DCOP

public:
    TextViewIface(TextView *view);

    k_dcop : void toggleBreakpoint();
    bool gotoLine(const int line);

protected:
    TextView *m_pTextView;
};

class ItemViewIface : public ViewIface
{
    K_DCOP

public:
    ItemViewIface(ItemView *view);

    k_dcop : double zoomLevel();

protected:
    ItemView *m_pItemView;
};

class MechanicsViewIface : public ItemViewIface
{
    K_DCOP

public:
    MechanicsViewIface(MechanicsView *view);

protected:
    MechanicsView *m_pMechanicsView;
};

class ICNViewIface : public ItemViewIface
{
    K_DCOP

public:
    ICNViewIface(ICNView *view);

protected:
    ICNView *m_pICNView;
};

class CircuitViewIface : public ICNViewIface
{
    K_DCOP

public:
    CircuitViewIface(CircuitView *view);

protected:
    CircuitView *m_pCircuitView;
};

class FlowCodeViewIface : public ICNViewIface
{
    K_DCOP

public:
    FlowCodeViewIface(FlowCodeView *view);

protected:
    FlowCodeView *m_pFlowCodeView;
};

#endif
