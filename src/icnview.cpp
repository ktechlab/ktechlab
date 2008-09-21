/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasmanipulator.h"
#include "icndocument.h"
#include "icnview.h"
#include "ktechlab.h"

#include <kconfig.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>

ICNView::ICNView( ICNDocument *icnDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name )
	: ItemView( icnDocument, viewContainer, viewAreaId, name )
{
	bool manualRouting = (icnDocument->m_cmManager->cmState() & CMManager::cms_manual_route);
	
	KActionCollection * ac = actionCollection();
	
	//BEGIN Routing Actions
	// These actions get inserted into the main menu
	m_pAutoRoutingAction = new KRadioAction( i18n("Automatic"), "", 0, this, SLOT(slotSetRoutingAuto()), ac, "routing_mode_auto" );
	m_pAutoRoutingAction->setExclusiveGroup("routing_mode");
	if ( !manualRouting )
		m_pAutoRoutingAction->setChecked( true );
	
	m_pManualRoutingAction = new KRadioAction( i18n("Manual"), "", 0, this, SLOT(slotSetRoutingManual()), ac, "routing_mode_manual" );
	m_pManualRoutingAction->setExclusiveGroup("routing_mode");
	if ( manualRouting )
		m_pManualRoutingAction->setChecked( true );
	
	
	// This popup gets inserted into the toolbar
	m_pRoutingModeToolbarPopup = new KToolBarPopupAction( i18n("Connection Routing Mode"), "pencil", 0, 0, 0, ac, "routing_mode" );
	m_pRoutingModeToolbarPopup->setDelayed(false);
	
	KPopupMenu * m = m_pRoutingModeToolbarPopup->popupMenu();
	m->insertTitle( i18n("Connection Routing Mode") );
	
	m->insertItem( /*KGlobal::iconLoader()->loadIcon( "routing_mode_auto",	KIcon::Small ), */i18n("Automatic"), 0 );
	m->insertItem( /*KGlobal::iconLoader()->loadIcon( "routing_mode_manual",	KIcon::Small ),*/ i18n("Manual"), 1 );
	
	m->setCheckable(true);
	m->setItemChecked( manualRouting ? 1 : 0, true );
	
	connect( m, SIGNAL(activated(int)), this, SLOT(slotSetRoutingMode(int)) );
	//END Routing Actions
	
	connect( icnDocument->m_cmManager, SIGNAL(manualRoutingChanged(bool )), this, SLOT(slotUpdateRoutingToggles(bool )) );
}


ICNView::~ICNView()
{
}


void ICNView::slotSetRoutingMode( int mode )
{
	// This function is called when the user selects a mode from the toolbar drop-down menu
	bool manualEnabled = (mode == 1);
	
	if ( bool(p_itemDocument->m_cmManager->cmState() & CMManager::cms_manual_route) == manualEnabled )
		return;
	
	slotUpdateRoutingMode( manualEnabled );
	slotUpdateRoutingToggles( manualEnabled );
}


void ICNView::slotSetRoutingManual()
{
	slotUpdateRoutingMode( true );
	slotUpdateRoutingToggles( true );
}


void ICNView::slotSetRoutingAuto()
{
	slotUpdateRoutingMode( false );
	slotUpdateRoutingToggles( false );
}


void ICNView::slotUpdateRoutingMode( bool manualRouting )
{
	p_itemDocument->m_cmManager->slotSetManualRoute( manualRouting );
	p_itemDocument->canvas()->setMessage( manualRouting ? i18n("<b>Manual</b> connection routing enabled.") : i18n("<b>Automatic</b> connection routing enabled.") );
}


void ICNView::slotUpdateRoutingToggles( bool manualRouting )
{
	m_pRoutingModeToolbarPopup->popupMenu()->setItemChecked( !manualRouting, 0 );
	m_pRoutingModeToolbarPopup->popupMenu()->setItemChecked(  manualRouting, 1 );
	
	if ( manualRouting )
		m_pManualRoutingAction->setChecked(true);
	else	m_pAutoRoutingAction->setChecked(true);
}


#include "icnview.moc"
