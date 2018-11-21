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
#include "textview.h"
#include "variablelabel.h"


//BEGIN class VariableLabel
VariableLabel::VariableLabel( TextView * parent )
	: QLabel( parent, Qt::WStyle_StaysOnTop | Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool | Qt::WX11BypassWM )
{
    setObjectName("toolTipTip");

	m_value = -1;
	
	setMargin(1);
	//setAutoMask( false ); // TODO is this needed?
	setFrameStyle( QFrame::Plain | QFrame::Box );
	setLineWidth( 1 );
	setAlignment( Qt::AlignAuto | Qt::AlignTop );
	setIndent(0);
	ensurePolished();
	adjustSize();
}


void VariableLabel::setRegister( RegisterInfo * info, const QString & name )
{
	disconnectRegisterInfo();
	
	if ( !info )
		return;
	
	m_value = -1;
	m_pRegisterInfo = info;
	m_registerName = name;
	
	connect( m_pRegisterInfo, SIGNAL(destroyed()), this, SLOT(hide()) );
	connect( m_pRegisterInfo, SIGNAL(valueChanged(unsigned)), this, SLOT(updateText()) );
	
	updateText();
}


void VariableLabel::disconnectRegisterInfo()
{
	if ( !m_pRegisterInfo )
		return;
	
	disconnect( m_pRegisterInfo, SIGNAL(destroyed()), this, SLOT(hide()) );
	disconnect( m_pRegisterInfo, SIGNAL(valueChanged(unsigned)), this, SLOT(updateText()) );
	
	m_pRegisterInfo = 0l;
	m_registerName = QString::null;
}


void VariableLabel::setValue( unsigned value )
{
	disconnectRegisterInfo();
	m_value = value;
	
	updateText();
}


void VariableLabel::updateText()
{
	if ( m_pRegisterInfo )
		setText( QString("%1 = %2").arg( m_registerName ).arg( SymbolViewer::self()->toDisplayString( m_pRegisterInfo->value() ) ) );
	
	else if ( m_value != -1 )
		setText( QString::number( m_value ) );
	
	adjustSize();
}
//END class VariableLabel

#include "variablelabel.moc"

#endif // !NO_GPSIM
