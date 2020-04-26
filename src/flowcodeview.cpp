/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "flowcodedocument.h"
#include "flowcodeview.h"
#include "ktechlab.h"
#include "viewiface.h"

#include <KLocalizedString>
#include <KToolBarPopupAction>
#include <KActionCollection>

#include <QIcon>
#include <QMenu>
#include <QDragEnterEvent>
#include <QMimeData>

FlowCodeView::FlowCodeView( FlowCodeDocument * flowCodeDocument, ViewContainer *viewContainer, uint viewAreaId, const char *name )
	: ICNView( flowCodeDocument, viewContainer, viewAreaId, name )
{
	KActionCollection * ac = actionCollection();

	//BEGIN Convert To * Actions
	//KToolBarPopupAction * pa = new KToolBarPopupAction( i18n("Convert to..."), "fork", 0, 0, 0, ac, "program_convert" );
    KToolBarPopupAction * pa = new KToolBarPopupAction( QIcon::fromTheme("fork"), i18n("Convert to..."), ac);
    pa->setObjectName( "program_convert" );
	pa->setDelayed(false);

	QMenu * m = pa->menu();

	m->setTitle( i18n("Convert To") );
	m->addAction( QIcon::fromTheme( "convert_to_microbe" ), i18n("Microbe"))->setData( FlowCodeDocument::MicrobeOutput );
	m->addAction( QIcon::fromTheme( "convert_to_assembly" ), i18n("Assembly"))->setData( FlowCodeDocument::AssemblyOutput );
	m->addAction( QIcon::fromTheme( "convert_to_hex" ), i18n("Hex"))->setData( FlowCodeDocument::HexOutput );
	m->addAction( QIcon::fromTheme( "convert_to_pic" ), i18n("PIC (upload)"))->setData( FlowCodeDocument::PICOutput );
	connect( m, SIGNAL(triggered(QAction*)), flowCodeDocument, SLOT(slotConvertTo(QAction*)) );
    ac->addAction("program_convert" , pa);
	//END Convert To * Actions




// 	new KAction( i18n("Convert to Microbe"), "convert_to_microbe", Qt::Key_F7, flowCodeDocument, SLOT(convertToMicrobe()), ac, "tools_to_microbe" );
// 	new KAction( i18n("Convert to Assembly"), "convert_to_assembly", Qt::Key_F8, flowCodeDocument, SLOT(convertToAssembly()), ac, "tools_to_assembly" );
// 	new KAction( i18n("Convert to Hex"), "convert_to_hex", Qt::Key_F9, flowCodeDocument, SLOT(convertToHex()), ac, "tools_to_hex" );
// 	new KAction( i18n("Upload PIC Program"), "convert_to_pic", 0, flowCodeDocument, SLOT(convertToPIC()), ac, "tools_to_pic" );




	setXMLFile( "ktechlabflowcodeui.rc", true );

	setWhatsThis( i18n(
			"Construct a FlowCode document by dragging FlowParts from the list on the left. All FlowCharts require an initial \"Start\" part, of which there can only be one.<br><br>"

			"Some FlowParts, such as Subroutines, act as a container element for other FlowParts. Drag the items in or out of a container as appropriate. The container that will become the parent of the part being dragged is indicated by being selected.<br><br>"

			"Note that connections cannot be made between FlowParts in different containers, or at different levels."
							   ) );

	m_pViewIface = new FlowCodeViewIface(this);
}


FlowCodeView::~FlowCodeView()
{
	delete m_pViewIface;
}


void FlowCodeView::dragEnterEvent( QDragEnterEvent * e )
{
	ICNView::dragEnterEvent(e);
	if ( e->isAccepted() )
		return;

	//bool acceptable = e->provides("ktechlab/flowpart");
    bool acceptable = e->mimeData()->hasFormat("ktechlab/flowpart");

	if ( !acceptable )
		return;

	e->setAccepted( true );
	createDragItem( e );
}
