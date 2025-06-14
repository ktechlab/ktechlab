/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke                              *
 *   daniel.jc@gmail.com						   *
 *									   *
 *   24-04-2007                                                            *
 *   Modified to add pic 16f877,16f627 and 16f628 			   *
 *   by george john george@space-kerala.org 				   *
 *   supported by SPACE www.space-kerala.org	 			   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "instruction.h"
#include "parser.h"
#include "pic14.h"

#include <QDebug>
#include <QLatin1StringView>

#include <cassert>
#include <iostream>

using Qt::Literals::operator""_L1;

using namespace std;


QString pic_type;
bool LEDSegTable[][7] = {
{ 1, 1, 1, 1, 1, 1, 0 },
{ 0, 1, 1, 0, 0, 0, 0 }, // 1
{ 1, 1, 0, 1, 1, 0, 1 }, // 2
{ 1, 1, 1, 1, 0, 0, 1 }, // 3
{ 0, 1, 1, 0 ,0, 1, 1 }, // 4
{ 1, 0, 1, 1, 0, 1, 1 }, // 5
{ 1, 0, 1, 1, 1, 1, 1 }, // 6
{ 1, 1, 1, 0, 0, 0, 0 }, // 7
{ 1, 1, 1, 1, 1, 1, 1 }, // 8
{ 1, 1, 1, 0, 0, 1, 1 }, // 9
{ 1, 1, 1, 0, 1, 1, 1 }, // A
{ 0, 0, 1, 1, 1, 1, 1 }, // b
{ 1, 0, 0, 1, 1, 1, 0 }, // C
{ 0, 1, 1, 1, 1, 0, 1 }, // d
{ 1, 0, 0, 1, 1, 1, 1 }, // E
{ 1, 0, 0, 0, 1, 1, 1 }  // F
};



PIC14::PIC14( MicrobeApp * master, Type type )
{
	mb = master;
	m_pCode = nullptr;
	m_type = type;

}


PIC14::~PIC14()
{
}

PortPin PIC14::toPortPin( const QString & portPinString )
{
	QString port,holdport;
	int pin = -1;
/*****************************modified *********************************************/
//inorder to support RB.3=high/1

	if ( portPinString.length()  == 3 )
	{
		port = QLatin1StringView("PORT%1"_L1).arg( portPinString[1].toUpper() );
		pin = QString( portPinString[2] ).toInt();
	}
	// In form e.g. RB.3
	else if ( portPinString.length()  == 4 )//modification change ==3 to ==4
	{
		port = QLatin1StringView("PORT%1"_L1).arg( portPinString[1].toUpper() );
		pin = QString( portPinString[3] ).toInt();//modification change 2 to 3
	}
	else
	{
		int dotpos = portPinString.indexOf(QLatin1StringView("."_L1));
		if ( dotpos == -1 )
			return PortPin();

		port = portPinString.left(dotpos);
//modified checking is added in the case of Register also
//now INTCON.GIE is high ,and INTCON.GIE=1/high works
		if(isValidRegister( port))
		{

			RegisterBit REG(portPinString.mid(dotpos+1));
			pin=REG.bitPos();
			Register Reg(REG.registerType());
			holdport=Reg.name();
			if(holdport!=port)
		    	 cerr << QLatin1StringView(" ERROR: %1 is not a Register bit\n"_L1).arg(portPinString ).toStdString();
		}
		else
			pin = portPinString.mid(dotpos+1).toInt();


//***************************Modification ends********************************
	}

	PortPin portPin( port, pin );

	if ( isValidPortPin( portPin ) )
		return portPin;
//**************************Modification start ********************************
	else if(isValidRegister(port))
		return portPin;
//**************************Modification ends ********************************
	else
	{
		cerr << QLatin1StringView("ERROR: %1 is not a Port/Register bit\n"_L1).arg(portPinString ).toStdString();
		return PortPin();
	}
}


void PIC14::mergeCode( Code * code )
{
	m_pCode->merge( code );
}


uchar PIC14::gprStart() const
{
	switch ( m_type )
	{
		case P16C84:
		case P16F84:
			return 0xc;

		case P16F627:
		case P16F628:
		case P16F877:
			return 0x20;

		case unknown:
			break;
	}

	qCritical() << Q_FUNC_INFO << "Unknown PIC type = " << m_type;
	return 0xc;
}


PIC14::Type PIC14::toType( const QString & _text )
{
	QString text = _text.toUpper().simplified().remove(QLatin1Char('P'));

	if ( text == QLatin1StringView("16C84"_L1) )
	{
		pic_type = QLatin1StringView("P16C84"_L1);
		return P16C84;
	}
	if ( text == QLatin1StringView("16F84"_L1) )
	{
		pic_type = QLatin1StringView("P16F84"_L1);
		return P16F84;
	}
	if ( text == QLatin1StringView("16F627"_L1) )
	{
		pic_type = QLatin1StringView("P16F627"_L1);
		return P16F627;
	}

	if ( text == QLatin1StringView("16F628"_L1) )
	{
		pic_type = QLatin1StringView("P16F628"_L1);
		return P16F628;
	}
//modified checking of 16F877 is included
	if ( text == QLatin1StringView("16F877"_L1) )
	{
		pic_type = QLatin1StringView("P16F877"_L1);
		return P16F877;
	}

	cerr << QLatin1StringView("%1 is not a known PIC identifier\n"_L1).arg(_text).toStdString();
	return unknown;
}


QString PIC14::minimalTypeString() const
{
	switch ( m_type )
	{
		case P16C84:
			return QLatin1StringView("16C84"_L1);

		case P16F84:
			return QLatin1StringView("16F84"_L1);

		case P16F627:
			return QLatin1StringView("16F627"_L1);

		case P16F628:
			return QLatin1StringView("16F628"_L1);

//modified checking of 16F877 is included

		case P16F877:
			return QLatin1StringView("16F877"_L1);

		case unknown:
			break;
	}

	qCritical() << Q_FUNC_INFO << "Unknown PIC type = " << m_type;
	return QLatin1StringView();
}


void PIC14::postCompileConstruct( const QStringList &interrupts )
{
	m_pCode->append( new Instr_raw(QLatin1StringView("\n\tEND\n"_L1)), Code::Subroutine );

	if ( interrupts.isEmpty() )
	{
		// If there are no ISRs then we don't need to put in any handler code.
		// Instead, just insert the goto start instruction in case we need to
		// jump past any lookup tables (and if there are none, then the optimizer
		// will remove the goto instruction).
		m_pCode->append(new Instr_goto(QLatin1StringView("_start"_L1)), Code::InterruptHandler);
		m_pCode->queueLabel( QLatin1StringView("_start"_L1), Code::LookupTable );
		return;
	}

	/*
		INTCON register:
		7 --- GIE EEIE T0IE INTE RBIE T0IF INTF RBIF --- 0

		E: enable
		F: flag
		Flag bits must be cleared manually before reactivating GIE,
		but we do this in each individual interrupt handler
	*/

	// The bizarre dance with swap is to ensure the status bits
	// are preserved properly
	m_pCode->append(new Instr_goto(QLatin1StringView("_start"_L1)), Code::InterruptHandler);

	m_pCode->append(new Instr_raw(QLatin1StringView("ORG 0x4"_L1)), Code::InterruptHandler);
	// When we arrive here:
	// Return address on stack,
	// GIE flag cleared (globally interrupts disabled)
	// W or STATUS not preserved by processor.
	m_pCode->append(new Instr_movwf("W_TEMP"), Code::InterruptHandler);
	m_pCode->append(new Instr_swapf("STATUS",0), Code::InterruptHandler);
	m_pCode->append(new Instr_movwf("STATUS_TEMP"), Code::InterruptHandler);

	QStringList::ConstIterator interruptsEnd = interrupts.end();
	for( QStringList::ConstIterator it = interrupts.begin(); it != interruptsEnd; ++it )
	{
		// Is the interrupt's flag bit set?
		m_pCode->append(new Instr_btfsc("INTCON",QString::number(interruptNameToBit((*it), true))), Code::InterruptHandler);
		m_pCode->append(new Instr_goto(QLatin1StringView("_interrupt_") + (*it)), Code::InterruptHandler); // Yes, do its handler routine
		// Otherwise fall through to the next.
	}

	// If there was "somehow" a spurious interrupt there isn't really
	// much we can do about that (??) so just fall through and hope for the worst.

	m_pCode->queueLabel( QLatin1StringView("_interrupt_end"_L1), Code::InterruptHandler );
	m_pCode->append(new Instr_swapf("STATUS_TEMP",0), Code::InterruptHandler );
	m_pCode->append(new Instr_movwf("STATUS"), Code::InterruptHandler );
	m_pCode->append(new Instr_swapf("W_TEMP",1), Code::InterruptHandler );
	m_pCode->append(new Instr_swapf("W_TEMP",0), Code::InterruptHandler );
	m_pCode->append(new Instr_retfie()); // Returns and re-enables globally interrupts.

	m_pCode->queueLabel( QLatin1StringView("_start"_L1), Code::LookupTable );
}

int PIC14::interruptNameToBit(const QString &name, bool flag)
{
	// 7 --- GIE EEIE T0IE INTE RBIE T0IF INTF RBIF --- 0

	if( name == QLatin1StringView("change"_L1) ) // RB
	{
		if(flag) return 0;
		else return 3;
	}
	else if( name == QLatin1StringView("timer"_L1) )
	{
		if(flag) return 2;
		else return 5;
	}
	else if( name == QLatin1StringView("external"_L1) )
	{
		if(flag) return 1;
		else return 4;
	}

	return -1;
}


bool PIC14::isValidPort( const QString & portName ) const
{

	if(pic_type == QLatin1StringView("P16F84"_L1) || pic_type == QLatin1StringView("P16C84"_L1) ||
		pic_type == QLatin1StringView("P16F627"_L1) || pic_type == QLatin1StringView("P16F628"_L1))
		return ( portName == QLatin1StringView("PORTA"_L1) || portName == QLatin1StringView("PORTB"_L1));

	if(pic_type == QLatin1StringView("P16F877"_L1))
		return ( portName == QLatin1StringView("PORTA"_L1) || portName == QLatin1StringView("PORTB"_L1) ||
			portName == QLatin1StringView("PORTC"_L1) || portName == QLatin1StringView("PORTD"_L1) || portName == QLatin1StringView("PORTE"_L1));

	return false;
}


bool PIC14::isValidPortPin( const PortPin & portPin ) const
{

	if(pic_type == "P16F84"_L1 ||pic_type =="P16C84"_L1)
	{
		if ( portPin.port() == "PORTA"_L1 )
			return (portPin.pin() >= 0) && (portPin.pin() <= 4);

		if ( portPin.port() == "PORTB"_L1 )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);
	}
	if(pic_type == "P16F627"_L1 ||pic_type =="P16F628"_L1)
	{
		if ( portPin.port() == "PORTA"_L1 )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);

		if ( portPin.port() == "PORTB"_L1 )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);
	}

	if(pic_type=="P16F877"_L1)
	{
		if ( portPin.port() == "PORTA"_L1 )
			return (portPin.pin() >= 0) && (portPin.pin() <= 5);

		if ( portPin.port() == "PORTB"_L1 )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);
		if ( portPin.port() == "PORTC"_L1 )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);

		if ( portPin.port() == "PORTD"_L1 )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);

		if ( portPin.port() == "PORTE"_L1 )
			return (portPin.pin() >= 0) && (portPin.pin() <= 2);
	}

	return false;
}


bool PIC14::isValidTris( const QString & trisName ) const
{
	if(pic_type =="P16F84"_L1 ||pic_type =="P16C84"_L1 ||pic_type =="P16F627"_L1 ||pic_type =="P16F628"_L1)
		return ( trisName == "TRISA"_L1 || trisName == "TRISB"_L1);

	if(pic_type=="P16F877"_L1)
		return ( trisName =="TRISA"_L1 || trisName =="TRISB"_L1 ||trisName =="TRISC"_L1 ||trisName == "TRISD"_L1 ||trisName == "TRISE"_L1 );

	return false;
}

//*****************Modified ****************************//
//New function isValiedRegister is added to check whether a register is valid or not
bool PIC14::isValidRegister( const QString & registerName)const
{
 	if(pic_type=="P16F84"_L1 ||pic_type=="P16C84"_L1)
		return ( registerName == "TMR0"_L1
			|| registerName == "PCL"_L1
			|| registerName == "STATUS"_L1
			|| registerName == "FSR"_L1
			|| registerName == "EEDATH"_L1
			|| registerName == "EEADR"_L1
			|| registerName == "PCLATH"_L1
			|| registerName == "INTCON"_L1
			|| registerName == "EECON1"_L1
			|| registerName == "EECON2"_L1
			|| registerName == "OPTION_REG"_L1);

	if(pic_type=="P16F877"_L1)
		return ( registerName == "TMR0"_L1
			|| registerName == "PCL"_L1
			|| registerName == "STATUS"_L1
			|| registerName == "FSR"_L1
			|| registerName == "PCLATH"_L1
			|| registerName == "INTCON"_L1
			|| registerName == "PIR1"_L1
			|| registerName == "PIR2"_L1
			|| registerName == "TMR1L"_L1
			|| registerName == "TMR1H"_L1
			|| registerName == "T1CON"_L1
			|| registerName == "TMR2"_L1
			|| registerName == "T2CON"_L1
			|| registerName == "SSPBUF"_L1
			|| registerName == "SSPCON"_L1
			|| registerName == "CCPR1L"_L1
			|| registerName == "CCPR1H"_L1
			|| registerName == "CCP1CON"_L1
			|| registerName == "RCSTA"_L1
			|| registerName == "TXREG"_L1
			|| registerName == "RCREG"_L1
			|| registerName == "CCPR2L"_L1
			|| registerName == "CCPR2H"_L1
			|| registerName == "CCP2CON"_L1
			|| registerName == "ADRESH"_L1
			|| registerName == "ADCON0"_L1 /*bank0ends*/
			|| registerName == "OPTION_REG"_L1
			|| registerName == "PIE1"_L1
			|| registerName == "PIE2"_L1
			|| registerName == "PCON"_L1
			|| registerName == "SSPCON2"_L1
			|| registerName == "PR2"_L1
			|| registerName == "SSPADD"_L1
			|| registerName == "SSPSTAT"_L1
			|| registerName == "TXSTA"_L1
			|| registerName == "SPBRG"_L1
			|| registerName == "ADRESL"_L1
			|| registerName == "ADCON1"_L1 /*bank1ends*/
			|| registerName == "EEDATA"_L1
			|| registerName == "EEADR"_L1
			|| registerName == "EEDATH"_L1
			|| registerName == "EEADRH"_L1 /*bank2ends*/
			|| registerName == "EECON1"_L1
			|| registerName == "EECON2"_L1 /*bank3ends*/   );

	if(pic_type=="P16F627"_L1 ||pic_type=="P16F628"_L1)
		return ( registerName == "TMR0"_L1
			|| registerName == "PCL"_L1
			|| registerName == "STATUS"_L1
			|| registerName == "FSR"_L1
			|| registerName == "PCLATH"_L1
			|| registerName == "INTCON"_L1
			|| registerName == "PIR1"_L1
			|| registerName == "TMR1L"_L1
			|| registerName == "TMR1H"_L1
			|| registerName == "T1CON"_L1
			|| registerName == "TMR2"_L1
			|| registerName == "T2CON"_L1
			|| registerName == "CCPR1L"_L1
			|| registerName == "CCPR1H"_L1
			|| registerName == "CCP1CON"_L1
			|| registerName == "RCSTA"_L1
			|| registerName == "TXREG"_L1
			|| registerName == "RCREG"_L1
			|| registerName == "CMCON"_L1 /*bank0ends*/
			|| registerName == "OPTION_REG"_L1
			|| registerName == "PIE1"_L1
			|| registerName == "PCON"_L1
			|| registerName == "PR2"_L1
			|| registerName == "TXSTA"_L1
			|| registerName == "SPBRG"_L1
			|| registerName == "EEDATA"_L1
			|| registerName == "EEADR"_L1
			|| registerName == "EECON1"_L1
			|| registerName == "EECON2"_L1
			|| registerName == "VRCON"_L1 /*bank1ends*/ );

	return false;
}

//****************************modifications ends********************************************

bool PIC14::isValidInterrupt( const QString & interruptName ) const
{
	if(pic_type == "P16F84"_L1 ||pic_type =="P16C84"_L1 ||pic_type =="P16F877"_L1 ||pic_type=="P16F627"_L1 ||pic_type=="P16F628"_L1)
		return ( interruptName == "change"_L1 ||
				 interruptName == "timer"_L1 ||
				 interruptName == "external"_L1 );

	return false;
}


void PIC14::setConditionalCode( Code * ifCode, Code * elseCode )
{
	m_ifCode = ifCode;
	m_elseCode = elseCode;
}

void PIC14::Sgoto(const QString &label)
{
	m_pCode->append( new Instr_goto(label) );
}

void PIC14::Slabel(const QString &label)
{
// 	std::cout << Q_FUNC_INFO << "label="<<label<<'\n';
	m_pCode->queueLabel( label, Code::Middle );
}

void PIC14::Send()
{
	m_pCode->append( new Instr_sleep() );
}

void PIC14::Ssubroutine( const QString &procName, Code * subCode )
{
	m_pCode->queueLabel( procName, Code::Subroutine );
	m_pCode->merge( subCode, Code::Subroutine );
	m_pCode->append( new Instr_return(), Code::Subroutine );
}

void PIC14::Sinterrupt( const QString &procName, Code * subCode )
{
	m_pCode->queueLabel( "_interrupt_"_L1 + procName, Code::Subroutine );

	// Clear the interrupt flag for this particular interrupt source
	m_pCode->append( new Instr_bcf("INTCON",QString::number(interruptNameToBit(procName,true))) );
	m_pCode->merge( subCode, Code::Subroutine );

	m_pCode->append( new Instr_goto("_interrupt_end"_L1), Code::Subroutine );
}


void PIC14::Scall(const QString &name)
{
	m_pCode->append( new Instr_call(name) );
}


void PIC14::Ssetlh( const PortPin & portPin, bool high)
{
	if(high)
		m_pCode->append( new Instr_bsf( portPin.port(),QString::number(portPin.pin()) ) );
	else
		m_pCode->append( new Instr_bcf( portPin.port(), QString::number(portPin.pin()) ) );
}

void PIC14::rearrangeOpArguments( QString * val1, QString * val2, LocationType * val1Type, LocationType * val2Type)
{
	if( *val2Type == work && *val1Type != work )
	{
		LocationType tempType = *val2Type;
		QString tempVal = *val2;

		*val2Type = *val1Type;
		*val2 = *val1;

		*val1Type = tempType;
		*val1 = tempVal;
	}
}

void PIC14::add( QString val1, QString val2, LocationType val1Type, LocationType val2Type )
{
	rearrangeOpArguments( &val1, &val2, &val1Type, &val2Type );

	switch(val1Type)
	{
		case num:  m_pCode->append(new Instr_movlw( val1.toInt( nullptr, 0 ) )); break;
		case work: break;
		case var: m_pCode->append(new Instr_movf(val1,0)); break;
	}

	switch(val2Type)
	{
		case num: m_pCode->append(new Instr_addlw(val2.toInt( nullptr, 0 ))); break;
		case work: break;
		case var: m_pCode->append(new Instr_addwf(val2,0)); break;
	}
}

void PIC14::subtract( const QString & val1, const QString & val2, LocationType val1Type, LocationType val2Type )
{
	switch(val2Type)
	{
		case num:  m_pCode->append(new Instr_movlw( val2.toInt( nullptr, 0 ) )); break;
		case work: break;
		case var: m_pCode->append(new Instr_movf(val2,0)); break;
	}
	switch(val1Type)
	{
		case num: m_pCode->append(new Instr_sublw(val1.toInt( nullptr, 0 ))); break;
		case work: break;
		case var: m_pCode->append(new Instr_subwf(val1,0)); break;
	}
}

void PIC14::assignNum(const QString & val)
{
	m_pCode->append(new Instr_movlw(val.toInt( nullptr, 0 )));
}

void PIC14::assignVar(const QString &val)
{
	m_pCode->append(new Instr_movf(val,0));
}

void PIC14::saveToReg(const QString &dest)
{
	m_pCode->append(new Instr_movwf(dest));
}

void PIC14::saveResultToVar( const QString & var )
{
	m_pCode->append( new Instr_movwf( var ) );
}

void PIC14::mul(QString val1, QString val2, LocationType val1Type, LocationType val2Type)
{
	multiply();

	rearrangeOpArguments( &val1, &val2, &val1Type, &val2Type );

	// First, set _i argument
	switch(val1Type)
	{
		case num: m_pCode->append(new Instr_movlw(val1.toInt( nullptr, 0 ))); break;
		case work: break;
		case var: m_pCode->append(new Instr_movf(val1,0)); break;
	}

	m_pCode->append(new Instr_movwf("__i"));

	// Then set _j argument
	switch(val2Type)
	{
		case num: m_pCode->append(new Instr_movlw(val2.toInt( nullptr, 0 ))); break;
		case work: break;
		case var: m_pCode->append(new Instr_movf(val2,0)); break;
	}

	m_pCode->append(new Instr_movwf("__j"));
	m_pCode->append(new Instr_call("__picfunc_multiply"_L1));
	m_pCode->append(new Instr_movf("__result",0));
}


void PIC14::multiply()
{
	if ( m_pCode->instruction("__picfunc_multiply"_L1) )
		return;

	m_pCode->queueLabel( "__picfunc_multiply"_L1, Code::Subroutine );
	m_pCode->append(new Instr_clrf("__result"), Code::Subroutine ); //result+=m_pCode->appenduction("clrf __result"_L1);

	m_pCode->queueLabel( "__picfunc_multiply_loop"_L1, Code::Subroutine );
	m_pCode->append(new Instr_movf("__i",0), Code::Subroutine ); //result+=m_pCode->appenduction("movf __i,0"_L1);
	m_pCode->append(new Instr_btfsc("__j","0"), Code::Subroutine ); //result+=m_pCode->appenduction("btfsc __j,0"_L1);
	m_pCode->append(new Instr_addwf("__result",1), Code::Subroutine ); //result+=m_pCode->appenduction("addwf __result,1"_L1);
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine ); //result+=m_pCode->appenduction("bcf STATUS,C"_L1);
	m_pCode->append(new Instr_rrf("__j",1), Code::Subroutine ); //result+=m_pCode->appenduction("rrf __j,1"_L1);
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine ); //result+=m_pCode->appenduction("bcf STATUS,C"_L1);
	m_pCode->append(new Instr_rlf("__i",1), Code::Subroutine ); //result+=m_pCode->appenduction("rlf __i,1"_L1);
	m_pCode->append(new Instr_movf("__j",1), Code::Subroutine ); //result+=m_pCode->appenduction("movf __j,1"_L1);
	m_pCode->append(new Instr_btfss("STATUS","Z"), Code::Subroutine ); //result+=m_pCode->appenduction("btfss STATUS,Z"_L1);
	m_pCode->append(new Instr_goto("__picfunc_multiply_loop"_L1), Code::Subroutine ); //result+=m_pCode->appenduction("goto __picfunc_multiply_loop"_L1);
	m_pCode->append(new Instr_return(), Code::Subroutine ); //result+=m_pCode->appenduction("return"_L1);
}


void PIC14::div( const QString & val1, const QString & val2, LocationType val1Type, LocationType val2Type)
{
	divide();

	// NOO - "x / 2" is NOT the same as "2 / x"
// 	rearrangeOpArguments( val1, val2, val1Type, val2Type );

	// First, set _i argument
	switch(val1Type)
	{
		case num: m_pCode->append(new Instr_movlw(val1.toInt( nullptr, 0 ))); break;
		case work: break;
		case var: m_pCode->append(new Instr_movf(val1,0)); break;
	}

	m_pCode->append(new Instr_movwf("__i"));

	// Then set _j argument
	switch(val2Type)
	{
		case num: m_pCode->append(new Instr_movlw(val2.toInt( nullptr, 0 ))); break;
		case work: break;
		case var: m_pCode->append(new Instr_movf(val2,0)); break;
	}

	m_pCode->append(new Instr_movwf("__j"));

	m_pCode->append(new Instr_call("__picfunc_divide"_L1));//result+=instruction("call __picfunc_divide"_L1);
	m_pCode->append(new Instr_movf("__result",0));//result+=instruction("movf __result,0"_L1);
}

void PIC14::divide()
{
	m_pCode->queueLabel( "__picfunc_divide"_L1, Code::Subroutine );
	m_pCode->append(new Instr_movf("__j",1), Code::Subroutine );
	m_pCode->append(new Instr_btfsc("STATUS","2"), Code::Subroutine );
	m_pCode->append(new Instr_return(), Code::Subroutine );
	m_pCode->append(new Instr_clrf("__result"), Code::Subroutine );
	m_pCode->append(new Instr_movlw(1), Code::Subroutine );
	m_pCode->append(new Instr_movwf("__k"), Code::Subroutine );

	m_pCode->queueLabel( "__divide_shift"_L1, Code::Subroutine );
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_rlf("__k",1), Code::Subroutine );
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_rlf("__j",1), Code::Subroutine );
	m_pCode->append(new Instr_btfss("__j","7"), Code::Subroutine );
	m_pCode->append(new Instr_goto("__divide_shift"_L1), Code::Subroutine );

	m_pCode->queueLabel( "__divide_loop"_L1, Code::Subroutine );
	m_pCode->append(new Instr_movf("__j",0), Code::Subroutine );
	m_pCode->append(new Instr_subwf("__i",1), Code::Subroutine );
	m_pCode->append(new Instr_btfsc("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_goto("__divide_count"_L1), Code::Subroutine );
	m_pCode->append(new Instr_addwf("__i",1), Code::Subroutine );
	m_pCode->append(new Instr_goto("__divide_final"_L1), Code::Subroutine );

	m_pCode->queueLabel( "__divide_count"_L1, Code::Subroutine );
	m_pCode->append(new Instr_movf("__k",0), Code::Subroutine );
	m_pCode->append(new Instr_addwf("__result",1), Code::Subroutine );

	m_pCode->queueLabel( "__divide_final"_L1, Code::Subroutine );
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_rrf("__j",1), Code::Subroutine );
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_rrf("__k",1), Code::Subroutine );
	m_pCode->append(new Instr_btfss("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_goto("__divide_loop"_L1), Code::Subroutine );
	m_pCode->append(new Instr_return(), Code::Subroutine );
}


Code * PIC14::ifCode()
{
	return m_ifCode;
}


Code * PIC14::elseCode()
{
	return m_elseCode;
}


void PIC14::ifInitCode( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type )
{
	// NOO - "x < 2" is NOT the same as "2 < x"
// 	rearrangeOpArguments( val1, val2, val1Type, val2Type );

	switch(val1Type)
	{
		case num:
			m_pCode->append(new Instr_movlw(val1.toInt( nullptr, 0 )));
			break;

		case work:
			break; // Nothing to do

		case var:
			m_pCode->append(new Instr_movf(val1,0));
			break;
	}

	switch(val2Type)
	{
		case num:
			m_pCode->append(new Instr_sublw(val2.toInt( nullptr, 0 )));
			break;

		case work:
			qCritical() << Q_FUNC_INFO << "Cannot subtract working from working!";
			break;

		case var:
			m_pCode->append(new Instr_subwf(val2,0));
			break;
	}
}

void PIC14::equal( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type )
{
	ifInitCode( val1, val2, val1Type, val2Type );
	const QString labelEnd = mb->uniqueLabel()+"_endif"_L1;
	const QString labelFalse = mb->uniqueLabel()+"_case_false"_L1;

	m_pCode->append(new Instr_btfss("STATUS","2"));
	m_pCode->append(new Instr_goto(labelFalse));

	mergeCode( ifCode() );

	m_pCode->append(new Instr_goto(labelEnd));

	m_pCode->queueLabel( labelFalse );
	mergeCode( elseCode() );
	m_pCode->queueLabel( labelEnd );
}

void PIC14::notEqual( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type )
{
	ifInitCode( val1, val2, val1Type, val2Type );
	const QString labelEnd = mb->uniqueLabel()+"_endif"_L1;
	const QString labelFalse = mb->uniqueLabel()+"_case_false"_L1;

	m_pCode->append(new Instr_btfsc("STATUS","2"));
	m_pCode->append(new Instr_goto(labelFalse));

	mergeCode( ifCode() );

	m_pCode->append(new Instr_goto(labelEnd));

	m_pCode->queueLabel( labelFalse );
	mergeCode( elseCode() );
	m_pCode->queueLabel( labelEnd );
}

void PIC14::greaterThan( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type )
{
	ifInitCode( val1, val2, val1Type, val2Type );
	const QString labelEnd = mb->uniqueLabel()+"_endif"_L1;
	const QString labelFalse = mb->uniqueLabel()+"_case_false"_L1;

	m_pCode->append(new Instr_btfsc("STATUS","0"));
	m_pCode->append(new Instr_goto(labelFalse));

	mergeCode( ifCode() );
	m_pCode->append(new Instr_goto(labelEnd));

	m_pCode->queueLabel( labelFalse );
	mergeCode( elseCode() );
	m_pCode->queueLabel( labelEnd );
}

void PIC14::lessThan( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type )
{
	cout << Q_FUNC_INFO << endl;
	ifInitCode( val1, val2, val1Type, val2Type );
	const QString labelEnd = mb->uniqueLabel()+"_endif"_L1;
	const QString labelFalse = mb->uniqueLabel()+"_case_false"_L1;

	m_pCode->append(new Instr_btfss("STATUS","0"));
	m_pCode->append(new Instr_goto(labelFalse));
	m_pCode->append(new Instr_btfsc("STATUS","2"));
	m_pCode->append(new Instr_goto(labelFalse));

	mergeCode( ifCode() );

	m_pCode->append(new Instr_goto(labelEnd));

	m_pCode->queueLabel( labelFalse );
	mergeCode( elseCode() );
	m_pCode->queueLabel( labelEnd );
}

void PIC14::greaterOrEqual( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type )
{
	ifInitCode( val1, val2, val1Type, val2Type );
	const QString labelEnd = mb->uniqueLabel()+"_endif"_L1;
	const QString labelTrue = mb->uniqueLabel()+"_case_true"_L1; // Note that unlike the others, this is labelTrue, not labelFalse

	m_pCode->append(new Instr_btfsc("STATUS","2"));
	m_pCode->append(new Instr_goto(labelTrue));
	m_pCode->append(new Instr_btfss("STATUS","0"));
	m_pCode->append(new Instr_goto(labelTrue));

	mergeCode( elseCode() );

	m_pCode->append(new Instr_goto(labelEnd));

	m_pCode->queueLabel( labelTrue );
	mergeCode( ifCode() );
	m_pCode->queueLabel( labelEnd );
}

void PIC14::lessOrEqual( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type )
{
	ifInitCode( val1, val2, val1Type, val2Type );
	const QString labelEnd = mb->uniqueLabel()+"_endif"_L1;
	const QString labelFalse = mb->uniqueLabel()+"_case_false"_L1;

	m_pCode->append(new Instr_btfss("STATUS","0"));
	m_pCode->append(new Instr_goto(labelFalse));

	mergeCode( ifCode() );
	m_pCode->append(new Instr_goto(labelEnd));

	m_pCode->queueLabel( labelFalse );
	mergeCode( elseCode() );
	m_pCode->queueLabel( labelEnd );
}


void PIC14::Swhile( Code * whileCode, const QString &expression)
{
	QString result;
	QString ul = mb->uniqueLabel();

	whileCode->append( new Instr_goto(ul) );

	m_pCode->queueLabel( ul, Code::Middle );

	// If the condition is not true, just fall through
	m_parser->compileConditionalExpression( expression, whileCode, nullptr );
}


void PIC14::Srepeat( Code * repeatCode, const QString &expression)
{
	QString result;
	QString ul = mb->uniqueLabel();

	Code * elseCode = new Code;
	elseCode->append( new Instr_goto(ul) );

	m_pCode->queueLabel( ul );
	m_pCode->merge( repeatCode );

	// If the condition is true, just fall through
	m_parser->compileConditionalExpression( expression, nullptr, elseCode );
}

void PIC14::Sif( Code * ifCode, Code * elseCode, const QString &expression)
{
	m_parser->compileConditionalExpression( expression, ifCode, elseCode );
}


void PIC14::Sfor( Code * forCode, Code * initCode, const QString &expression, const QString &variable, const QString &step, bool stepPositive)
{
	QString ul = mb->uniqueLabel();

	if ( step == "1"_L1 )
	{
		if (stepPositive)
			forCode->append(new Instr_incf(variable,1));
		else
			forCode->append(new Instr_decf(variable,1));
	}
	else
	{
		forCode->append(new Instr_movlw(step.toInt( nullptr, 0 )));
		if (stepPositive)
			forCode->append(new Instr_addwf(variable,1));
		else
			forCode->append(new Instr_subwf(variable,1));
	}
	forCode->append(new Instr_goto(ul));

	m_pCode->merge( initCode );

	m_pCode->queueLabel( ul );

	m_parser->compileConditionalExpression( expression, forCode, nullptr );
}


void PIC14::Spin( const PortPin & portPin, bool NOT)
{
	QString lowLabel, highLabel, postLabel;
	lowLabel = mb->uniqueLabel();
	highLabel = mb->uniqueLabel();
	postLabel = mb->uniqueLabel();
	/*result += indent + "goto\t" + lowLabel;
	result += indent + "movlw\t1" + "goto\t"+postLabel+;
	result += lowLabel +  + indent + "movlw\t0" + indent;
	result += postLabel + ;*/

	if(NOT)
		m_pCode->append(new Instr_btfsc( portPin.port(), QString::number( portPin.pin() ) ));
	//result +=instruction((QString)(NOT?"btfsc":"btfss"_L1)+"\t"+port+","+pin);
	else
		m_pCode->append(new Instr_btfss( portPin.port(), QString::number( portPin.pin() ) ));

	m_pCode->append(new Instr_goto(lowLabel));//result += instruction("goto\t" + lowLabel);
	mergeCode( ifCode() );
	m_pCode->append(new Instr_goto(postLabel));//result += instruction("goto\t"+postLabel);

	m_pCode->queueLabel( lowLabel );
	mergeCode( elseCode() );

	m_pCode->queueLabel( postLabel );
}


void PIC14::Sdelay( unsigned length_us, Code::InstructionPosition pos )
{
	if ( length_us == 0 )
		return;

	if ( length_us > 50070524 )
	{
		length_us += 50267642;
		int l = length_us/50070530;
		length_us -= l * 50070530;
		int k = length_us/196355;

		m_pCode->append( new Instr_movlw( l ), pos );
		m_pCode->append( new Instr_movwf( "__l" ), pos );
		m_pCode->append( new Instr_movlw( k ), pos );
		m_pCode->append( new Instr_movwf( "__k" ), pos );

		mb->addDelayRoutineWanted( Delay_50S );
	}

	else if ( length_us > 196350 )
	{
		length_us += 197116;
		int k = length_us/196355;
		length_us -= k * 196355;
		int j = length_us/770;

		m_pCode->append( new Instr_incf( "__l", 1 ), pos );
		m_pCode->append( new Instr_movlw( k ), pos );
		m_pCode->append( new Instr_movwf( "__k" ), pos );
		m_pCode->append( new Instr_movlw( j ), pos );
		m_pCode->append( new Instr_movwf( "__j" ), pos );

		mb->addDelayRoutineWanted( Delay_200mS );
	}

	else if ( length_us > 766 )
	{
		length_us += 765;
		int j = length_us/770;
		length_us -= j * 770;
		int i = length_us/3;

		m_pCode->append( new Instr_incf( "__l", 1 ), pos );
		m_pCode->append( new Instr_incf( "__k", 1 ), pos );
		m_pCode->append( new Instr_movlw( j ), pos );
		m_pCode->append( new Instr_movwf( "__j" ), pos );
		m_pCode->append( new Instr_movlw( i ), pos );
		m_pCode->append( new Instr_movwf( "__i" ), pos );

		mb->addDelayRoutineWanted( Delay_768uS );
	}

	else
	{
		length_us += -1;
		int i = length_us/3;

		m_pCode->append( new Instr_incf( "__l", 1 ), pos );
		m_pCode->append( new Instr_incf( "__k", 1 ), pos );
		m_pCode->append( new Instr_incf( "__j", 1 ), pos );
		m_pCode->append( new Instr_movlw( i ), pos );
		m_pCode->append( new Instr_movwf( "__i" ), pos );

		mb->addDelayRoutineWanted( Delay_3uS );
	}

	m_pCode->append( new Instr_call( "__delay_subroutine"_L1), pos );
}


void PIC14::addCommonFunctions( DelaySubroutine delay )
{
	if ( delay != Delay_None )
	{
		QString subName = "__delay_subroutine"_L1;
		m_pCode->queueLabel( subName, Code::Subroutine );

		m_pCode->append( new Instr_decfsz( "__i", 1 ), Code::Subroutine );
		m_pCode->append( new Instr_goto( subName ), Code::Subroutine );

		if ( delay > Delay_3uS )
		{
			m_pCode->append( new Instr_decfsz( "__j", 1 ), Code::Subroutine );
			m_pCode->append( new Instr_goto( subName ), Code::Subroutine );
		}

		if ( delay > Delay_768uS )
		{
			m_pCode->append( new Instr_decfsz( "__k", 1 ), Code::Subroutine );
			m_pCode->append( new Instr_goto( subName ), Code::Subroutine );
		}

		if ( delay > Delay_200mS )
		{
			m_pCode->append( new Instr_decfsz( "__l", 1 ), Code::Subroutine );
			m_pCode->append( new Instr_goto( subName ), Code::Subroutine );
		}

		m_pCode->append( new Instr_return(), Code::Subroutine );
	}
}


void PIC14::SsevenSegment( const Variable & pinMap )
{
	assert( pinMap.type() == Variable::sevenSegmentType );
	assert( pinMap.portPinList().size() == 7 );

	QString subName = QString("__output_seven_segment_%1"_L1).arg( pinMap.name() );

	m_pCode->append( new Instr_call( subName ) );

	if ( m_pCode->instruction(subName) )
		return;

	// Build up what are going to write to each port from the pin map
	struct SSPortOutput
	{
		bool used; // Whether we use this port at all
		bool use[8]; // Whether or not we use each pin.
		bool out[16][8]; // The bit to write to each pin for each value.
		uchar useMask; // The bits of use[8] - this is generated later from use[8]
	};

	unsigned numPorts = 2;
	SSPortOutput portOutput[ 2 ];  // numPorts
	memset( portOutput, 0, numPorts * sizeof(SSPortOutput) );

	for ( unsigned i = 0; i < 7; ++i )
	{
		PortPin portPin = pinMap.portPinList()[i];

		unsigned port = unsigned( portPin.portPosition() );
		unsigned pin = unsigned( portPin.pin() );

		portOutput[ port ].used = true;
		portOutput[ port ].use[ pin ] = true;

		for ( unsigned num = 0; num < 16; ++num )
		{
			portOutput[ port ].out[ num ][ pin ] = LEDSegTable[num][ i ];
		}
	}


	// See if we've used more than one port
	unsigned portsUsed = 0;
	for ( unsigned port = 0; port < numPorts; ++port )
	{
		if ( portOutput[port].used )
			portsUsed++;
	}


	// Generate the useMasks
	for ( unsigned port = 0; port < numPorts; ++port )
	{
		portOutput[port].useMask = 0;
		for ( unsigned pin = 0; pin < 8; ++pin )
			portOutput[port].useMask |= portOutput[port].use[pin] ? (1 << pin) : 0;
	}


	//BEGIN Generate [subName] Subroutine
	m_pCode->queueLabel( subName, Code::Subroutine );
// 	if ( portsUsed > 1 )
	{
		m_pCode->append( new Instr_movwf("__i"), Code::Subroutine );
	}

// 	bool overwrittenW = false;
	bool overwrittenW = true;

	for ( unsigned port = 0; port < numPorts; ++port )
	{
		if ( !portOutput[port].used )
			continue;

		QString portName = QString("PORT%1"_L1).arg( char('A'+port) );

		// Save the current value of the port pins that we should not be writing to
		m_pCode->append( new Instr_movf( portName, 0 ), Code::Subroutine );
		m_pCode->append( new Instr_andlw( ~portOutput[port].useMask ), Code::Subroutine );
		m_pCode->append( new Instr_movwf( "__j" ), Code::Subroutine );

		if ( overwrittenW )
			m_pCode->append( new Instr_movf("__i",0), Code::Subroutine );

		m_pCode->append( new Instr_call( subName + QString("_lookup_%1"_L1).arg(port) ), Code::Subroutine );
		overwrittenW = true;

		// Restore the state of the pins which aren't used
		m_pCode->append( new Instr_iorwf( "__j", 0 ), Code::Subroutine );

		// And write the result to the port
		m_pCode->append( new Instr_movwf( portName ), Code::Subroutine );
	}

	m_pCode->append( new Instr_return(), Code::Subroutine );
	//END Generate [subName] Subroutine

	// For each port, generate code for looking up the value for writing to it
	for ( unsigned port = 0; port < numPorts; ++port )
	{
		if ( !portOutput[port].used )
			continue;

		m_pCode->queueLabel( subName + QString("_lookup_%1"_L1).arg(port), Code::LookupTable );
		m_pCode->append( new Instr_andlw(15), Code::LookupTable );

		// Generate the lookup table
		m_pCode->append( new Instr_addwf( "pcl", 1 ), Code::LookupTable );
		for ( unsigned num = 0; num < 16; ++num )
		{
			unsigned literal = 0;
			for ( unsigned bit = 0; bit < 8; ++bit )
				literal += ( portOutput[port].out[num][bit] ? 1 : 0 ) << bit;

			m_pCode->append( new Instr_retlw( literal ), Code::LookupTable );
		}
	}
}


void PIC14::Skeypad( const Variable & pinMap )
{
	// pinMap = 4 rows, n columns

	assert( pinMap.type() == Variable::keypadType );
	assert( pinMap.portPinList().size() >= 7 ); // 4 rows, at least 3 columns

	QString subName = QString("__wait_read_keypad_%1"_L1).arg( pinMap.name() );
	QString waitName = QString("__wait_keypad_%1"_L1).arg( pinMap.name() );
	QString readName = QString("__read_keypad_%1"_L1).arg( pinMap.name() );

	m_pCode->append( new Instr_call( subName ) );

	if ( m_pCode->instruction( subName ) )
		return;

	//BEGIN Wait until read subroutine
	m_pCode->queueLabel( subName, Code::Subroutine );

	// Read current key (if any) from keypad and save to temporary variable
	m_pCode->append( new Instr_call( readName ), Code::Subroutine );
	m_pCode->append( new Instr_movwf( "__m" ), Code::Subroutine );

	// Test if any key was pressed; if not, then start again
// 	std::cout << "mb->alias(\"Keypad_None\"_L1)="<<mb->alias("Keypad_None"_L1) << std::endl;
	m_pCode->append( new Instr_sublw( mb->alias("Keypad_None"_L1).toInt( nullptr, 0 ) ), Code::Subroutine );
	m_pCode->append( new Instr_btfsc( "STATUS","Z" ), Code::Subroutine );
	m_pCode->append( new Instr_goto( subName ), Code::Subroutine );
	m_pCode->append( new Instr_goto( waitName ), Code::Subroutine );
	//END Wait until read subroutine


	//BEGIN Wait until released subroutine
	m_pCode->queueLabel( waitName, Code::Subroutine );

	Sdelay( 10000, Code::Subroutine ); // 10 milliseconds for debouncing

	// Key was pressed; now we wait until the key is released again
	m_pCode->append( new Instr_call( readName ), Code::Subroutine );
	m_pCode->append( new Instr_sublw( mb->alias("Keypad_None"_L1).toInt( nullptr, 0 ) ), Code::Subroutine );
	m_pCode->append( new Instr_btfss( "STATUS","Z" ), Code::Subroutine );
	m_pCode->append( new Instr_goto( waitName ), Code::Subroutine );
	m_pCode->append( new Instr_movf( "__m", 0 ), Code::Subroutine );
	m_pCode->append( new Instr_return(), Code::Subroutine );
	//END Wait until released subroutine


	if ( m_pCode->instruction( readName ) )
		return;

	//BEGIN Read current value of keypad subroutine
	m_pCode->queueLabel( readName, Code::Subroutine );

	// Make the four row lines low
	for ( unsigned row = 0; row < 4; ++ row )
	{
		PortPin rowPin = pinMap.portPinList()[row];
		m_pCode->append( new Instr_bcf( rowPin.port(), QString::number( rowPin.pin() ) ), Code::Subroutine );
	}

	// Test each row in turn
	for ( unsigned row = 0; row < 4; ++ row )
	{
		// Make the high low
		PortPin rowPin = pinMap.portPinList()[row];
		m_pCode->append( new Instr_bsf( rowPin.port(), QString::number( rowPin.pin() ) ), Code::Subroutine );

		for ( unsigned col = 0; col < 3; ++ col )
		{
			PortPin colPin = pinMap.portPinList()[4+col];
			m_pCode->append( new Instr_btfsc( colPin.port(), QString::number( colPin.pin() ) ), Code::Subroutine );
			m_pCode->append( new Instr_retlw( mb->alias( QString("Keypad_%1_%2"_L1).arg(row+1).arg(col+1) ).toInt( nullptr, 0 ) ), Code::Subroutine );
		}

		// Make the low again
		m_pCode->append( new Instr_bcf( rowPin.port(), QString::number( rowPin.pin() ) ), Code::Subroutine );
	}

	// No key was pressed
	m_pCode->append( new Instr_retlw( mb->alias("Keypad_None"_L1).toInt( nullptr, 0 ) ), Code::Subroutine );
	//END Read current value of keypad subroutine
}

/*****************************commented for modification *******************************

void PIC14::bitwise( Expression::Operation op, const QString &r_val1, const QString &val2, bool val1IsNum, bool val2IsNum )
{
	QString val1 = r_val1;
	// There is no instruction for notting a literal,
	// so instead I am going to XOR with 0xFF
	if( op == Expression::bwnot ) val1 = "0xFF";
	if( val1IsNum ) m_pCode->append(new Instr_movlw(val1.toInt( 0, 0 )));// result += instruction("movlw\t"+val1);
	else m_pCode->append(new Instr_movf(val1,0));//result += instruction("movf\t"+val1+",0"_L1);

	QString opString;
	if( val2IsNum )
	{
		switch(op)
		{
			case Expression::bwand: m_pCode->append(new Instr_andlw(val2.toInt( 0, 0 ))); break;
			case Expression::bwor: m_pCode->append(new Instr_iorlw(val2.toInt( 0, 0 ))); break;
			case Expression::bwxor: m_pCode->append(new Instr_xorlw(val2.toInt( 0, 0 ))); break;
			case Expression::bwnot: m_pCode->append(new Instr_xorlw(val2.toInt( 0, 0 ))); break;
			default: break;
		}
	}
	else
	{
		switch(op)
		{
			case Expression::bwand: m_pCode->append(new Instr_andwf(val2,0)); break;
			case Expression::bwor: m_pCode->append(new Instr_iorwf(val2,0)); break;
			case Expression::bwxor: m_pCode->append(new Instr_xorwf(val2,0)); break;
			case Expression::bwnot: m_pCode->append(new Instr_xorwf(val2,0)); break;
			default: break;
		}

	}
}*/
///comment end and the new function is given below -- new code is working well
// TODO - One error with OR operation if A OR 255 result in segebentation fault
//*****************modified to make the bit operation works***************************/
void PIC14::bitwise( Expression::Operation op,const QString & r_val1, const QString & val2, LocationType val1Type, LocationType val2Type)
{
	QString val1 = r_val1;
	if( op == Expression::bwnot ) val1 = "0xFF"_L1;
	switch(val1Type)
	{
		case num: m_pCode->append(new Instr_movlw(val1.toInt( nullptr, 0 ))); break;
		case work: break;
		case var: m_pCode->append(new Instr_movf(val1,0)); break;
	}
	switch(val2Type)
	{
		case num:
		{
			switch(op)
			{
				case Expression::bwand: m_pCode->append(new Instr_andlw(val2.toInt( nullptr, 0 ))); break;
				case Expression::bwor:  m_pCode->append(new Instr_iorlw(val2.toInt( nullptr, 0 ))); break;
				case Expression::bwxor: m_pCode->append(new Instr_xorlw(val2.toInt( nullptr, 0 ))); break;
				case Expression::bwnot: m_pCode->append(new Instr_xorlw(val2.toInt( nullptr, 0 ))); break;
				default: break;
		}
		}
		case work: break;
		case var:
		{
			switch(op)
			{
				case Expression::bwand: m_pCode->append(new Instr_andwf(val2,0)); break;
				case Expression::bwor:  m_pCode->append(new Instr_iorwf(val2,0)); break;
				case Expression::bwxor: m_pCode->append(new Instr_xorwf(val2,0)); break;
				case Expression::bwnot: m_pCode->append(new Instr_xorwf(val2,0)); break;
				default: break;
			}

		}
	}

}//***************************************modification ends*****************************
void PIC14::SincVar( const QString &var )
{
	m_pCode->append(new Instr_incf(var,1) );
}

void PIC14::SdecVar( const QString &var )
{
	m_pCode->append(new Instr_decf(var,1) );
}

void PIC14::SrotlVar( const QString &var )
{
	m_pCode->append(new Instr_rlf(var,1));
}

void PIC14::SrotrVar( const QString &var )
{
	m_pCode->append(new Instr_rrf(var,1));
}

void PIC14::Stristate(const QString &port)
{
//modification pic type is checked here
	m_pCode->append( new Instr_bsf("STATUS","5") );//commented
	if(pic_type== "P16C84"_L1 || pic_type =="P16F84"_L1 ||pic_type =="P16F627"_L1)
	{
		if( port == "trisa"_L1 || port == "TRISA"_L1 )
			saveResultToVar( "TRISA"_L1 );
		else	saveResultToVar( "TRISB"_L1 );
	}
	if(pic_type =="P16F877"_L1)
	{
		if( port == "trisa"_L1 || port == "TRISA"_L1 )
			saveResultToVar( "TRISA"_L1 );
		else if( port == "trisb"_L1 || port == "TRISB"_L1 )
			saveResultToVar( "TRISB"_L1 );
		else if( port == "trisc"_L1 || port == "TRISC"_L1 )
			saveResultToVar( "TRISC"_L1 );
		else if( port == "trisd"_L1 || port == "TRISD"_L1 )
			saveResultToVar( "TRISD"_L1 );
		else	saveResultToVar( "TRISE"_L1 );

	}
	m_pCode->append( new Instr_bcf(Register("STATUS"),"5") );//commented
}

void PIC14::Sasm(const QString &raw)
{
	m_pCode->append(new Instr_asm(raw));
}

//BEGIN class PortPin
PortPin::PortPin( const QString & port, int pin )
{
	m_port = port.toUpper();
	m_pin = pin;
}


PortPin::PortPin()
{
	m_port = QLatin1Char(' ');
	m_pin = -1;
}


int PortPin::portPosition() const
{
	if ( m_port.isEmpty() )
		return 0;
	return uchar( m_port[ m_port.length() - 1 ].toLatin1() ) - 'A';
}

//END class PortPin

