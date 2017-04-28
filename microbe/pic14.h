/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke                              *
 *   daniel.jc@gmail.com                                                   *
 *									   *
 *   24-04-2007                                                            *
 *   Modified to add pic 16f877,16f627 and 16f628 			   *
 *   by george john george@space-kerala.org 				   *
 *   supported by SPACE www.space-kerala.org				   *
 *                                                                          *
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

#ifndef PIC14_H
#define PIC14_H

#include "expression.h"
#include "microbe.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qlist.h>

class Code;
class Microbe;
class Parser;

/**
@author David Saxton
 */
class PortPin
{
	public:
		PortPin( const QString & port, int pin );
		/**
		 * Creates an invalid PortPin ( pin() will return -1).
		 */
		PortPin();
		/**
		 * Returns port (uppercase).
		 */
		QString port() const { return m_port; }
		/**
		 * Returns the port position (e.g. "PORTA" is 0, "PORTB" is 1, etc).
		 */
		int portPosition() const;
		/**
		 * Returns the pin (-1 == invalid PortPin).
		 */
		int pin() const { return m_pin; }
		
	protected:
		QString m_port;
		int m_pin;
};
typedef QList<PortPin> PortPinList;


/**
@author Daniel Clarke
@author David Saxton
*/
class PIC14
{
	public:
		enum Type
		{
			P16C84,
			P16F84,
			P16F627,
			P16F628,
			P16F877,
			unknown
		};
		enum LocationType
		{
			num = 1,
			work = 2,
			var = 3
		};
		/**
		 * Used in determining which delay subroutine should be created.
		 */
		enum DelaySubroutine
		{
			Delay_None	= 0,
			Delay_3uS	= 1,
			Delay_768uS	= 2,
			Delay_200mS	= 3,
			Delay_50S	= 4
		};
		
		/*PIC14::*/PIC14( Microbe * master, Type type );
		~PIC14();
		
		/**
		 * Tries to convert the string to a PIC type, returning unknown if
		 * unsuccessful.
		 */
		static Type toType( const QString & text );
		/**
		 * @return the PIC type.
		 */
		Type type() const { return m_type; }
		/**
		 * @return the Type as a string without the P at the front.
		 */
		QString minimalTypeString() const;
		/**
		 * Translates the portPinString (e.g. "PORTA.2") into a PortPin if the port
		 * and pin combination is valid (otherwise returns an invalid PortPin with
		 * a pin of -1.
		 */
		PortPin toPortPin( const QString & portPinString );
		/**
		 * Returns the address that the General Purpose Registers starts at.
		 */
		uchar gprStart() const;
		
		void setParser(Parser *parser) { m_parser = parser; }
		void setCode( Code * code ) { m_pCode = code; }
		void mergeCode( Code * code );
	
		void setConditionalCode( Code * ifCode, Code * elseCode );
		Code * ifCode();
		Code * elseCode();
	
		Code * m_ifCode;
		Code * m_elseCode;

		void postCompileConstruct( const QStringList &interrupts );

		/**
		 * @returns whether or not the port is valid.
		 * @see isValidPortPin
		 */
		bool isValidPort( const QString & portName ) const;
		/**
		 * @returns whether or not the port and pin is a valid combination.
		 * @see isValidPort
		 */
		bool isValidPortPin( const PortPin & portPin ) const;
		bool isValidTris( const QString & trisName ) const;
		bool isValidInterrupt( const QString & interruptName ) const;

///modified new function isValidRegister is added ******************

		bool isValidRegister( const QString & interruptName ) const;
//		bool isValidRegisterBit( const QString & interruptName ) const; 
//TODO GIE=high
//******************************modification ends***********************************
		void Sgoto(const QString &label);
		void Slabel(const QString &label);
		void Send();
		void Ssubroutine(const QString &procName, Code * compiledProcCode);
		void Sinterrupt(const QString & procName, Code * compiledProcCode);
		void Scall(const QString &name);
	
		void Ssetlh( const PortPin & portPin, bool high);
	
		void add( QString val1, QString val2, LocationType val1Type, LocationType val2Type );
		void subtract( const QString & val1, const QString & val2, LocationType val1Type, LocationType val2Type );
		void mul( QString val1, QString val2, LocationType val1Type, LocationType val2Type);
		void div( const QString & val1, const QString & val2, LocationType val1Type, LocationType val2Type);
	
		void assignNum(const QString & val);
		void assignVar(const QString & val);
	
		void saveToReg(const QString &dest);
		/**
		 * Move the contents of the working register to the register with the given
		 * name.
		 */
		void saveResultToVar( const QString & var );
		/** Code for "==" */
		void equal( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type );
		/** Code for "!=" */
		void notEqual( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type );
		/** Code for ">" */
		void greaterThan( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type );
		/** Code for "<" */
		void lessThan( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type );
		/** Code for ">=" */
		void greaterOrEqual( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type );
		/** Code for "<=" */
		void lessOrEqual( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type );

///*****modified the function **************

		//commented for new function since it is not working 
//		void bitwise( Expression::Operation op, const QString &val1, const QString &val2, bool val1IsNum, bool val2IsNum );
		//code for AND OR XOR opertions 
		void bitwise( Expression::Operation op,const QString & val1, const QString & val2, LocationType val1Type, LocationType val2Type);

//*******************modification end  ---Result --- new code is working well**************
	
		void Swhile( Code * whileCode, const QString &expression);
		void Srepeat( Code * repeatCode, const QString &expression);
		void Sif( Code * ifCode, Code * elseCode, const QString &expression);
		void Sfor( Code * forCode, Code * initCode, const QString &expression, const QString &variable, const QString &step, bool stepPositive);
	
		void Spin( const PortPin & portPin, bool NOT);
		void addCommonFunctions( DelaySubroutine delay );
	
		//BEGIN "Special Functionality" functions
		/**
		 * Delay the program execution, for the given period of length_us (unit:
		 * microseconds).
		 * @param pos the position to add the code for calling the delay subroutine.
		 */
		void Sdelay( unsigned length_us, Code::InstructionPosition pos = Code::Middle );
		/**
		 * Output the working register to the given seven segment.
		 * @param name The variable giving the pin configuration of the seven
		 * segment.
		 */
		void SsevenSegment( const Variable & pinMap );
		/**
		 * Read the value of the keypad to the working register.
		 * @param name The variable giving the pin configuration of the keypad.
		 */
		void Skeypad( const Variable & pinMap );
		//END "Special Functionality" functions
	
		void SincVar( const QString &var );
		void SdecVar( const QString &var );
		void SrotlVar( const QString &var );
		void SrotrVar( const QString &var );
	
		void Stristate( const QString &port );
	
		void Sasm(const QString &raw);

	
	protected:
		void multiply();
		void divide();
	
		/** @see Microbe::m_picType */
		Type m_type;
	
		Parser * m_parser;
		Microbe * mb;
		Code * m_pCode;
	
		void ifInitCode( const QString &val1, const QString &val2, LocationType val1Type, LocationType val2Type );
	
		/**
		 * The function makes sure that val1 always contains a working register
		 * variable, if one has been passed, this is done by swapping val1 and val2 when
		 * neccessary
		 */
		void rearrangeOpArguments( QString * val1, QString * val2, LocationType * val1Type, LocationType * val2Type);
	
		/**
		 * @param flag True means give flag bit, false means give enable bit instead
		 */
		int interruptNameToBit(const QString &name, bool flag);
};

#endif
