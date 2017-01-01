/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
#ifndef NO_GPSIM

#include "gpsimprocessor.h"
#include "symbolviewer.h"
#include "katemdi.h"

#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kconfiggroup.h>

#include <Qt/qlabel.h>
#include <Qt/qlayout.h>

#include <cassert>

static const int NAME_COLUMN = 0;
static const int VALUE_COLUMN = 1;


//BEGIN class SymbolViewerItem
SymbolViewerItem::SymbolViewerItem( SymbolViewer * symbolViewer, RegisterInfo * registerInfo )
	: Q3ListViewItem( symbolViewer->symbolList() )
{
	assert(registerInfo);
	m_pRegisterInfo = registerInfo;
	m_pSymbolViewer = symbolViewer;
	
	setText( NAME_COLUMN, m_pRegisterInfo->name() );
// 	setText( TYPE_COLUMN, RegisterInfo::toString( m_pRegisterInfo->type() ) );
	radixChanged(); // force update of displayed string
	
	connect( m_pRegisterInfo, SIGNAL(valueChanged(unsigned)), this, SLOT(valueChanged(unsigned)) );
	connect( m_pSymbolViewer, SIGNAL(valueRadixChanged(SymbolViewer::Radix)), this, SLOT(radixChanged()) );
	
}


void SymbolViewerItem::valueChanged( unsigned newValue )
{
	setText( VALUE_COLUMN, m_pSymbolViewer->toDisplayString( newValue ) );
}


void SymbolViewerItem::radixChanged()
{
	valueChanged( m_pRegisterInfo->value() );
}
//END class SymbolViewerItem



//BEGIN class SymbolView
SymbolViewer * SymbolViewer::m_pSelf = 0l;
SymbolViewer * SymbolViewer::self( KateMDI::ToolView * parent )
{
	if (!m_pSelf)
	{
		assert (parent);
		m_pSelf = new SymbolViewer(parent);
	}
	return m_pSelf;
}

SymbolViewer::SymbolViewer( KateMDI::ToolView * parent )
	: QWidget( (QWidget*)parent )
{
    if (parent->layout()) {
        parent->layout()->addWidget(this);
        qDebug() << Q_FUNC_INFO << " added item selector to parent's layout " << parent;
    } else {
        qWarning() << Q_FUNC_INFO << " unexpected null layout on parent " << parent ;
    }

	QGridLayout  * grid = new QGridLayout( this, 1, 1, 0, 6 );
	
	m_pSymbolList = new Q3ListView(this);
	m_pSymbolList->setFocusPolicy( Qt::NoFocus );
	grid->addMultiCellWidget( m_pSymbolList, 0, 0, 0, 1 );
	
	grid->addWidget( new QLabel( i18n("Value radix:"), this ), 1, 0 );
	
	m_pRadixCombo = new KComboBox( false, this );
	grid->addWidget( m_pRadixCombo, 1, 1 );
	m_pRadixCombo->insertItem( i18n("Binary") );
	m_pRadixCombo->insertItem( i18n("Octal") );
	m_pRadixCombo->insertItem( i18n("Decimal") );
	m_pRadixCombo->insertItem( i18n("Hexadecimal") );
	m_valueRadix = Decimal;
	m_pRadixCombo->setCurrentIndex(2);
	connect( m_pRadixCombo, SIGNAL(activated(int)), this, SLOT(selectRadix(int)) );
	
	m_pGpsim = 0l;
	m_pCurrentContext = 0l;
	
	m_pSymbolList->addColumn( i18n("Name") );
	m_pSymbolList->addColumn( i18n("Value") );
	//m_pSymbolList->setFullWidth(true);
    m_pSymbolList->setColumnWidthMode(1, Q3ListView::Maximum);
	m_pSymbolList->setAllColumnsShowFocus( true );
}


SymbolViewer::~SymbolViewer()
{
}


void SymbolViewer::saveProperties( KConfig * config )
{
	//QString oldGroup = config->group();
	
	KConfigGroup grSym = config->group( "SymbolEditor" );
	grSym.writeEntry( "Radix", (int) m_valueRadix );
	
	//config->setGroup( oldGroup );
}


void SymbolViewer::readProperties( KConfig * config )
{
	//QString oldGroup = config->group();

	
	KConfigGroup grSym = config->group( "SymbolEditor" );

    m_valueRadix = (SymbolViewer::Radix)grSym.readEntry( "Radix", (int) Decimal );
	
	int pos = 4;
	switch ( m_valueRadix )
	{
		case Binary:
			pos--;
		case Octal:
			pos--;
		case Decimal:
			pos--;
		case Hexadecimal:
			pos--;
	}
	m_pRadixCombo->setCurrentIndex( pos );
	
	//config->setGroup( oldGroup );
}


void SymbolViewer::setContext( GpsimProcessor * gpsim )
{
	RegisterSet * registerSet = gpsim ? gpsim->registerMemory() : 0l;
	
	if ( registerSet == m_pCurrentContext )
		return;
	
	m_pSymbolList->clear();
	m_pGpsim = gpsim;
	m_pCurrentContext = registerSet;
	
	if (!m_pCurrentContext)
		return;
		
	connect( gpsim, SIGNAL(destroyed()), m_pSymbolList, SLOT(clear()) );
	
	unsigned count = m_pCurrentContext->size();
	for ( unsigned i = 0; i < count; ++i )
	{
		RegisterInfo * reg = m_pCurrentContext->fromAddress(i);
		
		if ( (reg->type() == RegisterInfo::Generic) ||
					(reg->type() == RegisterInfo::Invalid) )
		continue;
		
		new SymbolViewerItem( this, reg );
	}
}


void SymbolViewer::selectRadix( int selectIndex )
{
	if ( (selectIndex<0) || (selectIndex>3) )
	{
		kWarning() << k_funcinfo << "Invalid select position for radix: " << selectIndex << endl;
		return;
	}
	
	Radix radii[] = { Binary, Octal, Decimal, Hexadecimal };
	Radix newRadix = radii[selectIndex];
	
	if ( newRadix == m_valueRadix )
		return;
	
	m_valueRadix = newRadix;
	
	emit valueRadixChanged(m_valueRadix);
}


QString SymbolViewer::toDisplayString( unsigned value ) const
{
	switch ( m_valueRadix )
	{
		case Binary:
			return QString::number( value, 2 ).rightJustify( 8, '0', false );
			
		case Octal:
			return "0" + QString::number( value, 8 );
			
		case Decimal:
			return QString::number( value, 10 );
			
		case Hexadecimal:
			return "0x" + QString::number( value, 16 );
	}
	
	return "?";
}
//END class SymbolView

#include "symbolviewer.moc"

#endif
