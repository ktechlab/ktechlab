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
#include "picinfo16bit.h"

#include <klocalizedstring.h>
// #include <k3staticdeleter.h>
#include <kglobal.h>

// PicAsm16bit *PicAsm16bit::m_self = 0;
// static K3StaticDeleter<PicAsm16bit> picAsm16BitStaticDeleter;

K_GLOBAL_STATIC(PicAsm16bit, globalPicAsm16bit);

PicAsm16bit *PicAsm16bit::self()
{
// 	if ( !m_self )
// 		picAsm16BitStaticDeleter.setObject( m_self, new PicAsm16bit() );
// 	return m_self;
    return globalPicAsm16bit;
}

PicInfo16bit::PicInfo16bit()
	: PicInfo()
{
}


PicInfo16bit::~PicInfo16bit()
{
}


PicAsm16bit::PicAsm16bit()
	: AsmInfo()
{
	// 16 bit Asm instructions for PICs

	// Byte-orientated file register operations
	addInstruction( "ADDWF",	0,	"000111dfffffff" );
	addInstruction( "ANDWF",	0,	"000101dfffffff" );
	addInstruction( "CLRF",		0,	"0000011fffffff" );
	addInstruction( "CLRW",		0,	"0000010xxxxxxx" );
	addInstruction( "COMF",		0,	"001001dfffffff" );
	addInstruction( "DECF",		0,	"000011dfffffff" );
	addInstruction( "DECFSZ",	0,	"001011dfffffff" );
	addInstruction( "INCF",		0,	"001010dfffffff" );
	addInstruction( "INCFSZ",	0,	"001111dfffffff" );
	addInstruction( "IORWF",	0,	"000100dfffffff" );
	addInstruction( "MOVF",		0,	"001000dfffffff" );
	addInstruction( "MOVWF",	0,	"0000001fffffff" );
	addInstruction( "NOP",		0,	"0000000xx00000" );
	addInstruction( "RLF",		0,	"001101dfffffff" );
	addInstruction( "RRF",		0,	"001100dfffffff" );
	addInstruction( "SUBWF",	0,	"000010dfffffff" );
	addInstruction( "SWAPF",	0,	"001110dfffffff" );
	addInstruction( "XORWF",	0,	"000110dfffffff" );
	
	// Bit-orientated file register operations
	addInstruction( "BCF",		0,	"0100bbbfffffff" );
	addInstruction( "BSF",		0,	"0101bbbfffffff" );
	addInstruction( "BTFSC",	0,	"0110bbbfffffff" );
	addInstruction( "BTFSS",	0,	"0111bbbfffffff" );
	
	// Literal and control operations
	addInstruction( "ADDLW",	0,	"11111xkkkkkkkk" );
	addInstruction( "ANDLW",	0,	"111001kkkkkkkk" );
	addInstruction( "CALL",		0,	"100kkkkkkkkkkk" );
	addInstruction( "CLRWDT",	0,	"00000001100100" );
	addInstruction( "GOTO",		0,	"101kkkkkkkkkkk" );
	addInstruction( "IORLW",	0,	"111000kkkkkkkk" );
	addInstruction( "MOVLW",	0,	"1100xxkkkkkkkk" );
	addInstruction( "RETFIE",	0,	"00000000001001" );
	addInstruction( "RETLW",	0,	"1101xxkkkkkkkk" );
	addInstruction( "RETURN",	0,	"00000000001000" );
	addInstruction( "SLEEP",	0,	"00000000000011" );
	addInstruction( "SUBLW",	0,	"11110xkkkkkkkk" );
	addInstruction( "XORLW",	0,	"111010kkkkkkkk" );
}


PicInfo17C7xx::PicInfo17C7xx()
	: PicInfo16bit()
{
	m_id = "P17C7xx";
}

PicInfo17C7xx::~PicInfo17C7xx()
{
}

PicInfo17C75x::PicInfo17C75x()
	: PicInfo17C7xx()
{
	m_id = "P17C75x";
}

PicInfo17C75x::~PicInfo17C75x()
{
}

PicInfo17C752::PicInfo17C752()
	: PicInfo17C75x()
{
	m_id = "P17C752";
}

PicInfo17C752::~PicInfo17C752()
{
}

PicInfo17C756::PicInfo17C756()
	: PicInfo17C75x()
{
	m_id = "P17C756";
}

PicInfo17C756::~PicInfo17C756()
{
}

PicInfo17C756A::PicInfo17C756A()
	: PicInfo17C75x()
{
	m_id = "P17C756A";
}

PicInfo17C756A::~PicInfo17C756A()
{
}

PicInfo17C762::PicInfo17C762()
	: PicInfo17C75x()
{
	m_id = "P17C762";
}

PicInfo17C762::~PicInfo17C762()
{
}

PicInfo17C766::PicInfo17C766()
	: PicInfo17C75x()
{
	m_id = "P17C766";
}

PicInfo17C766::~PicInfo17C766()
{
}

PicInfo18Cxx2::PicInfo18Cxx2()
	: PicInfo16bit()
{
	m_id = "P18Cxx2";
}

PicInfo18Cxx2::~PicInfo18Cxx2()
{
}

PicInfo18C2x2::PicInfo18C2x2()
	: PicInfo16bit()
{
	m_id = "P18C2x2";
}

PicInfo18C2x2::~PicInfo18C2x2()
{
}

PicInfo18C242::PicInfo18C242()
	: PicInfo18C2x2()
{
	m_id = "P18C242";
}


PicInfo18C242::~PicInfo18C242()
{
}

PicInfo18C252::PicInfo18C252()
	: PicInfo18C242()
{
	m_id = "P18C252";
}

PicInfo18C252::~PicInfo18C252()
{
}

PicInfo18C4x2::PicInfo18C4x2()
	: PicInfo16bit()
{
	m_id = "P18C4x2";
	
	delete m_package;
	m_package = new MicroPackage(40);
	
	m_package->assignPin( 2,	PicPin::type_bidir,	"RA0",	"PORTA", 0 );
	m_package->assignPin( 3,	PicPin::type_bidir,	"RA1",	"PORTA", 1 );
	m_package->assignPin( 4,	PicPin::type_bidir,	"RA2",  "PORTA", 2 );
	m_package->assignPin( 5,	PicPin::type_bidir,	"RA3",  "PORTA", 3 );
	m_package->assignPin( 6,	PicPin::type_open,	"RA4",  "PORTA", 4 );
	m_package->assignPin( 7,	PicPin::type_bidir,	"RA5",  "PORTB", 5 );
	
	m_package->assignPin( 33,	PicPin::type_bidir,	"RB0",  "PORTB", 0 );
	m_package->assignPin( 34,	PicPin::type_bidir,	"RB1",  "PORTB", 1 );
	m_package->assignPin( 35,	PicPin::type_bidir,	"RB2",  "PORTB", 2 );
	m_package->assignPin( 36,	PicPin::type_bidir,	"RB3",	"PORTB", 3 );
	m_package->assignPin( 37,	PicPin::type_bidir,	"RB4",	"PORTB", 4 );
	m_package->assignPin( 38,	PicPin::type_bidir,	"RB5",	"PORTB", 5 );
	m_package->assignPin( 39,	PicPin::type_bidir,	"RB6",	"PORTB", 6 );
	m_package->assignPin( 40,	PicPin::type_bidir,	"RB7",	"PORTB", 7 );
	
	m_package->assignPin( 15,	PicPin::type_bidir,	"RC0",  "PORTC", 0 );
	m_package->assignPin( 16,	PicPin::type_bidir,	"RC1",  "PORTC", 1 );
	m_package->assignPin( 17,	PicPin::type_bidir,	"RC2",  "PORTC", 2 );
	m_package->assignPin( 18,	PicPin::type_bidir,	"RC3",	"PORTC", 3 );
	m_package->assignPin( 23,	PicPin::type_bidir,	"RC4",	"PORTC", 4 );
	m_package->assignPin( 24,	PicPin::type_bidir,	"RC5",	"PORTC", 5 );
	m_package->assignPin( 25,	PicPin::type_bidir,	"RC6",	"PORTC", 6 );
	m_package->assignPin( 26,	PicPin::type_bidir,	"RC7",	"PORTC", 7 );
	
	m_package->assignPin( 19,	PicPin::type_bidir,	"RD0",  "PORTD", 0 );
	m_package->assignPin( 20,	PicPin::type_bidir,	"RD1",  "PORTD", 1 );
	m_package->assignPin( 21,	PicPin::type_bidir,	"RD2",  "PORTD", 2 );
	m_package->assignPin( 22,	PicPin::type_bidir,	"RD3",	"PORTD", 3 );
	m_package->assignPin( 27,	PicPin::type_bidir,	"RD4",	"PORTD", 4 );
	m_package->assignPin( 28,	PicPin::type_bidir,	"RD5",	"PORTD", 5 );
	m_package->assignPin( 29,	PicPin::type_bidir,	"RD6",	"PORTD", 6 );
	m_package->assignPin( 30,	PicPin::type_bidir,	"RD7",	"PORTD", 7 );
	
	m_package->assignPin( 8,	PicPin::type_bidir,	"RE0",  "PORTE", 0 );
	m_package->assignPin( 9,	PicPin::type_bidir,	"RE1",  "PORTE", 1 );
	m_package->assignPin( 10,	PicPin::type_bidir,	"RE2",  "PORTE", 2 );
	
	m_package->assignPin( 1,	PicPin::type_mclr,	"MCLR" );
	m_package->assignPin( 11,	PicPin::type_vdd,	"VDD"  );
	m_package->assignPin( 12,	PicPin::type_vss,	"VSS"  );
	m_package->assignPin( 13,	PicPin::type_osc,	"OSC1" );
	m_package->assignPin( 14,	PicPin::type_osc,	"OSC2" );
	m_package->assignPin( 31,	PicPin::type_vss,	"VSS"  );
	m_package->assignPin( 32,	PicPin::type_vdd,	"VDD"  );
}

PicInfo18C4x2::~PicInfo18C4x2()
{
	delete m_package;
	m_package = 0l;
}

PicInfo18C442::PicInfo18C442()
	: PicInfo18C4x2()
{
	m_id = "P18C442";
}

PicInfo18C442::~PicInfo18C442()
{
}

PicInfo18C452::PicInfo18C452()
	: PicInfo18C442()
{
	m_id = "P18C452";
}

PicInfo18C452::~PicInfo18C452()
{
}

PicInfo18F442::PicInfo18F442()
	: PicInfo18C442()
{
	m_id = "P18F442";
}

PicInfo18F442::~PicInfo18F442()
{
}

PicInfo18F248::PicInfo18F248()
	: PicInfo18F442()
{
	m_id = "P18F248";
}

PicInfo18F248::~PicInfo18F248()
{
}

PicInfo18F452::PicInfo18F452()
	: PicInfo18F442()
{
	m_id = "P18F452";
}

PicInfo18F452::~PicInfo18F452()
{
}

PicInfo18Fxx20::PicInfo18Fxx20()
	: PicInfo16bit()
{
	m_id = "P18Fxx20";
}

PicInfo18Fxx20::~PicInfo18Fxx20()
{
}

PicInfo18F1220::PicInfo18F1220()
	: PicInfo18Fxx20()
{
	m_id = "P18F1220";
	
	delete m_package;
	m_package = new MicroPackage(18);
	
	m_package->assignPin( 1,	PicPin::type_bidir,	"RA0",	"PORTA", 0 );
	m_package->assignPin( 2,	PicPin::type_bidir,	"RA1",	"PORTA", 1 );
	m_package->assignPin( 6,	PicPin::type_bidir,	"RA2",  "PORTA", 2 );
	m_package->assignPin( 7,	PicPin::type_bidir,	"RA3",  "PORTA", 3 );
	m_package->assignPin( 3,	PicPin::type_open,	"RA4",  "PORTA", 4 );
	m_package->assignPin( 4,	PicPin::type_open,	"RA5",  "PORTA", 5 );
	m_package->assignPin( 15,	PicPin::type_open,	"RA6",  "PORTA", 6 );
	m_package->assignPin( 16,	PicPin::type_open,	"RA7",  "PORTA", 7 );
	
	m_package->assignPin( 8,	PicPin::type_bidir,	"RB0",  "PORTB", 0 );
	m_package->assignPin( 9,	PicPin::type_bidir,	"RB1",  "PORTB", 1 );
	m_package->assignPin( 17,	PicPin::type_bidir,	"RB2",  "PORTB", 2 );
	m_package->assignPin( 18,	PicPin::type_bidir,	"RB3",  "PORTB", 3 );
	m_package->assignPin( 10,	PicPin::type_bidir,	"RB4",	"PORTB", 4 );
	m_package->assignPin( 11,	PicPin::type_bidir,	"RB5",	"PORTB", 5 );
	m_package->assignPin( 12,	PicPin::type_bidir,	"RB6",	"PORTB", 6 );
	m_package->assignPin( 13,	PicPin::type_bidir,	"RB7",	"PORTB", 7 );
	
	m_package->assignPin( 5,	PicPin::type_vss,	"VSS" );
	m_package->assignPin( 14,	PicPin::type_vdd,	"VDD" );
}

PicInfo18F1220::~PicInfo18F1220()
{

}

PicInfo18F1320::PicInfo18F1320()
	: PicInfo18F1220()
{
	m_id = "P18F1320";
}

PicInfo18F1320::~PicInfo18F1320()
{
}

