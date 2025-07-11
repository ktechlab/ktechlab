/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "picinfo14bit.h"
#include "micropackage.h"

#include <KLocalizedString>
// #include <k3staticdeleter.h>

#include <QGlobalStatic>
#include <QLatin1StringView>

using Qt::Literals::operator""_L1;

// PicAsm14bit *PicAsm14bit::m_self = 0;
// static K3StaticDeleter<PicAsm14bit> picAsm14BitStaticDeleter;

Q_GLOBAL_STATIC(PicAsm14bit, globalPicAsm14bit);

PicAsm14bit *PicAsm14bit::self()
{
    //	if ( !m_self )
    //		picAsm14BitStaticDeleter.setObject( m_self, new PicAsm14bit() );
    //	return m_self;
    return globalPicAsm14bit;
}

PicInfo14bit::PicInfo14bit()
    : PicInfo()
{
}

PicInfo14bit::~PicInfo14bit()
{
}

PicAsm14bit::PicAsm14bit()
    : AsmInfo()
{
    // Byte-orientated file register operations
    addInstruction("ADDWF", nullptr, "000111dfffffff");
    addInstruction("ANDWF", nullptr, "000101dfffffff");
    addInstruction("CLRF", nullptr, "0000011fffffff");
    addInstruction("CLRW", nullptr, "0000010xxxxxxx");
    addInstruction("COMF", nullptr, "001001dfffffff");
    addInstruction("DECF", nullptr, "000011dfffffff");
    addInstruction("DECFSZ", nullptr, "001011dfffffff");
    addInstruction("INCF", nullptr, "001010dfffffff");
    addInstruction("INCFSZ", nullptr, "001111dfffffff");
    addInstruction("IORWF", nullptr, "000100dfffffff");
    addInstruction("MOVF", nullptr, "001000dfffffff");
    addInstruction("MOVWF", nullptr, "0000001fffffff");
    addInstruction("NOP", nullptr, "0000000xx00000");
    addInstruction("RLF", nullptr, "001101dfffffff");
    addInstruction("RRF", nullptr, "001100dfffffff");
    addInstruction("SUBWF", nullptr, "000010dfffffff");
    addInstruction("SWAPF", nullptr, "001110dfffffff");
    addInstruction("XORWF", nullptr, "000110dfffffff");

    // Bit-orientated file register operations
    addInstruction("BCF", nullptr, "0100bbbfffffff");
    addInstruction("BSF", nullptr, "0101bbbfffffff");
    addInstruction("BTFSC", nullptr, "0110bbbfffffff");
    addInstruction("BTFSS", nullptr, "0111bbbfffffff");

    // Literal and control operations
    addInstruction("ADDLW", nullptr, "11111xkkkkkkkk");
    addInstruction("ANDLW", nullptr, "111001kkkkkkkk");
    addInstruction("CALL", nullptr, "100kkkkkkkkkkk");
    addInstruction("CLRWDT", nullptr, "00000001100100");
    addInstruction("GOTO", nullptr, "101kkkkkkkkkkk");
    addInstruction("IORLW", nullptr, "111000kkkkkkkk");
    addInstruction("MOVLW", nullptr, "1100xxkkkkkkkk");
    addInstruction("RETFIE", nullptr, "00000000001001");
    addInstruction("RETLW", nullptr, "1101xxkkkkkkkk");
    addInstruction("RETURN", nullptr, "00000000001000");
    addInstruction("SLEEP", nullptr, "00000000000011");
    addInstruction("SUBLW", nullptr, "11110xkkkkkkkk");
    addInstruction("XORLW", nullptr, "111010kkkkkkkk");
}

PicInfo16C8x::PicInfo16C8x()
    : PicInfo14bit()
{
    delete m_package;
    m_package = new MicroPackage(18);

    m_package->assignPin(17, PicPin::type_bidir, "RA0", "PORTA", 0);
    m_package->assignPin(18, PicPin::type_bidir, "RA1", "PORTA", 1);
    m_package->assignPin(1, PicPin::type_bidir, "RA2", "PORTA", 2);
    m_package->assignPin(2, PicPin::type_bidir, "RA3", "PORTA", 3);
    m_package->assignPin(3, PicPin::type_open, "RA4", "PORTA", 4);

    m_package->assignPin(6, PicPin::type_bidir, "RB0", "PORTB", 0);
    m_package->assignPin(7, PicPin::type_bidir, "RB1", "PORTB", 1);
    m_package->assignPin(8, PicPin::type_bidir, "RB2", "PORTB", 2);
    m_package->assignPin(9, PicPin::type_bidir, "RB3", "PORTB", 3);
    m_package->assignPin(10, PicPin::type_bidir, "RB4", "PORTB", 4);
    m_package->assignPin(11, PicPin::type_bidir, "RB5", "PORTB", 5);
    m_package->assignPin(12, PicPin::type_bidir, "RB6", "PORTB", 6);
    m_package->assignPin(13, PicPin::type_bidir, "RB7", "PORTB", 7);

    m_package->assignPin(4, PicPin::type_mclr, "MCLR");
    m_package->assignPin(5, PicPin::type_vss, "VSS");
    m_package->assignPin(14, PicPin::type_vdd, "VDD");
    m_package->assignPin(15, PicPin::type_osc, "OSC2");
    m_package->assignPin(16, PicPin::type_osc, "OSC1");
}

PicInfo16C8x::~PicInfo16C8x()
{
}

PicInfo16C84::PicInfo16C84()
    : PicInfo16C8x()
{
    m_id = "P16C84"_L1;
}

PicInfo16C84::~PicInfo16C84()
{
}

PicInfo16F84::PicInfo16F84()
    : PicInfo16C8x()
{
    m_id = "P16F84"_L1;
}

PicInfo16F84::~PicInfo16F84()
{
}

PicInfo16CR84::PicInfo16CR84()
    : PicInfo16F84()
{
    m_id = "P16CR84"_L1;
}

PicInfo16CR84::~PicInfo16CR84()
{
}

PicInfo16F83::PicInfo16F83()
    : PicInfo16C8x()
{
    m_id = "P16F83"_L1;
}

PicInfo16F83::~PicInfo16F83()
{
}

PicInfo16CR83::PicInfo16CR83()
    : PicInfo16F83()
{
    m_id = "P16CR83"_L1;
}
PicInfo16CR83::~PicInfo16CR83()
{
}

PicInfo16C61::PicInfo16C61()
    : PicInfo16C8x()
{
    m_id = "P16C61"_L1;
}

PicInfo16C61::~PicInfo16C61()
{
}

PicInfo16X6X::PicInfo16X6X()
    : PicInfo14bit()
{
    m_id = "P16X6X"_L1;
}

PicInfo16X6X::~PicInfo16X6X()
{
}

PicInfo16C62::PicInfo16C62()
    : PicInfo16X6X()
{
    m_id = "P16C62"_L1;

    delete m_package;
    m_package = new MicroPackage(28);

    m_package->assignPin(2, PicPin::type_bidir, "RA0", "PORTA", 0);
    m_package->assignPin(3, PicPin::type_bidir, "RA1", "PORTA", 1);
    m_package->assignPin(4, PicPin::type_bidir, "RA2", "PORTA", 2);
    m_package->assignPin(5, PicPin::type_bidir, "RA3", "PORTA", 3);
    m_package->assignPin(6, PicPin::type_open, "RA4", "PORTA", 4);
    m_package->assignPin(7, PicPin::type_bidir, "RA5", "PORTA", 5);

    m_package->assignPin(21, PicPin::type_bidir, "RB0", "PORTB", 0);
    m_package->assignPin(22, PicPin::type_bidir, "RB1", "PORTB", 1);
    m_package->assignPin(23, PicPin::type_bidir, "RB2", "PORTB", 2);
    m_package->assignPin(24, PicPin::type_bidir, "RB3", "PORTB", 3);
    m_package->assignPin(25, PicPin::type_bidir, "RB4", "PORTB", 4);
    m_package->assignPin(26, PicPin::type_bidir, "RB5", "PORTB", 5);
    m_package->assignPin(27, PicPin::type_bidir, "RB6", "PORTB", 6);
    m_package->assignPin(28, PicPin::type_bidir, "RB7", "PORTB", 7);

    m_package->assignPin(11, PicPin::type_bidir, "RC0", "PORTC", 0);
    m_package->assignPin(12, PicPin::type_bidir, "RC1", "PORTC", 1);
    m_package->assignPin(13, PicPin::type_bidir, "RC2", "PORTC", 2);
    m_package->assignPin(14, PicPin::type_bidir, "RC3", "PORTC", 3);
    m_package->assignPin(15, PicPin::type_bidir, "RC4", "PORTC", 4);
    m_package->assignPin(16, PicPin::type_bidir, "RC5", "PORTC", 5);
    m_package->assignPin(17, PicPin::type_bidir, "RC6", "PORTC", 6);
    m_package->assignPin(18, PicPin::type_bidir, "RC7", "PORTC", 7);

    m_package->assignPin(1, PicPin::type_mclr, "MCLR");
    m_package->assignPin(8, PicPin::type_vss, "VSS");
    m_package->assignPin(9, PicPin::type_osc, "OSC1");
    m_package->assignPin(10, PicPin::type_osc, "OSC2");
    m_package->assignPin(19, PicPin::type_vss, "VSS");
    m_package->assignPin(20, PicPin::type_vdd, "VDD");
}

PicInfo16C62::~PicInfo16C62()
{
}

PicInfo16C63::PicInfo16C63()
    : PicInfo16C62()
{
    m_id = "P16C63"_L1;
}

PicInfo16C63::~PicInfo16C63()
{
}

PicInfo16C64::PicInfo16C64()
    : PicInfo16X6X()
{
    m_id = "P16C64"_L1;

    delete m_package;
    m_package = new MicroPackage(40);

    m_package->assignPin(2, PicPin::type_bidir, "RA0", "PORTA", 0);
    m_package->assignPin(3, PicPin::type_bidir, "RA1", "PORTA", 1);
    m_package->assignPin(4, PicPin::type_bidir, "RA2", "PORTA", 2);
    m_package->assignPin(5, PicPin::type_bidir, "RA3", "PORTA", 3);
    m_package->assignPin(6, PicPin::type_open, "RA4", "PORTA", 4);
    m_package->assignPin(7, PicPin::type_bidir, "RA5", "PORTB", 5);

    m_package->assignPin(33, PicPin::type_bidir, "RB0", "PORTB", 0);
    m_package->assignPin(34, PicPin::type_bidir, "RB1", "PORTB", 1);
    m_package->assignPin(35, PicPin::type_bidir, "RB2", "PORTB", 2);
    m_package->assignPin(36, PicPin::type_bidir, "RB3", "PORTB", 3);
    m_package->assignPin(37, PicPin::type_bidir, "RB4", "PORTB", 4);
    m_package->assignPin(38, PicPin::type_bidir, "RB5", "PORTB", 5);
    m_package->assignPin(39, PicPin::type_bidir, "RB6", "PORTB", 6);
    m_package->assignPin(40, PicPin::type_bidir, "RB7", "PORTB", 7);

    m_package->assignPin(15, PicPin::type_bidir, "RC0", "PORTC", 0);
    m_package->assignPin(16, PicPin::type_bidir, "RC1", "PORTC", 1);
    m_package->assignPin(17, PicPin::type_bidir, "RC2", "PORTC", 2);
    m_package->assignPin(18, PicPin::type_bidir, "RC3", "PORTC", 3);
    m_package->assignPin(23, PicPin::type_bidir, "RC4", "PORTC", 4);
    m_package->assignPin(24, PicPin::type_bidir, "RC5", "PORTC", 5);
    m_package->assignPin(25, PicPin::type_bidir, "RC6", "PORTC", 6);
    m_package->assignPin(26, PicPin::type_bidir, "RC7", "PORTC", 7);

    m_package->assignPin(19, PicPin::type_bidir, "RD0", "PORTD", 0);
    m_package->assignPin(20, PicPin::type_bidir, "RD1", "PORTD", 1);
    m_package->assignPin(21, PicPin::type_bidir, "RD2", "PORTD", 2);
    m_package->assignPin(22, PicPin::type_bidir, "RD3", "PORTD", 3);
    m_package->assignPin(27, PicPin::type_bidir, "RD4", "PORTD", 4);
    m_package->assignPin(28, PicPin::type_bidir, "RD5", "PORTD", 5);
    m_package->assignPin(29, PicPin::type_bidir, "RD6", "PORTD", 6);
    m_package->assignPin(30, PicPin::type_bidir, "RD7", "PORTD", 7);

    m_package->assignPin(8, PicPin::type_bidir, "RE0", "PORTE", 0);
    m_package->assignPin(9, PicPin::type_bidir, "RE1", "PORTE", 1);
    m_package->assignPin(10, PicPin::type_bidir, "RE2", "PORTE", 2);

    m_package->assignPin(1, PicPin::type_mclr, "MCLR");
    m_package->assignPin(11, PicPin::type_vdd, "VDD");
    m_package->assignPin(12, PicPin::type_vss, "VSS");
    m_package->assignPin(13, PicPin::type_osc, "OSC1");
    m_package->assignPin(14, PicPin::type_osc, "OSC2");
    m_package->assignPin(31, PicPin::type_vss, "VSS");
    m_package->assignPin(32, PicPin::type_vdd, "VDD");
}

PicInfo16C64::~PicInfo16C64()
{
}

PicInfo16C65::PicInfo16C65()
    : PicInfo16C64()
{
    m_id = "P16C65"_L1;
}

PicInfo16C65::~PicInfo16C65()
{
}

PicInfo16F62x::PicInfo16F62x()
    : PicInfo16X6X()
{
    m_id = "P16F62x"_L1;

    delete m_package;
    m_package = new MicroPackage(18);

    m_package->assignPin(17, PicPin::type_bidir, "RA0", "PORTA", 0);
    m_package->assignPin(18, PicPin::type_bidir, "RA1", "PORTA", 1);
    m_package->assignPin(1, PicPin::type_bidir, "RA2", "PORTA", 2);
    m_package->assignPin(2, PicPin::type_bidir, "RA3", "PORTA", 3);
    m_package->assignPin(3, PicPin::type_bidir, "RA4", "PORTA", 4);
    m_package->assignPin(4, PicPin::type_input, "RA5", "PORTA", 5);
    m_package->assignPin(15, PicPin::type_bidir, "RA6", "PORTA", 6);
    m_package->assignPin(16, PicPin::type_bidir, "RA7", "PORTA", 7);

    m_package->assignPin(6, PicPin::type_bidir, "RB0", "PORTB", 0);
    m_package->assignPin(7, PicPin::type_bidir, "RB1", "PORTB", 1);
    m_package->assignPin(8, PicPin::type_bidir, "RB2", "PORTB", 2);
    m_package->assignPin(9, PicPin::type_bidir, "RB3", "PORTB", 3);
    m_package->assignPin(10, PicPin::type_bidir, "RB4", "PORTB", 4);
    m_package->assignPin(11, PicPin::type_bidir, "RB5", "PORTB", 5);
    m_package->assignPin(12, PicPin::type_bidir, "RB6", "PORTB", 6);
    m_package->assignPin(13, PicPin::type_bidir, "RB7", "PORTB", 7);

    m_package->assignPin(5, PicPin::type_vss, "VSS");
    m_package->assignPin(14, PicPin::type_vdd, "VDD");
}

PicInfo16F62x::~PicInfo16F62x()
{
    delete m_package;
    m_package = nullptr;
}

PicInfo16F627::PicInfo16F627()
    : PicInfo16F62x()
{
    m_id = "P16F627"_L1;
}

PicInfo16F627::~PicInfo16F627()
{
}

PicInfo16F628::PicInfo16F628()
    : PicInfo16F627()
{
    m_id = "P16F628"_L1;
}

PicInfo16F628::~PicInfo16F628()
{
}

PicInfo16F648::PicInfo16F648()
    : PicInfo16F628()
{
    m_id = "P16F648"_L1;
}

PicInfo16F648::~PicInfo16F648()
{
}

PicInfo16C71::PicInfo16C71()
    : PicInfo16C61()
{
    m_id = "P16C71"_L1;
}

PicInfo16C71::~PicInfo16C71()
{
}

PicInfo16C712::PicInfo16C712()
    : PicInfo16C62()
{
    m_id = "P16C712"_L1;
}

PicInfo16C712::~PicInfo16C712()
{
}

PicInfo16C716::PicInfo16C716()
    : PicInfo16C712()
{
    m_id = "P16C716"_L1;
}

PicInfo16C716::~PicInfo16C716()
{
}

PicInfo16C72::PicInfo16C72()
    : PicInfo16C62()
{
    m_id = "P16C72"_L1;
}

PicInfo16C72::~PicInfo16C72()
{
}

PicInfo16C73::PicInfo16C73()
    : PicInfo16C63()
{
    m_id = "P16C73"_L1;
}

PicInfo16C73::~PicInfo16C73()
{
}

PicInfo16C74::PicInfo16C74()
    : PicInfo16C65()
{
    m_id = "P16C74"_L1;
}

PicInfo16C74::~PicInfo16C74()
{
}

PicInfo16F873::PicInfo16F873()
    : PicInfo16C73()
{
    m_id = "P16F873"_L1;
}

PicInfo16F873::~PicInfo16F873()
{
}

PicInfo16F874::PicInfo16F874()
    : PicInfo16C74()
{
    m_id = "P16F874"_L1;
}

PicInfo16F874::~PicInfo16F874()
{
}

PicInfo16F877::PicInfo16F877()
    : PicInfo16F874()
{
    m_id = "P16F877"_L1;
}

PicInfo16F877::~PicInfo16F877()
{
}
