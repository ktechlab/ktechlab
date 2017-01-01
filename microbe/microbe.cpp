/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke   daniel.jc@gmail.com        *
 *   Copyright (C)      2005 by David Saxton                               *
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
#include "microbe.h"
#include "parser.h"
#include "optimizer.h"
#include "pic14.h"

#include <kdebug.h>
#include <klocale.h>
#include <Qt/qfile.h>

#include <iostream>
using namespace std;


//BEGIN class Microbe
Microbe::Microbe()
{
	m_maxDelaySubroutine = PIC14::Delay_None;
	m_dest = 0;
	m_uniqueLabel = 0;
	
	// Hardwired constants
	m_aliasList["true"] = "1";
	m_aliasList["false"] = "0";
	// Things starting with b are reserved by gpasm (for binary numbers)
	m_aliasList["b"] = "_b";
	
	//BEGIN Keypad values
	int bv[4][6] = {
		{ 1,   2, 3,   10, 14, 18 },
		{ 4,   5, 6,   11, 15, 19 },
		{ 7,   8, 9,   12, 16, 20 },
		{ 253, 0, 254, 13, 17, 21 } };
	
	for ( unsigned row = 0; row < 4; ++row )
	{
		for ( unsigned col = 0; col < 6; ++col )
		{
			m_aliasList[ QString("Keypad_%1_%2").arg(row+1).arg(col+1) ] = QString::number( bv[row][col] );
		}
	}
	
	m_aliasList[ "Keypad_None" ] = "0xff";
	//END Keypad values
}


Microbe::~Microbe()
{
}


QString Microbe::compile( const QString & url, bool optimize )
{
	QFile file( url );
	if( file.open( QIODevice::ReadOnly ) )
	{
		QTextStream stream(&file);
		unsigned line = 0;
		while( !stream.atEnd() )
			m_program += SourceLine( stream.readLine(), url, line++ );
		file.close();
		simplifyProgram();
	}
	else
	{
		m_errorReport += i18n("Could not open file '%1'\n", url);
		return 0;
	}
	
	Parser parser(this);
	
	// Extract the PIC ID
	if ( !m_program.isEmpty() )
	{
		m_picType = PIC14::toType( m_program[0].text() );
		m_program.erase( m_program.begin() );
	}
	
	PIC14 * pic = makePic();
	if ( !pic )
		return 0;
	
	Code * code = parser.parse( m_program );
	pic->setCode( code );
	pic->addCommonFunctions( (PIC14::DelaySubroutine)m_maxDelaySubroutine );

	pic->postCompileConstruct( m_usedInterrupts );
	code->postCompileConstruct();
	
	if ( optimize )
	{
		Optimizer opt;
		opt.optimize( code );
	}

	return code->generateCode( pic );
}


PIC14 * Microbe::makePic()
{
	return new PIC14( this, (PIC14::Type)m_picType );
}


void Microbe::simplifyProgram()
{
	SourceLineList simplified;
	
	enum CommentType { None, SingleLine, MultiLine };
	CommentType commentType = None;
	
	SourceLineList::const_iterator end = m_program.end();
	for ( SourceLineList::const_iterator it = m_program.begin(); it != end; ++it )
	{
		QString code = (*it).text();
		QString simplifiedLine;
		
		if ( commentType == SingleLine )
			commentType = None;
		
		unsigned l = code.length();
	
		for ( unsigned i = 0; i < l; ++i )
		{
			QChar c = code[i];
            const char cc = c.toLatin1();
			switch ( cc )
			{
				case '/':
					// Look for start of comments in form "//" and "/*"
					
					if ( commentType == None && (i+1 < l) )
					{
						if ( code[i+1] == '/' )
						{
							commentType = SingleLine;
							i++;
						}
				
						else if ( code[i+1] == '*' )
						{
							commentType = MultiLine;
							i++;
						}
					}
					break;
				
				case '*':
					// Look for end of comments in form "*/"
					if ( commentType == MultiLine && (i+1 < l) && code[i+1] == '/' )
					{
						i++;
						commentType = None;
						continue;
					}
					break;
					
				case '{':
				case '}':
					// Put braces on separate lines
					
					if ( commentType != None )
						break;
					
					simplified << SourceLine( simplifiedLine.simplified(), (*it).url(), (*it).line() );
					simplified << SourceLine( c, (*it).url(), (*it).line() );
					
					simplifiedLine = "";
					continue;
			}
		
			if ( commentType == None )
				simplifiedLine += c;
		}
		
		simplified << SourceLine( simplifiedLine.simplified(), (*it).url(), (*it).line() );
	}
	
	m_program.clear();
	end = simplified.end();
	for ( SourceLineList::const_iterator it = simplified.begin(); it != end; ++it )
	{
		if ( !(*it).text().isEmpty() )
			m_program << *it;
	}
}


void Microbe::compileError( MistakeType type, const QString & context, const SourceLine & sourceLine )	
{
	QString message;
	switch (type)
	{
		case UnknownStatement:
			message = i18n("Unknown statement");
			break;
		case InvalidPort:
			message = i18n("Port '%1' is not supported by target PIC", context);
			break;
		case UnassignedPin:
			message = i18n("Pin identifier was not followed by '='");
			break;
		case NonHighLowPinState:
			message = i18n("Pin state can only be 'high' or 'low'");
			break;
		case UnassignedPort:
			message = i18n("Invalid token '%1'. Port identifier should be followed by '='", context);
			break;
		case UnexpectedStatementBeforeBracket:
			message = i18n("Unexpected statement before '{'");
			break;
		case MismatchedBrackets:
			message = i18n("Mismatched brackets in expression '%1'", context);
			break;
		case InvalidEquals:
			message = i18n("Invalid '=' found in expression");
			break;
		case ReservedKeyword:
			message = i18n("Reserved keyword '%1' cannot be a variable name.", context);
			break;
		case ConsecutiveOperators:
			message = i18n("Nothing between operators");
			break;
		case MissingOperator:
			message = i18n("Missing operator or space in operand");
			break;
		case UnknownVariable:
			if ( context.isEmpty() )
				message = i18n("Unknown variable");
			else
				message = i18n("Unknown variable '%1'", context);
			break;
		case UnopenableInclude:
			message = i18n("Could not open include file '%1'", context);
			break;
		case DivisionByZero:
			message = i18n("Division by zero");
			break;
		case NumberTooBig:
			message = i18n("Number too big");
			break;
		case NonConstantStep:
			message = i18n("Step can only be a constant expression");
			break;
		case NonConstantDelay:
			message = i18n("Delay must be a positive constant value");
			break;
		case HighLowExpected:
			message = i18n("'high' or 'low' expected after pin expression '%1'", context);
			break;
		case InvalidComparison:
			message = i18n("Comparison operator in '%1' is not recognized");
			break;
		case SubBeforeEnd:
			message = i18n("Subroutine definition before end of program");
			break;
		case InterruptBeforeEnd:
			message = i18n("Interrupt routine definition before end of program");
			break;
		case LabelExpected:
			message = i18n("Label expected");
			break;
		case TooManyTokens:
			message = i18n("Extra tokens at end of line");
			break;
		case FixedStringExpected:
			message = i18n("Expected '%1'", context);
			break;
		case PinListExpected:
			message = i18n("Pin list expected");
			break;
		case AliasRedefined:
			message = i18n("Alias already defined");
			break;
		case InvalidInterrupt:
			message = i18n("Interrupt type not supported by target PIC");
			break;
		case InterruptRedefined:
			message = i18n("Interrupt already defined");
			break;
		case ReadOnlyVariable:
			message = i18n("Variable '%1' is read only", context);
			break;
		case WriteOnlyVariable:
			message = i18n("Variable '%1' is write only", context);
			break;
		case InvalidPinMapSize:
			message = i18n("Invalid pin list size");
			break;
		case VariableRedefined:
			message = i18n("Variable '%1' is already defined", context);
			break;
		case InvalidVariableName:
			message = i18n("'%1' is not a valid variable name", context);
			break;
		case VariableExpected:
			message = i18n("Variable expected");
			break;
		case NameExpected:
			message = i18n("Name expected");
			break;
	}
	
	
	m_errorReport += QString("%1:%2:Error [%3] %4\n")
			.arg( sourceLine.url() )
			.arg( sourceLine.line()+1 )
			.arg( type )
			.arg( message );
}


bool Microbe::isValidVariableName( const QString & variableName)
{

//*****modified checking is included for preventing the uses of registername as varable name*****

//Prevent usage of register/port name as varable
	if (/*PORT-NAME*/ variableName == "PORTA"
		|| variableName == "PORTB"
		|| variableName == "PORTC"
		|| variableName == "PORTD"
		|| variableName == "PORTE" /*TRIS-NAME*/
		|| variableName == "TRISA"
		|| variableName == "TRISB"
		|| variableName == "TRISC"
		|| variableName == "TRISD"
		|| variableName == "TRISE" /**REGISTER-NAME**/
		|| variableName == "TMR0"
		|| variableName == "PCL"
		|| variableName == "STATUS"
		|| variableName == "FSR"
		|| variableName == "PCLATH"
		|| variableName == "INTCON"
		|| variableName == "PIR1"
		|| variableName == "PIR2"
		|| variableName == "TMR1L"
		|| variableName == "TMR1H"
		|| variableName == "T1CON"
		|| variableName == "TMR2"
		|| variableName == "T2CON"
		|| variableName == "SSPBUF"
		|| variableName == "SSPCON"
		|| variableName == "CCPR1L"
		|| variableName == "CCPR1H"
		|| variableName == "CCP1CON"
		|| variableName == "RCSTA"
		|| variableName == "TXREG"
		|| variableName == "RCREG"
		|| variableName == "CCPR2L"
		|| variableName == "CCPR2H"
		|| variableName == "CCP2CON"
		|| variableName == "ADRESH"
		|| variableName == "ADCON0" /*bank0ends*/
		|| variableName == "OPTION_REG"
		|| variableName == "PIE1"
		|| variableName == "PIE2"
		|| variableName == "PCON"
		|| variableName == "SSPCON2"
		|| variableName == "PR2"
		|| variableName == "SSPADD"
		|| variableName == "SSPSTAT"
		|| variableName == "TXSTA"
		|| variableName == "SPBRG"
		|| variableName == "ADRESL"
		|| variableName == "ADCON1" /*bank1ends*/
		|| variableName == "EEDATA "
		|| variableName == "EEADR"
		|| variableName == "EEDATH"
		|| variableName == "EEADRH" /*bank2ends*/
		|| variableName == "EECON1"
		|| variableName == "EECON2"  /*bank3ends*/ )
			return false;

//****************************modification ends*******************************

	if ( variableName.isEmpty() )
		return false;
	
	if ( !variableName[0].isLetter() && variableName[0] != '_' )
		return false;
	
	for ( unsigned i = 1; i < variableName.length(); ++i )
	{
		if ( !variableName[i].isLetterOrNumber() && variableName[i] != '_' )
			return false;
	}
	
	return true;
}


void Microbe::addVariable( const Variable & variable )
{
	if ( variable.type() == Variable::invalidType )
		return;
	
	if ( !isVariableKnown( variable.name() ) )
		m_variables << variable;
}


Variable Microbe::variable( const QString & name ) const
{
	VariableList::const_iterator end = m_variables.end();
	for ( VariableList::const_iterator it = m_variables.begin(); it != end; ++it )
	{
		if ( (*it).name() == name )
			return *it;
	}
	return Variable();
}


bool Microbe::isVariableKnown( const QString & name ) const
{
	return variable(name).type() != Variable::invalidType;
}


void Microbe::addDelayRoutineWanted( unsigned routine )
{
	if ( m_maxDelaySubroutine < routine )
		m_maxDelaySubroutine = routine;
}


void Microbe::addAlias( const QString & name, const QString & dest )
{
	m_aliasList[name] = dest;
}


QString Microbe::alias( const QString & alias ) const
{
	// If the string is an alias, return the real string,
	// otherwise just return the alias as that is the real string.
	AliasMap::const_iterator it = m_aliasList.find(alias);
	if ( it != m_aliasList.constEnd() )
		return it.value();
	return alias;
}


void Microbe::setInterruptUsed(const QString &interruptName)
{
	// Don't add it again if it is already in the list
	if ( m_usedInterrupts.contains( interruptName ) )
		return;
	m_usedInterrupts.append(interruptName);
}


bool Microbe::isInterruptUsed( const QString & interruptName )
{
	return m_usedInterrupts.contains( interruptName );
}


QString Microbe::dest() const
{
	return QString("__op%1").arg(m_dest);
}


void Microbe::incDest()
{
	m_dest++;
// 	if ( ++m_dest > m_highestDest )
// 		m_highestDest = m_dest;
}


void Microbe::decDest()
{
	m_dest--;
}


void Microbe::resetDest()
{
	m_dest = 0;
}
//END class Microbe



//BEGIN class SourceLine
SourceLine::SourceLine( const QString & text, const QString & url, int line )
{
	m_text = text;
	m_url = url;
	m_line = line;
}


SourceLine::SourceLine()
{
	m_line = -1;
}


QStringList SourceLine::toStringList( const SourceLineList & lines )
{
	QStringList joined;
	SourceLineList::const_iterator end = lines.end();
	for ( SourceLineList::const_iterator it = lines.begin(); it != end; ++it )
		joined << (*it).text();
	return joined;
		
}
//END class SourceLine

