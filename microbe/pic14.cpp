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

#include <cassert>
#include <iostream>
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



PIC14::PIC14( Microbe * master, Type type )
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
		port = QString("PORT%1").arg( portPinString[1].toUpper() );
		pin = QString( portPinString[2] ).toInt();
	}
	// In form e.g. RB.3
	else if ( portPinString.length()  == 4 )//modification change ==3 to ==4
	{
		port = QString("PORT%1").arg( portPinString[1].toUpper() );
		pin = QString( portPinString[3] ).toInt();//modification change 2 to 3
	}
	else
	{
		int dotpos = portPinString.indexOf(".");
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
		    	 cerr << QString(" ERROR: %1 is not a Register bit\n").arg(portPinString ).toStdString();
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
		cerr << QString("ERROR: %1 is not a Port/Register bit\n").arg(portPinString ).toStdString();
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

	qCritical() << Q_FUNC_INFO << "Unknown PIC type = " << m_type << endl;
	return 0xc;
}


PIC14::Type PIC14::toType( const QString & _text )
{
	QString text = _text.toUpper().simplified().remove('P');

	if ( text == "16C84" )
	{
		pic_type="P16C84";
		return P16C84;
	}
	if ( text == "16F84" )
	{
		pic_type="P16F84";
		return P16F84;
	}
	if ( text == "16F627" )
	{
		pic_type="P16F627";
		return P16F627;
	}

	if ( text == "16F628" )
	{
		pic_type="P16F627";
		return P16F628;
	}
//modified checking of 16F877 is included
	if ( text == "16F877" )
	{
		pic_type="P16F877";
		return P16F877;
	}

	cerr << QString("%1 is not a known PIC identifier\n").arg(_text).toStdString();
	return unknown;
}


QString PIC14::minimalTypeString() const
{
	switch ( m_type )
	{
		case P16C84:
			return "16C84";

		case P16F84:
			return "16F84";

		case P16F627:
			return "16F627";

		case P16F628:
			return "16F628";

//modified checking of 16F877 is included

		case P16F877:
			return "16F877";

		case unknown:
			break;
	}

	qCritical() << Q_FUNC_INFO << "Unknown PIC type = " << m_type << endl;
	return nullptr;;
}


void PIC14::postCompileConstruct( const QStringList &interrupts )
{
	m_pCode->append( new Instr_raw("\n\tEND\n"), Code::Subroutine );

	if ( interrupts.isEmpty() )
	{
		// If there are no ISRs then we don't need to put in any handler code.
		// Instead, just insert the goto start instruction in case we need to
		// jump past any lookup tabes (and if there are none, then the optimizer
		// will remove the goto instruction).
		m_pCode->append(new Instr_goto("_start"), Code::InterruptHandler);
		m_pCode->queueLabel( "_start", Code::LookupTable );
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
	m_pCode->append(new Instr_goto("_start"), Code::InterruptHandler);

	m_pCode->append(new Instr_raw("ORG 0x4"), Code::InterruptHandler);
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
		m_pCode->append(new Instr_goto("_interrupt_" + (*it)), Code::InterruptHandler); // Yes, do its handler routine
		// Otherwise fall through to the next.
	}

	// If there was "somehow" a suprious interrupt there isn't really
	// much we can do about that (??) so just fall through and hope for the worst.

	m_pCode->queueLabel( "_interrupt_end", Code::InterruptHandler );
	m_pCode->append(new Instr_swapf("STATUS_TEMP",0), Code::InterruptHandler );
	m_pCode->append(new Instr_movwf("STATUS"), Code::InterruptHandler );
	m_pCode->append(new Instr_swapf("W_TEMP",1), Code::InterruptHandler );
	m_pCode->append(new Instr_swapf("W_TEMP",0), Code::InterruptHandler );
	m_pCode->append(new Instr_retfie()); // Returns and renables globally interrupts.

	m_pCode->queueLabel( "_start", Code::LookupTable );
}

int PIC14::interruptNameToBit(const QString &name, bool flag)
{
	// 7 --- GIE EEIE T0IE INTE RBIE T0IF INTF RBIF --- 0

	if( name == "change" ) // RB
	{
		if(flag) return 0;
		else return 3;
	}
	else if( name == "timer" )
	{
		if(flag) return 2;
		else return 5;
	}
	else if( name == "external" )
	{
		if(flag) return 1;
		else return 4;
	}

	return -1;
}


bool PIC14::isValidPort( const QString & portName ) const
{

	if(pic_type =="P16F84"||pic_type =="P16C84"||pic_type =="P16F627"||pic_type =="P16F628")
		return ( portName == "PORTA" || portName == "PORTB");

	if(pic_type=="P16F877")
		return ( portName == "PORTA" ||portName == "PORTB"||portName == "PORTC" ||portName == "PORTD"||portName == "PORTE");

	return false;
}


bool PIC14::isValidPortPin( const PortPin & portPin ) const
{

	if(pic_type == "P16F84" ||pic_type =="P16C84")
	{
		if ( portPin.port() == "PORTA" )
			return (portPin.pin() >= 0) && (portPin.pin() <= 4);

		if ( portPin.port() == "PORTB" )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);
	}
	if(pic_type == "P16F627" ||pic_type =="P16F628")
	{
		if ( portPin.port() == "PORTA" )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);

		if ( portPin.port() == "PORTB" )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);
	}

	if(pic_type=="P16F877")
	{
		if ( portPin.port() == "PORTA" )
			return (portPin.pin() >= 0) && (portPin.pin() <= 5);

		if ( portPin.port() == "PORTB" )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);
		if ( portPin.port() == "PORTC" )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);

		if ( portPin.port() == "PORTD" )
			return (portPin.pin() >= 0) && (portPin.pin() <= 7);

		if ( portPin.port() == "PORTE" )
			return (portPin.pin() >= 0) && (portPin.pin() <= 2);
	}

	return false;
}


bool PIC14::isValidTris( const QString & trisName ) const
{
	if(pic_type =="P16F84"||pic_type =="P16C84"||pic_type =="P16F627"||pic_type =="P16F628")
		return ( trisName == "TRISA" || trisName == "TRISB");

	if(pic_type=="P16F877")
		return ( trisName =="TRISA"|| trisName =="TRISB"||trisName =="TRISC"||trisName == "TRISD"||trisName == "TRISE" );

	return false;
}

//*****************Modified ****************************//
//New function isValiedRegister is added to check whether a register is valied or not
bool PIC14::isValidRegister( const QString & registerName)const
{
 	if(pic_type=="P16F84"||pic_type=="P16C84")
		return ( registerName == "TMR0"
			|| registerName == "PCL"
			|| registerName == "STATUS"
			|| registerName == "FSR"
			|| registerName == "EEDATH"
			|| registerName == "EEADR"
			|| registerName == "PCLATH"
			|| registerName == "INTCON"
			|| registerName == "EECON1"
			|| registerName == "EECON2"
			|| registerName == "OPTION_REG");

	if(pic_type=="P16F877")
		return ( registerName == "TMR0"
			|| registerName == "PCL"
			|| registerName == "STATUS"
			|| registerName == "FSR"
			|| registerName == "PCLATH"
			|| registerName == "INTCON"
			|| registerName == "PIR1"
			|| registerName == "PIR2"
			|| registerName == "TMR1L"
			|| registerName == "TMR1H"
			|| registerName == "T1CON"
			|| registerName == "TMR2"
			|| registerName == "T2CON"
			|| registerName == "SSPBUF"
			|| registerName == "SSPCON"
			|| registerName == "CCPR1L"
			|| registerName == "CCPR1H"
			|| registerName == "CCP1CON"
			|| registerName == "RCSTA"
			|| registerName == "TXREG"
			|| registerName == "RCREG"
			|| registerName == "CCPR2L"
			|| registerName == "CCPR2H"
			|| registerName == "CCP2CON"
			|| registerName == "ADRESH"
			|| registerName == "ADCON0" /*bank0ends*/
			|| registerName == "OPTION_REG"
			|| registerName == "PIE1"
			|| registerName == "PIE2"
			|| registerName == "PCON"
			|| registerName == "SSPCON2"
			|| registerName == "PR2"
			|| registerName == "SSPADD"
			|| registerName == "SSPSTAT"
			|| registerName == "TXSTA"
			|| registerName == "SPBRG"
			|| registerName == "ADRESL"
			|| registerName == "ADCON1" /*bank1ends*/
			|| registerName == "EEDATA"
			|| registerName == "EEADR"
			|| registerName == "EEDATH"
			|| registerName == "EEADRH" /*bank2ends*/
			|| registerName == "EECON1"
			|| registerName == "EECON2" /*bank3ends*/   );

	if(pic_type=="P16F627"||pic_type=="P16F628")
		return ( registerName == "TMR0"
			|| registerName == "PCL"
			|| registerName == "STATUS"
			|| registerName == "FSR"
			|| registerName == "PCLATH"
			|| registerName == "INTCON"
			|| registerName == "PIR1"
			|| registerName == "TMR1L"
			|| registerName == "TMR1H"
			|| registerName == "T1CON"
			|| registerName == "TMR2"
			|| registerName == "T2CON"
			|| registerName == "CCPR1L"
			|| registerName == "CCPR1H"
			|| registerName == "CCP1CON"
			|| registerName == "RCSTA"
			|| registerName == "TXREG"
			|| registerName == "RCREG"
			|| registerName == "CMCON"/*bank0ends*/
			|| registerName == "OPTION_REG"
			|| registerName == "PIE1"
			|| registerName == "PCON"
			|| registerName == "PR2"
			|| registerName == "TXSTA"
			|| registerName == "SPBRG"
			|| registerName == "EEDATA"
			|| registerName == "EEADR"
			|| registerName == "EECON1"
			|| registerName == "EECON2"
			|| registerName == "VRCON"/*bank1ends*/ );

	return false;
}

//****************************modifications ends********************************************

bool PIC14::isValidInterrupt( const QString & interruptName ) const
{
	if(pic_type == "P16F84" ||pic_type =="P16C84"||pic_type =="P16F877"||pic_type=="P16F627"||pic_type=="P16F628")
		return ( interruptName == "change" ||
				 interruptName == "timer" ||
				 interruptName == "external" );

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
	m_pCode->queueLabel( "_interrupt_" + procName, Code::Subroutine );

	// Clear the interrupt flag for this particular interrupt source
	m_pCode->append( new Instr_bcf("INTCON",QString::number(interruptNameToBit(procName,true))) );
	m_pCode->merge( subCode, Code::Subroutine );

	m_pCode->append( new Instr_goto("_interrupt_end"), Code::Subroutine );
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
	m_pCode->append(new Instr_call("__picfunc_multiply"));
	m_pCode->append(new Instr_movf("__result",0));
}


void PIC14::multiply()
{
	if ( m_pCode->instruction("__picfunc_multiply") )
		return;

	m_pCode->queueLabel( "__picfunc_multiply", Code::Subroutine );
	m_pCode->append(new Instr_clrf("__result"), Code::Subroutine ); //result+=m_pCode->appenduction("clrf __result");

	m_pCode->queueLabel( "__picfunc_multiply_loop", Code::Subroutine );
	m_pCode->append(new Instr_movf("__i",0), Code::Subroutine ); //result+=m_pCode->appenduction("movf __i,0");
	m_pCode->append(new Instr_btfsc("__j","0"), Code::Subroutine ); //result+=m_pCode->appenduction("btfsc __j,0");
	m_pCode->append(new Instr_addwf("__result",1), Code::Subroutine ); //result+=m_pCode->appenduction("addwf __result,1");
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine ); //result+=m_pCode->appenduction("bcf STATUS,C");
	m_pCode->append(new Instr_rrf("__j",1), Code::Subroutine ); //result+=m_pCode->appenduction("rrf __j,1");
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine ); //result+=m_pCode->appenduction("bcf STATUS,C");
	m_pCode->append(new Instr_rlf("__i",1), Code::Subroutine ); //result+=m_pCode->appenduction("rlf __i,1");
	m_pCode->append(new Instr_movf("__j",1), Code::Subroutine ); //result+=m_pCode->appenduction("movf __j,1");
	m_pCode->append(new Instr_btfss("STATUS","Z"), Code::Subroutine ); //result+=m_pCode->appenduction("btfss STATUS,Z");
	m_pCode->append(new Instr_goto("__picfunc_multiply_loop"), Code::Subroutine ); //result+=m_pCode->appenduction("goto __picfunc_multiply_loop");
	m_pCode->append(new Instr_return(), Code::Subroutine ); //result+=m_pCode->appenduction("return");
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

	m_pCode->append(new Instr_call("__picfunc_divide"));//result+=instruction("call __picfunc_divide");
	m_pCode->append(new Instr_movf("__result",0));//result+=instruction("movf __result,0");
}

void PIC14::divide()
{
	m_pCode->queueLabel( "__picfunc_divide", Code::Subroutine );
	m_pCode->append(new Instr_movf("__j",1), Code::Subroutine );
	m_pCode->append(new Instr_btfsc("STATUS","2"), Code::Subroutine );
	m_pCode->append(new Instr_return(), Code::Subroutine );
	m_pCode->append(new Instr_clrf("__result"), Code::Subroutine );
	m_pCode->append(new Instr_movlw(1), Code::Subroutine );
	m_pCode->append(new Instr_movwf("__k"), Code::Subroutine );

	m_pCode->queueLabel( "__divide_shift", Code::Subroutine );
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_rlf("__k",1), Code::Subroutine );
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_rlf("__j",1), Code::Subroutine );
	m_pCode->append(new Instr_btfss("__j","7"), Code::Subroutine );
	m_pCode->append(new Instr_goto("__divide_shift"), Code::Subroutine );

	m_pCode->queueLabel( "__divide_loop", Code::Subroutine );
	m_pCode->append(new Instr_movf("__j",0), Code::Subroutine );
	m_pCode->append(new Instr_subwf("__i",1), Code::Subroutine );
	m_pCode->append(new Instr_btfsc("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_goto("__divide_count"), Code::Subroutine );
	m_pCode->append(new Instr_addwf("__i",1), Code::Subroutine );
	m_pCode->append(new Instr_goto("__divide_final"), Code::Subroutine );

	m_pCode->queueLabel( "__divide_count", Code::Subroutine );
	m_pCode->append(new Instr_movf("__k",0), Code::Subroutine );
	m_pCode->append(new Instr_addwf("__result",1), Code::Subroutine );

	m_pCode->queueLabel( "__divide_final", Code::Subroutine );
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_rrf("__j",1), Code::Subroutine );
	m_pCode->append(new Instr_bcf("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_rrf("__k",1), Code::Subroutine );
	m_pCode->append(new Instr_btfss("STATUS","C"), Code::Subroutine );
	m_pCode->append(new Instr_goto("__divide_loop"), Code::Subroutine );
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
			qCritical() << Q_FUNC_INFO << "Cannot subtract working from working!" << endl;
			break;

		case var:
			m_pCode->append(new Instr_subwf(val2,0));
			break;
	}
}

void PIC14::equal( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type )
{
	ifInitCode( val1, val2, val1Type, val2Type );
	const QString labelEnd = mb->uniqueLabel()+"_endif";
	const QString labelFalse = mb->uniqueLabel()+"_case_false";

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
	const QString labelEnd = mb->uniqueLabel()+"_endif";
	const QString labelFalse = mb->uniqueLabel()+"_case_false";

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
	const QString labelEnd = mb->uniqueLabel()+"_endif";
	const QString labelFalse = mb->uniqueLabel()+"_case_false";

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
	const QString labelEnd = mb->uniqueLabel()+"_endif";
	const QString labelFalse = mb->uniqueLabel()+"_case_false";

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
	const QString labelEnd = mb->uniqueLabel()+"_endif";
	const QString labelTrue = mb->uniqueLabel()+"_case_true"; // Note that unlike the others, this is labelTrue, not labelFalse

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
	const QString labelEnd = mb->uniqueLabel()+"_endif";
	const QString labelFalse = mb->uniqueLabel()+"_case_false";

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

	if ( step == "1" )
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
	//result +=instruction((QString)(NOT?"btfsc":"btfss")+"\t"+port+","+pin);
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

	m_pCode->append( new Instr_call( "__delay_subroutine"), pos );
}


void PIC14::addCommonFunctions( DelaySubroutine delay )
{
	if ( delay != Delay_None )
	{
		QString subName = "__delay_subroutine";
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

	QString subName = QString("__output_seven_segment_%1").arg( pinMap.name() );

	m_pCode->append( new Instr_call( subName ) );

	if ( m_pCode->instruction(subName) )
		return;

	// Build up what are going to write to each port from the pin map
	struct SSPortOutput
	{
		bool used; // Wheter we use this port at all
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

		QString portName = QString("PORT%1").arg( char('A'+port) );

		// Save the current value of the port pins that we should not be writing to
		m_pCode->append( new Instr_movf( portName, 0 ), Code::Subroutine );
		m_pCode->append( new Instr_andlw( ~portOutput[port].useMask ), Code::Subroutine );
		m_pCode->append( new Instr_movwf( "__j" ), Code::Subroutine );

		if ( overwrittenW )
			m_pCode->append( new Instr_movf("__i",0), Code::Subroutine );

		m_pCode->append( new Instr_call( subName + QString("_lookup_%1").arg(port) ), Code::Subroutine );
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

		m_pCode->queueLabel( subName + QString("_lookup_%1").arg(port), Code::LookupTable );
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

	QString subName = QString("__wait_read_keypad_%1").arg( pinMap.name() );
	QString waitName = QString("__wait_keypad_%1").arg( pinMap.name() );
	QString readName = QString("__read_keypad_%1").arg( pinMap.name() );

	m_pCode->append( new Instr_call( subName ) );

	if ( m_pCode->instruction( subName ) )
		return;

	//BEGIN Wait until read subroutine
	m_pCode->queueLabel( subName, Code::Subroutine );

	// Read current key (if any) from keypad and save to temporary variable
	m_pCode->append( new Instr_call( readName ), Code::Subroutine );
	m_pCode->append( new Instr_movwf( "__m" ), Code::Subroutine );

	// Test if any key was pressed; if not, then start again
// 	std::cout << "mb->alias(\"Keypad_None\")="<<mb->alias("Keypad_None") << std::endl;
	m_pCode->append( new Instr_sublw( mb->alias("Keypad_None").toInt( nullptr, 0 ) ), Code::Subroutine );
	m_pCode->append( new Instr_btfsc( "STATUS","Z" ), Code::Subroutine );
	m_pCode->append( new Instr_goto( subName ), Code::Subroutine );
	m_pCode->append( new Instr_goto( waitName ), Code::Subroutine );
	//END Wait until read subroutine


	//BEGIN Wait until released subroutine
	m_pCode->queueLabel( waitName, Code::Subroutine );

	Sdelay( 10000, Code::Subroutine ); // 10 milliseconds for debouncing

	// Key was pressed; now we wait until the key is released again
	m_pCode->append( new Instr_call( readName ), Code::Subroutine );
	m_pCode->append( new Instr_sublw( mb->alias("Keypad_None").toInt( nullptr, 0 ) ), Code::Subroutine );
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
			m_pCode->append( new Instr_retlw( mb->alias( QString("Keypad_%1_%2").arg(row+1).arg(col+1) ).toInt( nullptr, 0 ) ), Code::Subroutine );
		}

		// Make the low again
		m_pCode->append( new Instr_bcf( rowPin.port(), QString::number( rowPin.pin() ) ), Code::Subroutine );
	}

	// No key was pressed
	m_pCode->append( new Instr_retlw( mb->alias("Keypad_None").toInt( nullptr, 0 ) ), Code::Subroutine );
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
	else m_pCode->append(new Instr_movf(val1,0));//result += instruction("movf\t"+val1+",0");

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
///comment end and the new function is given bellow -- new code is working well
// TODO - One error with OR operation if A OR 255 result in segebentation fault
//*****************modified to make the bit operation works***************************/
void PIC14::bitwise( Expression::Operation op,const QString & r_val1, const QString & val2, LocationType val1Type, LocationType val2Type)
{
	QString val1 = r_val1;
	if( op == Expression::bwnot ) val1 = "0xFF";
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
	if(pic_type== "P16C84" || pic_type =="P16F84"||pic_type =="P16F627")
	{
		if( port == "trisa" || port == "TRISA" )
			saveResultToVar( "TRISA" );
		else	saveResultToVar( "TRISB" );
	}
	if(pic_type =="P16F877")
	{
		if( port == "trisa" || port == "TRISA" )
			saveResultToVar( "TRISA" );
		else if( port == "trisb" || port == "TRISB" )
			saveResultToVar( "TRISB" );
		else if( port == "trisc" || port == "TRISC" )
			saveResultToVar( "TRISC" );
		else if( port == "trisd" || port == "TRISD" )
			saveResultToVar( "TRISD" );
		else	saveResultToVar( "TRISE" );

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
	m_port = ' ';
	m_pin = -1;
}


int PortPin::portPosition() const
{
	if ( m_port.isEmpty() )
		return 0;
	return uchar( m_port[ m_port.length() - 1 ].toLatin1() ) - 'A';
}

//END class PortPin

