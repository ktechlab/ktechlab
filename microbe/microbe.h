/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke                              *
 *   daniel.jc@gmail.com                                                   *
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

#ifndef MICROBE_H
#define MICROBE_H

#include <instruction.h>
#include <variable.h>
// #include <pic14.h>

#include <Qt/qmap.h>
#include <Qt/qstring.h>
#include <Qt/qstringlist.h>

class QString;
class BTreeBase;
class BTreeNode;
class Code;
class PIC14;
class PortPin;

typedef QList<PortPin> PortPinList;

typedef QList<Variable> VariableList;
typedef QMap<QString,QString> AliasMap;

enum ExprType
{
	unset		= 1,
	working		= 2,
	number		= 3,
	variable	= 4,
	extpin		= 5,
	keypad		= 6
};


class SourceLine;
typedef QList<SourceLine> SourceLineList;
/**
Represents a source line, with the convention of line number starting at zero.
@author David Saxton
*/
class SourceLine
{
	public:
		/**
		 * The QList template requires a default constructor - calling this
		 * though creates an invalid SourceLine with line() returning -1. So
		 * this constructor should never be used.
		 */
		SourceLine();
		SourceLine( const QString & text, const QString & url, int line );
		
		QString text() const { return m_text; }
		QString url() const { return m_url; }
		int line() const { return m_line; }
		
		/**
		 * Extracts the text from each SourceLine and adds it to the
		 * returned QStringList.
		 */
		static QStringList toStringList( const SourceLineList & lines );
		
	protected:
		QString m_text;
		QString m_url;
		int m_line;
};


	
/**
@author Daniel Clarke
@author David Saxton
*/
class Microbe
{
	public:
		Microbe();
		~Microbe();
		
		enum MistakeType
		{
			UnknownStatement = 1,
			InvalidPort = 2,
			UnassignedPin = 3, // pin identifier without an "= something"
			NonHighLowPinState = 4,
			UnassignedPort = 5, // port identifier without an "= something"
			UnexpectedStatementBeforeBracket = 6,
			MismatchedBrackets = 7,
			InvalidEquals = 8,
			ReservedKeyword = 9,
			ConsecutiveOperators = 10,
			MissingOperator = 11,
			UnknownVariable = 12,
			UnopenableInclude = 16,
			DivisionByZero = 17,
			NumberTooBig = 18,
			NonConstantStep = 19,
			NonConstantDelay = 20,
			HighLowExpected = 21,
			InvalidComparison = 22,
			SubBeforeEnd = 23,
			LabelExpected = 24,
			TooManyTokens = 25,
			FixedStringExpected = 26,
			PinListExpected = 27,
			AliasRedefined = 28,
			InvalidInterrupt = 29,
			InterruptRedefined = 30,
			InterruptBeforeEnd = 31,
			ReadOnlyVariable = 32,
			WriteOnlyVariable = 33,
			InvalidPinMapSize = 34,
			VariableRedefined = 35,
			InvalidVariableName = 36,
			VariableExpected = 40,
			NameExpected = 41
		};
	
		/**
		 * Returns a list of errors occurred during compilation, intended for
		 * outputting to stderr.
		 */
		QString errorReport() const { return m_errorReport; }
		/**
		 * Call this to compile the given code. This serves as the top level of
		 * recursion as it performs initialisation of things, to recurse at
		 * levels use parseUsingChild(), or create your own Parser.
		 * @param url is used for reporting errors
		 */
		QString compile( const QString & url, bool optimize );
		/**
		 * Adds the given compiler error at the file line number to the
		 * compilation report.
		 */
		void compileError( MistakeType type, const QString & context, const SourceLine & sourceLine );
		/**
		 * This is for generating unique numbers for computer generated labels.
		 */
		QString uniqueLabel() { return QString("__%1").arg(m_uniqueLabel++); }
		/**
		 * If alias is an alias for something then it returns that something,
		 * otherwise it just returns alias (which in that case is not an alias!)
		 */
		QString alias( const QString & alias ) const;
		/**
		 * Aliases the name to the dest.
		 */
		void addAlias( const QString & name, const QString & dest );
		/**
		 * Tell Microbe that a minimum of the given delay routine needs to be
		 * created.
		 * @see PIC14::DelaySubroutine
		 * @param routine - DelaySubroutine enum, higher is more priority
		 */
		void addDelayRoutineWanted( unsigned routine );
		/**
		 * Makes a new PIC assembly object, based on the PIC string that the
		 * user has given in the source.
		 */
		PIC14 * makePic();
		/**
		 * Add the interrupt as being used, i.e. make sure there is one and only
		 * one occurance of its name in m_usedInterrupts.
		 */ 
		void setInterruptUsed( const QString & interruptName );
		/**
		 * @returns whether the given interrupt has already been used.
		 */
		bool isInterruptUsed( const QString & interruptName );
		/**
		 * @returns whether the variable name is valid.
		 */
		static bool isValidVariableName( const QString & variableName );
		/**
		 * Appends the given variable name to the variable list.
		 */
		void addVariable( const Variable & variable );
		/**
		 * @returns the variable with the given name, or one of invalidType if
		 * no such variable exists.
		 */
		Variable variable( const QString & variableName ) const;
		/**
		 * @returns whether the variable has been declared yet.
		 */
		bool isVariableKnown( const QString & variableName ) const;
		/**
		 * This is used as a temporary variable while evaluating an expression.
		 */
		QString dest() const;
		void incDest();
		void decDest();
		void resetDest();
	
	protected:
		/**
		 * Strips comments from m_program, simplifies the white space in each line,
		 * puts braces on separate lines, and then removes any blank lines.
		 */
		void simplifyProgram();
	
		QStringList m_usedInterrupts;
		SourceLineList m_program;
		QString m_errorReport;
		int m_uniqueLabel;
		VariableList m_variables;
		int m_dest;
		unsigned m_maxDelaySubroutine;
	
		/**
		 * Keeps a list of aliases that have been created which maps the key as
		 * the alias text to the data which is the thing being aliased, so that
		 * something can be  aliased to two different things. e.g. 
		 * alias ken bob
		 * alias mary bob
		 */
		QMap<QString,QString> m_aliasList;
		/**
		 * Once the child parser has found it, this is set to the pic type
		 * string found in the source file. The pic type directive must be
		 * the first thing in the microbe program, before even includes.
		 * @see PIC14::Type
		 */
		int m_picType;
};


#endif

