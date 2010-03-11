/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "language.h"
#include "microselectwidget.h"
#include "picprogrammer.h"
#include "port.h"
#include "programmerdlg.h"
#include "programmerwidget.h"
#include "src/core/ktlconfig.h"

#include <kcombobox.h>
#include <kguiitem.h>
#include <klocale.h>
#include <kstdguiitem.h>

ProgrammerDlg::ProgrammerDlg( const QString & picID, QWidget *parent, const char *name )
	: KDialogBase( parent, name, true, i18n("PIC Programmer"), Ok|Cancel )
{
	// Change the "Ok" button to a "Burn" button
	KGuiItem burnItem = KStdGuiItem::ok();
	burnItem.setText( i18n("Burn") );
	setButtonOK( burnItem );
	
	m_bAccepted = false;
	m_pProgrammerWidget = new ProgrammerWidget( this );
	m_pProgrammerSettings = new PicProgrammerSettings;
	
	// Setup the list of programmers
	KComboBox * programmerCombo = m_pProgrammerWidget->m_pProgrammerProgram;
	QStringList programmerNames = m_pProgrammerSettings->configNames( false );
	programmerCombo->insertStringList( programmerNames );
	programmerCombo->setSizeLimit( programmerNames.size() );
	programmerCombo->setCurrentText( KTLConfig::picProgrammerProgram() );
	
	// Sets up the list of ports
	m_pProgrammerWidget->m_pPicProgrammerPort->insertStringList( Port::ports( Port::ExistsAndRW ) );
	m_pProgrammerWidget->m_pPicProgrammerPort->setCurrentText( KTLConfig::picProgrammerPort() );
	
	// Set the pic type to the one requested
	if ( !picID.isEmpty() )
		m_pProgrammerWidget->m_pMicroSelect->setMicro( picID );
	
	setMainWidget( m_pProgrammerWidget );
}


ProgrammerDlg::~ProgrammerDlg()
{
}


void ProgrammerDlg::initOptions( ProcessOptions * options )
{
	if ( !options )
		return;
	
	options->m_picID = m_pProgrammerWidget->m_pMicroSelect->micro();
	options->m_port = m_pProgrammerWidget->m_pPicProgrammerPort->currentText();
	options->m_program = m_pProgrammerWidget->m_pProgrammerProgram->currentText();
}


void ProgrammerDlg::accept()
{
	m_bAccepted = true;
	hide();
}


void ProgrammerDlg::reject()
{
	m_bAccepted = false;
}


MicroSelectWidget * ProgrammerDlg::microSelect( ) const
{
	return m_pProgrammerWidget->m_pMicroSelect;
}


#include "programmerdlg.moc"
