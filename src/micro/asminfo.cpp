/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "asminfo.h"

#include <kdebug.h>

AsmInfo::AsmInfo()
{
}


AsmInfo::~AsmInfo()
{
}


void AsmInfo::addInstruction( const QString & operand, const QString & description, const QString & opcode )
{
	Instruction instruction;
	instruction.operand = operand;
	instruction.description = description;
	instruction.opcode = opcode;
	m_instructionList.append( instruction );
	m_operandList.append( operand );
}


QString AsmInfo::setToString( Set set )
{
	switch (set)
	{
		case AsmInfo::PIC12:
			return "PIC12";
			
		case AsmInfo::PIC14:
			return "PIC14";
			
		case AsmInfo::PIC16:
			return "PIC16";
	}
	
	kWarning() << k_funcinfo << "Unrecognized set="<<set<<endl;
	return QString::null;
}


AsmInfo::Set AsmInfo::stringToSet( const QString & set )
{
	if ( set == "PIC12" )
		return PIC12;
	
	if ( set == "PIC14" )
		return PIC14;
	
	if ( set == "PIC16" )
		return PIC16;
	
// 	kWarning() << k_funcinfo << "Unrecognized set="<<set<<endl;
	return PIC14;
}
