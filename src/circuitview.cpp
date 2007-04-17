/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitdocument.h"
#include "circuitview.h"
#include "config.h"
#include "ktechlab.h"
#include "simulator.h"
#include "viewiface.h"

#include <klocale.h>
#include <qwhatsthis.h>

CircuitView::CircuitView( CircuitDocument * circuitDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name )
	: ICNView( circuitDocument, viewContainer, viewAreaId, name ),
	p_circuitDocument(circuitDocument)
{
	KActionCollection * ac = actionCollection();
	
	new KAction( "Dump linear equations", Qt::CTRL|Qt::Key_D, circuitDocument, SLOT(displayEquations()), ac, "dump_les" );
	
	//BEGIN Item Control Actions
	KRadioAction * ra;
	ra = new KRadioAction( i18n("0 Degrees"), "", 0, circuitDocument, SLOT(setOrientation0()), ac, "edit_orientation_0" );
	ra->setExclusiveGroup("orientation");
	ra->setChecked(true);
	ra = new KRadioAction( i18n("90 Degrees"), "", 0, circuitDocument, SLOT(setOrientation90()), ac, "edit_orientation_90" );
	ra->setExclusiveGroup("orientation");
	ra = new KRadioAction( i18n("180 Degrees"), "", 0, circuitDocument, SLOT(setOrientation180()), ac, "edit_orientation_180" );
	ra->setExclusiveGroup("orientation");
	ra =new KRadioAction( i18n("270 Degrees"), "", 0, circuitDocument, SLOT(setOrientation270()), ac, "edit_orientation_270" );
	ra->setExclusiveGroup("orientation");
	
	new KAction( i18n("Create Subcircuit"), "", 0, circuitDocument, SLOT(createSubcircuit()), ac, "circuit_create_subcircuit" );
	new KAction( i18n("Rotate Clockwise"), "rotate_cw", "]", circuitDocument, SLOT(rotateClockwise()), ac, "edit_rotate_cw" );
	new KAction( i18n("Rotate Counter-Clockwise"), "rotate_ccw", "[", circuitDocument, SLOT(rotateCounterClockwise()), ac, "edit_rotate_ccw" );
	new KAction( i18n("Flip Horizontally"), "", 0, circuitDocument, SLOT(flipHorizontally()), ac, "edit_flip_horizontally" );
	new KAction( i18n("Flip Vertically"), "", 0, circuitDocument, SLOT(flipVertically()), ac, "edit_flip_vertically" );
	//END Item Control Actions
	
	setXMLFile( "ktechlabcircuitui.rc", true );
	
	
	QWhatsThis::add( this, i18n(
			"Construct a circuit by dragging components from the Component selector from the left. Create the connections by dragging a wire from the component connectors.<br><br>"
					
			"The simulation is running by default, but can be paused and resumed from the Tools menu.<br><br>"
					
			"To delete a wire, select it with a select box, and hit delete.<br><br>"
					
			"To edit the attributes of a component, select it (making sure that no components of another type are also selected), and edit in the toolbar. More advanced properties can be edited using the item editor on the right.<br><br>"
					
			"Subcircuits can be created by connecting the components with an External Connection, selecting the desired components and clicking on \"Create Subcircuit\" in the right-click menu.")
				   );
	
	m_pViewIface = new CircuitViewIface(this);
	
	m_statusBar->insertItem( "", ViewStatusBar::SimulationState );
	connect( Simulator::self(), SIGNAL(simulatingStateChanged(bool )), this, SLOT(slotUpdateRunningStatus(bool )) );
	slotUpdateRunningStatus( Simulator::self()->isSimulating() );
}


CircuitView::~CircuitView()
{
	delete m_pViewIface;
	m_pViewIface = 0l;
}


void CircuitView::slotUpdateRunningStatus( bool isRunning )
{
	m_statusBar->changeItem( isRunning ? i18n("Simulation Running") : i18n("Simulation Paused"), ViewStatusBar::SimulationState );
}


void CircuitView::dragEnterEvent( QDragEnterEvent * e )
{
	ICNView::dragEnterEvent(e);
	if ( e->isAccepted() )
		return;
	
	bool acceptable = e->provides("ktechlab/component") || e->provides("ktechlab/subcircuit");
	if ( !acceptable )
		return;
	
	e->accept( true );
	createDragItem( e );
}

#include "circuitview.moc"
