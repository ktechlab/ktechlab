/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "icnview.h"
#include "canvasmanipulator.h"
#include "icndocument.h"
#include "ktechlab.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <KToolBarPopupAction>

#include <QAction>
#include <QActionGroup>
#include <QMenu>

ICNView::ICNView(ICNDocument *icnDocument, ViewContainer *viewContainer, uint viewAreaId)
    : ItemView(icnDocument, viewContainer, viewAreaId)
{
    bool manualRouting = (icnDocument->m_cmManager->cmState() & CMManager::cms_manual_route);

    KActionCollection *ac = actionCollection();

    // BEGIN Routing Actions
    // These actions get inserted into the main menu
    // m_pAutoRoutingAction = new QAction( i18n("Automatic"), "", 0, this, SLOT(slotSetRoutingAuto()), ac, "routing_mode_auto" );
    m_pAutoRoutingAction = new QAction(i18n("Automatic"), ac);
    m_pAutoRoutingAction->setObjectName("routing_mode_auto");
    connect(m_pAutoRoutingAction, SIGNAL(triggered(bool)), this, SLOT(slotSetRoutingAuto()));
    ac->addAction(m_pAutoRoutingAction->objectName(), m_pAutoRoutingAction);
    // m_pAutoRoutingAction->setExclusiveGroup("routing_mode");// TODO TEST
    if (!manualRouting)
        m_pAutoRoutingAction->setChecked(true);

    // m_pManualRoutingAction = new QAction( i18n("Manual"), "", 0, this, SLOT(slotSetRoutingManual()), ac, "routing_mode_manual" );
    m_pManualRoutingAction = new QAction(i18n("Manual"), ac);
    m_pManualRoutingAction->setObjectName("routing_mode_manual");
    connect(m_pManualRoutingAction, SIGNAL(triggered(bool)), this, SLOT(slotSetRoutingManual()));
    ac->addAction(m_pManualRoutingAction->objectName(), m_pManualRoutingAction);
    // m_pManualRoutingAction->setExclusiveGroup("routing_mode"); // TODO TEST
    if (manualRouting)
        m_pManualRoutingAction->setChecked(true);

    // This popup gets inserted into the toolbar
    // m_pRoutingModeToolbarPopup = new KToolBarPopupAction( i18n("Connection Routing Mode"), "pencil", 0, 0, 0, ac, "routing_mode" );
    m_pRoutingModeToolbarPopup = new KToolBarPopupAction(QIcon(QString::fromLatin1("pencil")), i18n("Connection Routing Mode"), ac);
    m_pRoutingModeToolbarPopup->setObjectName("routing_mode");
    m_pRoutingModeToolbarPopup->setDelayed(false);
    ac->addAction(m_pRoutingModeToolbarPopup->objectName(), m_pRoutingModeToolbarPopup);

    QMenu *m = m_pRoutingModeToolbarPopup->menu();
    m->setTitle(i18n("Connection Routing Mode"));

    m_actMenuRouteAutoRoute = m->addAction(/*QIconLoader::global()->loadIcon(
       "routing_mode_auto",	QIconLoader::Small ), */
                                           i18n("Automatic"));
    m_actMenuRouteAutoRoute->setData(0);
    m_actMenuRouteManRoute = m->addAction(/*QIconLoader::global()->loadIcon(
       "routing_mode_manual",	QIconLoader::Small ),*/
                                          i18n("Manual"));
    m_actMenuRouteManRoute->setData(1);

    // m->setCheckable(true); // 2018.12.02
    if (manualRouting) {
        m_actMenuRouteManRoute->setChecked(true);
    } else {
        m_actMenuRouteAutoRoute->setChecked(true);
    }
    // m->setItemChecked( manualRouting ? 1 : 0, true ); // 2018.12.02

    connect(m, SIGNAL(triggered(QAction *)), this, SLOT(slotSetRoutingMode(QAction *)));
    // END Routing Actions

    connect(icnDocument->m_cmManager, SIGNAL(manualRoutingChanged(bool)), this, SLOT(slotUpdateRoutingToggles(bool)));
}

ICNView::~ICNView()
{
}

void ICNView::slotSetRoutingMode(QAction *action)
{
    int mode = action->data().toInt();
    // This function is called when the user selects a mode from the toolbar drop-down menu
    bool manualEnabled = (mode == 1);

    if (bool(p_itemDocument->m_cmManager->cmState() & CMManager::cms_manual_route) == manualEnabled)
        return;

    slotUpdateRoutingMode(manualEnabled);
    slotUpdateRoutingToggles(manualEnabled);
}

void ICNView::slotSetRoutingManual()
{
    slotUpdateRoutingMode(true);
    slotUpdateRoutingToggles(true);
}

void ICNView::slotSetRoutingAuto()
{
    slotUpdateRoutingMode(false);
    slotUpdateRoutingToggles(false);
}

void ICNView::slotUpdateRoutingMode(bool manualRouting)
{
    p_itemDocument->m_cmManager->slotSetManualRoute(manualRouting);
    p_itemDocument->canvas()->setMessage(manualRouting ? i18n("<b>Manual</b> connection routing enabled.") : i18n("<b>Automatic</b> connection routing enabled."));
}

void ICNView::slotUpdateRoutingToggles(bool manualRouting)
{
    // m_pRoutingModeToolbarPopup->menu()->setItemChecked( !manualRouting, 0 );  // 2018.12.02
    // m_pRoutingModeToolbarPopup->menu()->setItemChecked(  manualRouting, 1 );
    if (manualRouting) {
        m_actMenuRouteAutoRoute->setChecked(false);
        m_actMenuRouteManRoute->setChecked(true);
    } else {
        m_actMenuRouteAutoRoute->setChecked(true);
        m_actMenuRouteManRoute->setChecked(false);
    }

    if (manualRouting)
        m_pManualRoutingAction->setChecked(true);
    else
        m_pAutoRoutingAction->setChecked(true);
}
