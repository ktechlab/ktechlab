/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mechanicsview.h"
#include "mechanicsdocument.h"
#include "viewiface.h"

#include <QDragEnterEvent>
#include <QMimeData>

MechanicsView::MechanicsView(MechanicsDocument *mechanicsDocument, ViewContainer *viewContainer, uint viewAreaId)
    : ItemView(mechanicsDocument, viewContainer, viewAreaId)
{
    setXMLFile("ktechlabmechanicsui.rc", true);
    m_pViewIface = new MechanicsViewIface(this);
}

MechanicsView::~MechanicsView()
{
    delete m_pViewIface;
}

void MechanicsView::dragEnterEvent(QDragEnterEvent *e)
{
    ItemView::dragEnterEvent(e);
    if (e->isAccepted())
        return;

    // e->setAccepted( e->provides("ktechlab/mechanical") ); // 2019.04.14
    e->setAccepted(e->mimeData()->hasFormat("ktechlab/mechanical"));
}

#include "moc_mechanicsview.cpp"
