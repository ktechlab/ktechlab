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

#include <KLocalizedString>
#include <KActionCollection>
#include <KLocalizedString>

#include <QAction>
#include <QActionGroup>
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>

CircuitView::CircuitView( CircuitDocument * circuitDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name )
	: ICNView( circuitDocument, viewContainer, viewAreaId, name ),
	p_circuitDocument(circuitDocument)
{
	KActionCollection * ac = actionCollection();

    {
	//new QAction( "Dump linear equations", Qt::CTRL|Qt::Key_D, circuitDocument, SLOT(displayEquations()), ac, "dump_les" );
        QAction * a = new QAction( i18n("Dump linear equations"), ac);
        a->setObjectName("dump_les");
        ac->setDefaultShortcut(a, Qt::CTRL | Qt::Key_D);
        connect(a, SIGNAL(triggered(bool)), circuitDocument, SLOT(displayEquations()));
        ac->addAction(a->objectName(), a);
    }

	//BEGIN Item Control Actions
	//QAction * ra; // 2017.10.01 - commented unused variable
    {
	//ra = new QAction( i18n("0 Degrees"), "", 0, circuitDocument, SLOT(setOrientation0()), ac, "edit_orientation_0" );
	// ra->setExclusiveGroup("orientation"); // TODO test
        QAction *ra = new QAction( i18n("0 Degrees"), ac);
        ra->setObjectName("edit_orientation_0");
        connect(ra, SIGNAL(triggered(bool)), circuitDocument, SLOT(setOrientation0()));
        ac->addAction(ra->objectName(), ra);
        ra->setChecked(true);
    }
    {
	//ra = new QAction( i18n("90 Degrees"), "", 0, circuitDocument, SLOT(setOrientation90()), ac, "edit_orientation_90" );
	// ra->setExclusiveGroup("orientation"); // TODO test
        QAction *ra = new QAction( i18n("90 Degrees"), ac);
        ra->setObjectName("edit_orientation_90");
        connect(ra, SIGNAL(triggered(bool)), circuitDocument, SLOT(setOrientation90()));
        ac->addAction(ra->objectName(), ra);
    }
    {
	//ra = new QAction( i18n("180 Degrees"), "", 0, circuitDocument, SLOT(setOrientation180()), ac, "edit_orientation_180" );
	//ra->setExclusiveGroup("orientation"); // TODO test
        QAction *ra = new QAction( i18n("180 Degrees"), ac);
        ra->setObjectName("edit_orientation_180");
        connect(ra, SIGNAL(triggered(bool)), circuitDocument, SLOT(setOrientation180()));
        ac->addAction(ra->objectName(), ra);
    }
    {
	//ra =new QAction( i18n("270 Degrees"), "", 0, circuitDocument, SLOT(setOrientation270()), ac, "edit_orientation_270" );
	//ra->setExclusiveGroup("orientation"); // TODO test
        QAction *ra = new QAction( i18n("270 Degrees"), ac);
        ra->setObjectName("edit_orientation_270");
        connect(ra, SIGNAL(triggered(bool)), circuitDocument, SLOT(setOrientation270()));
        ac->addAction(ra->objectName(), ra);
    }

    {
	//new QAction( i18n("Create Subcircuit"), "", 0, circuitDocument, SLOT(createSubcircuit()), ac, "circuit_create_subcircuit" );
        QAction *ra = new QAction( i18n("Create Subcircuit"), ac);
        ra->setObjectName("circuit_create_subcircuit");
        connect(ra, SIGNAL(triggered(bool)), circuitDocument, SLOT(createSubcircuit()));
        ac->addAction(ra->objectName(), ra);
    }
    {
	//new QAction( i18n("Rotate Clockwise"), "object-rotate-right", "]", circuitDocument, SLOT(rotateClockwise()), ac, "edit_rotate_cw" );
        QAction *ra = new QAction( QIcon::fromTheme("object-rotate-right"), i18n("Rotate Clockwise"), ac);
        ra->setObjectName("edit_rotate_cw");
        connect(ra, SIGNAL(triggered(bool)), circuitDocument, SLOT(rotateClockwise()));
        ac->addAction(ra->objectName(), ra);
    }
    {
	//new QAction( i18n("Rotate Counter-Clockwise"), "object-rotate-left", "[", circuitDocument, SLOT(rotateCounterClockwise()), ac, "edit_rotate_ccw" );
        QAction *ra = new QAction( QIcon::fromTheme("object-rotate-left"), i18n("Rotate Counter-Clockwise"), ac);
        ra->setObjectName("edit_rotate_ccw");
        connect(ra, SIGNAL(triggered(bool)), circuitDocument, SLOT(rotateCounterClockwise()));
        ac->addAction(ra->objectName(), ra);
    }
    {
	//new QAction( i18n("Flip Horizontally"), "", 0, circuitDocument, SLOT(flipHorizontally()), ac, "edit_flip_horizontally" );
        QAction *ra = new QAction( QIcon::fromTheme("object-flip-horizontal"), i18n("Flip Horizontally"), ac);
        ra->setObjectName("edit_flip_horizontally");
        connect(ra, SIGNAL(triggered(bool)), circuitDocument, SLOT(flipHorizontally()));
        ac->addAction(ra->objectName(), ra);
    }
    {
	//new QAction( i18n("Flip Vertically"), "", 0, circuitDocument, SLOT(flipVertically()), ac, "edit_flip_vertically" );
        QAction *ra = new QAction( QIcon::fromTheme("object-flip-vertical"), i18n("Flip Vertically"), ac);
        ra->setObjectName("edit_flip_vertically");
        connect(ra, SIGNAL(triggered(bool)), circuitDocument, SLOT(flipVertically()));
        ac->addAction(ra->objectName(), ra);
    }
	//END Item Control Actions

	setXMLFile( "ktechlabcircuitui.rc", true );


	setWhatsThis( i18n(
			"Construct a circuit by dragging components from the Component selector from the left. Create the connections by dragging a wire from the component connectors.<br><br>"

			"The simulation is running by default, but can be paused and resumed from the Tools menu.<br><br>"

			"To delete a wire, select it with a select box, and hit delete.<br><br>"

			"To edit the attributes of a component, select it (making sure that no components of another type are also selected), and edit in the toolbar. More advanced properties can be edited using the item editor on the right.<br><br>"

			"Subcircuits can be created by connecting the components with an External Connection, selecting the desired components and clicking on \"Create Subcircuit\" in the right-click menu.")
				   );

	m_pViewIface = new CircuitViewIface(this);

    // note: the text below normally should not be visible; it is needed to get the "real" status displayed;
    // see slotUpdateRunningStatus
	m_statusBar->setStatusText(i18n("Simulation Initializing"));
	connect( Simulator::self(), SIGNAL(simulatingStateChanged(bool )), this, SLOT(slotUpdateRunningStatus(bool )) );
	slotUpdateRunningStatus( Simulator::self()->isSimulating() );
}


CircuitView::~CircuitView()
{
	delete m_pViewIface;
}


void CircuitView::slotUpdateRunningStatus( bool isRunning )
{
    m_statusBar->setStatusText(isRunning ? i18n("Simulation Running") : i18n("Simulation Paused"));
}


void CircuitView::dragEnterEvent( QDragEnterEvent * e )
{
    const QMimeData* mimeData = e->mimeData();
    qDebug() << Q_FUNC_INFO << mimeData->formats();

	ICNView::dragEnterEvent(e);
	if ( e->isAccepted() )
		return;
	
	//bool acceptable = e->provides("ktechlab/component") || e->provides("ktechlab/subcircuit");
    bool acceptable = e->mimeData()->hasFormat("ktechlab/component")
            || e->mimeData()->hasFormat("ktechlab/subcircuit");
	if ( !acceptable )
		return;

	e->setAccepted( true );
	createDragItem( e );
}
