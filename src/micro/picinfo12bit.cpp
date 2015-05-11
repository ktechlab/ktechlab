/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "micropackage.h"
#include "picinfo12bit.h"

#include <klocale.h>
#include <k3staticdeleter.h>

PicAsm12bit *PicAsm12bit::m_self = 0;
static K3StaticDeleter<PicAsm12bit> picAsm12BitStaticDeleter;

PicAsm12bit *PicAsm12bit::self()
{
	if ( !m_self )
		picAsm12BitStaticDeleter.setObject( m_self, new PicAsm12bit() );
	return m_self;
}


PicInfo12bit::PicInfo12bit()
	: PicInfo()
{
}


PicInfo12bit::~PicInfo12bit()
{
}

PicAsm12bit::PicAsm12bit()
	: AsmInfo()
{
	// Byte-orientated file register operations
	addInstruction( "ADDWF",	0,	"000111dfffff" );
	addInstruction( "ANDWF",	0,	"000101dfffff" );
	addInstruction( "CLRF",		0,	"0000011fffff" );
	addInstruction( "CLRW",		0,	"000001000000" );
	addInstruction( "COMF",		0,	"001001dfffff" );
	addInstruction( "DECF",		0,	"000011dfffff" );
	addInstruction( "DECFSZ",	0,	"001011dfffff" );
	addInstruction( "INCF",		0,	"001010dfffff" );
	addInstruction( "INCFSZ",	0,	"001111dfffff" );
	addInstruction( "IORWF",	0,	"000100dfffff" );
	addInstruction( "MOVF",		0,	"001000dfffff" );
	addInstruction( "MOVWF",	0,	"0000001fffff" );
	addInstruction( "NOP",		0,	"000000000000" );
	addInstruction( "RLF",		0,	"001101dfffff" );
	addInstruction( "RRF",		0,	"001100dfffff" );
	addInstruction( "SUBWF",	0,	"000010dfffff" );
	addInstruction( "SWAPF",	0,	"001110dfffff" );
	addInstruction( "XORWF",	0,	"000110dfffff" );
	
	// Bit-orientated file register operations
	addInstruction( "BCF",		0,	"0100bbbfffff" );
	addInstruction( "BSF",		0,	"0101bbbfffff" );
	addInstruction( "BTFSC",	0,	"0110bbbfffff" );
	addInstruction( "BTFSS",	0,	"0111bbbfffff" );
	
	// Literal and control operations
	addInstruction( "ANDLW",	0,	"1110kkkkkkkk" );
	addInstruction( "CALL",		0,	"1001kkkkkkkk" );
	addInstruction( "CLRWDT",	0,	"000000000100" );
	addInstruction( "GOTO",		0,	"101kkkkkkkkk" );
	addInstruction( "IORLW",	0,	"1101kkkkkkkk" );
	addInstruction( "MOVLW",	0,	"1100kkkkkkkk" );
// 	addInstruction( "RETFIE",	0,	"00000000001001" );
	addInstruction( "OPTION",	0,	"000000000010" );
	addInstruction( "RETLW",	0,	"1000kkkkkkkk" );
// 	addInstruction( "RETURN",	0,	"00000000001000" );
	addInstruction( "SLEEP",	0,	"000000000011" );
// 	addInstruction( "SUBLW",	0,	"11110xkkkkkkkk" );
	addInstruction( "TRIS",		0,	"000000000fff" );
	addInstruction( "XORLW",	0,	"1111kkkkkkkk" );
}


PicInfo16C54::PicInfo16C54()
	: PicInfo12bit()
{
	m_id = "P16C54";
	
	delete m_package;
	m_package = new MicroPackage(18);
	
	m_package->assignPin( 17,	PicPin::type_bidir,	"RA0",	"PORTA", 0 );
	m_package->assignPin( 18,	PicPin::type_bidir,	"RA1",	"PORTA", 1 );
	m_package->assignPin( 1,	PicPin::type_bidir,	"RA2",  "PORTA", 2 );
	m_package->assignPin( 2,	PicPin::type_bidir,	"RA3",  "PORTA", 3 );
	m_package->assignPin( 3,	PicPin::type_open,	"RA4",  "PORTA", 4 );
	
	m_package->assignPin( 6,	PicPin::type_bidir,	"RB0",  "PORTB", 0 );
	m_package->assignPin( 7,	PicPin::type_bidir,	"RB1",  "PORTB", 1 );
	m_package->assignPin( 8,	PicPin::type_bidir,	"RB2",  "PORTB", 2 );
	m_package->assignPin( 9,	PicPin::type_bidir,	"RB3",  "PORTB", 3 );
	m_package->assignPin( 10,	PicPin::type_bidir,	"RB4",	"PORTB", 4 );
	m_package->assignPin( 11,	PicPin::type_bidir,	"RB5",	"PORTB", 5 );
	m_package->assignPin( 12,	PicPin::type_bidir,	"RB6",	"PORTB", 6 );
	m_package->assignPin( 13,	PicPin::type_bidir,	"RB7",	"PORTB", 7 );
	
	m_package->assignPin( 4,	PicPin::type_mclr,	"MCLR" );
	m_package->assignPin( 5,	PicPin::type_vss,	"VSS" );
	m_package->assignPin( 14,	PicPin::type_vdd,	"VDD" );
	m_package->assignPin( 15,	PicPin::type_osc,	"OSC2" );
	m_package->assignPin( 16,	PicPin::type_osc,	"OSC1" );
}

PicInfo16C54::~PicInfo16C54()
{
}

PicInfo16C55::PicInfo16C55()
	: PicInfo12bit()
{
	m_id = "P16C55";
}

PicInfo16C55::~PicInfo16C55()
{
}

PicInfo12C508::PicInfo12C508()
	: PicInfo12bit()
{
	m_id = "P12C508";
	
	delete m_package;
	m_package = new MicroPackage(8);
	
	m_package->assignPin( 7, PicPin::type_bidir, "GP0",	"GPIO", 0 );
	m_package->assignPin( 6, PicPin::type_bidir, "GP1",	"GPIO", 1 );
	m_package->assignPin( 5, PicPin::type_bidir, "GP2",	"GPIO", 2 );
	m_package->assignPin( 4, PicPin::type_input, "GP3",	"GPIO", 3 );
	m_package->assignPin( 3, PicPin::type_bidir, "GP4",	"GPIO", 4 );
	m_package->assignPin( 2, PicPin::type_bidir, "GP5",	"GPIO", 5 );
	
	m_package->assignPin( 8, PicPin::type_vss, "VSS" );
	m_package->assignPin( 1, PicPin::type_vdd, "VDD" );
}

PicInfo12C508::~PicInfo12C508()
{
}

PicInfo12C509::PicInfo12C509()
	: PicInfo12C508()
{
	m_id = "P12C509";
}

PicInfo12C509::~PicInfo12C509()
{
}

PicInfo12C671::PicInfo12C671()
	: PicInfo12C508()
{
	m_id = "P12C671";
}

PicInfo12C671::~PicInfo12C671()
{
}

PicInfo12C672::PicInfo12C672()
	: PicInfo12C508()
{
	m_id = "P12C672";
}

PicInfo12C672::~PicInfo12C672()
{
}

