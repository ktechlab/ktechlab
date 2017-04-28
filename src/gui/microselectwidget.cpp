/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "asminfo.h"
#include "microinfo.h"
#include "microlibrary.h"
#include "microselectwidget.h"

#include <kcombobox.h>
#include <klocalizedstring.h>

#include <q3groupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvariant.h>

MicroSelectWidget::MicroSelectWidget( QWidget* parent, const char* name, Qt::WFlags )
	: Q3GroupBox( 4, Qt::Horizontal, i18n("Microprocessor"), parent, name )
{
	m_allowedAsmSet = AsmInfo::AsmSetAll;
	m_allowedGpsimSupport = m_allowedFlowCodeSupport = m_allowedMicrobeSupport = MicroInfo::AllSupport;
	
	if ( !name )
		setName( "MicroSelectWidget" );

	m_pMicroFamilyLabel = new QLabel( this, "m_pMicroFamilyLabel" );
	m_pMicroFamilyLabel->setText( i18n("Family") );

	m_pMicroFamily = new KComboBox( false, this ); //, "m_pMicroFamily" );
	m_pMicroFamily->setName("m_pMicroFamily");
	m_pMicroFamily->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );

	m_pMicroLabel = new QLabel( this, "m_pMicroLabel" );
	m_pMicroLabel->setText( i18n("Micro") );

	m_pMicro = new KComboBox( false, this ); //, "m_pMicro" );
	m_pMicro->setName("m_pMicro");
	m_pMicro->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	m_pMicro->setEditable( true );
	m_pMicro->setAutoCompletion(true);
	updateFromAllowed();
	setMicro("P16F84");
	connect( m_pMicroFamily, SIGNAL(activated(const QString & )), this, SLOT(microFamilyChanged(const QString& )) );
}


MicroSelectWidget::~MicroSelectWidget()
{
}

void MicroSelectWidget::setAllowedAsmSet( unsigned allowed )
{
	m_allowedAsmSet = allowed;
	updateFromAllowed();
}
void MicroSelectWidget::setAllowedGpsimSupport( unsigned allowed )
{
	m_allowedGpsimSupport = allowed;
	updateFromAllowed();
}
void MicroSelectWidget::setAllowedFlowCodeSupport( unsigned allowed )
{
	m_allowedFlowCodeSupport = allowed;
	updateFromAllowed();
}	
void MicroSelectWidget::setAllowedMicrobeSupport( unsigned allowed )
{
	m_allowedMicrobeSupport = allowed;
	updateFromAllowed();
}


void MicroSelectWidget::updateFromAllowed()
{
	QString oldFamily = m_pMicroFamily->currentText();
	
	m_pMicroFamily->clear();
	
#define CHECK_ADD(family) if ( (m_allowedAsmSet & AsmInfo::family) && !MicroLibrary::self()->microIDs( AsmInfo::family, m_allowedGpsimSupport, m_allowedFlowCodeSupport, m_allowedMicrobeSupport ).isEmpty() ) m_pMicroFamily->insertItem( AsmInfo::setToString(AsmInfo::family) );
	CHECK_ADD(PIC12)
	CHECK_ADD(PIC14)
	CHECK_ADD(PIC16);
#undef CHECK_ADD

	if ( m_pMicroFamily->contains(oldFamily) )
		m_pMicroFamily->setCurrentText(oldFamily);
	
	microFamilyChanged(oldFamily);
}


void MicroSelectWidget::setMicro( const QString & id )
{
	MicroInfo * info = MicroLibrary::self()->microInfoWithID(id);
	if (!info)
		return;
	
	m_pMicro->clear();
	m_pMicro->insertStringList( MicroLibrary::self()->microIDs( info->instructionSet()->set() ) );
	m_pMicro->setCurrentText(id);
	
	m_pMicroFamily->setCurrentText( AsmInfo::setToString( info->instructionSet()->set() ) );
}


QString MicroSelectWidget::micro() const
{
	return m_pMicro->currentText();
}


void MicroSelectWidget::microFamilyChanged( const QString & family )
{
	QString oldID = m_pMicro->currentText();
	
	m_pMicro->clear();
	m_pMicro->insertStringList( MicroLibrary::self()->microIDs( AsmInfo::stringToSet(family), m_allowedGpsimSupport, m_allowedFlowCodeSupport, m_allowedMicrobeSupport ) );
	
	if ( m_pMicro->contains(oldID) )
		m_pMicro->setCurrentText(oldID);
}

#include "microselectwidget.moc"
