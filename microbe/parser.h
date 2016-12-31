/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke                              *
 *   daniel.jc@gmail.com                                                   *
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

#ifndef PARSER_H
#define PARSER_H

#include "expression.h"
#include "instruction.h"
#include "microbe.h"

#include <Qt/qmap.h>
#include <Qt/qlist.h>

class PIC14;

/**
@author Daniel Clarke
@author David Saxton
*/
class Statement
{
	public:
		/**
		 * Is the assembly output generated for this statement.
		 */
		InstructionList * code;
		/**
		 * The original microbe source line.
		 */
		SourceLine content;
		/**
		 * Returns the microbe code from content.
		 */
		QString text() const { return content.text(); }
		/**
		 * If this Statement is for a for loop, then content will contain
		 * something like "for x = 1 to 10", and bracedCode will contain the
		 * source code within (but not including) the braces.
		 */
		SourceLineList bracedCode;
		/**
		 * Just returns whether or not the braced code is empty.
		 */
		bool hasBracedCode() const { return !bracedCode.isEmpty(); }
		/**
		 * This breaks up the line separated by spaces,{,and =/
		 */
		static QStringList tokenise(const QString &line);
		/**
		 * @see tokenise(const QString &line)
		 */
		QStringList tokenise() const { return tokenise( content.text() ); }
		/**
		 * @returns whether or not the content looks like a label (ends with a
		 * colon).
		 */
		bool isLabel() const { return content.text().right(1) == ":"; }
};

typedef QList<Statement> StatementList;

/**
@author Daniel Clarke
@author David Saxton
*/
class Field
{
	public:
		enum Type
		{
			// String that doesn't change the program logic, but might or might
			// not need to be there depending on the statement (e.g. "then" in
			// the if-statement).
			FixedString,
			
			// Label, Variable, Name are all treated similarly (only different
			// error messages are given).
			Label, // e.g. in "goto [Label]"
			Variable, // e.g. in "increment [Variable]"
			Name, // e.g. in "sevenseg [Name]"
			
			// List of strings which should be pin names.
			PinList,
			
			// Braced code.
			Code,
			
			Expression,
			Newline,
			None
		};
	
		/**
		 * Create a Field of type None.
		 */
		Field();
		/**
		 * Create a Field.
		 */
		Field( Type type, const QString & key = 0 );
		/**
		 * Create a Field (this constructor should only be used with
		 * FixedStrings.
		 */
		Field( Type type, const QString & key, const QString & string, bool compulsory = true);
		
		/**
		 * The type of field expected.
		 */
		Type type() const { return m_type; }
		/**
		 * String data relevant to the field dependent on m_type.
		 */
		QString string() const { return m_string; }
		/**
		 * The key in which the found token will be attached to 
		 * in the output map. If it is an empty string, then the field will be 
		 * processed but not put in the output, effectively ignoring it.
		 */
		QString key() const { return m_key; }
		/**
		 * Only FixedStrings may be compulsory, that is the only type that can
		 * actually have its presence checked.
		 * This flag is set to indicate that no error should be rasied if the 
		 * field is not present. Note that if a field is found missing, then
		 * the rest of the statement is ignored (regardless of whether the rest
		 * is marked compulsory or not.)
		 */
		bool compulsory() const { return m_compulsory; }
	
	private:
		Type m_type;
		QString m_string;
		QString m_key;
		bool m_compulsory;
};


class OutputField
{
	public:
		/**
		 * Constructs an empty output field.
		 */
		OutputField();
		/**
		 * Constructs an output field consisting of braced code.
		 */
		OutputField( const SourceLineList & bracedCode );
		/**
		 * Constructs an output field consisting of a single string.
		 */
		OutputField( const QString &string );
	
		QString string() const { return m_string; }
		SourceLineList bracedCode() const { return m_bracedCode; }
		bool found() const { return m_found; }
	
	private:
		QString m_string;
		SourceLineList m_bracedCode;
		/**
		 * This specifies if a non compulsory field was found or not.
		 */
		bool m_found;
};

typedef QList<Field> StatementDefinition;
typedef QMap<QString,StatementDefinition> DefinitionMap;
typedef QMap<QString,OutputField> OutputFieldMap;


/**
@author Daniel Clarke
@author David Saxton
*/
class Parser
{
	public:
		Parser( Microbe * mb );
		~Parser();
	
		/**
		 * Report a compile error to Microbe; the current source line will be
		 * sent. Context is extra information to be inserted into the error
		 * message, only applicable to some errors (such as a use of a reserved
		 * keyword).
		 */
		void mistake( Microbe::MistakeType type, const QString & context = 0 );
		/**
		 * Creates a new instance of the parser class with all state information
		 * (class members) copied from this instance of the class. Don't forget to
		 * delete it when you are done!
		 */
		Parser * createChildParser();
		/**
		 * Creates a child class and uses it to parse recursively.
		 */
		Code * parseWithChild( const SourceLineList & lines );
		/**
		 * This is the actual parsing function, make sure to use parseUsingChild
		 * instead (???)
		 */
		Code * parse( const SourceLineList & lines );
		/**
		 * Returns the lines between the braces, excluding the braces, e.g.
		 * defproc name
		 * {
		 * more code 
		 * some more code
		 * }
		 * returns ("more code","some more code").
		 * Note that Microbe has already put the braces on separate lines for us.
		 * @param it is the iterator at the position of the first brace, this
		 * function will return with it pointing at the matching closing brace.
		 * @param end is the iterator pointing to the end of the source line
		 * list, so that we don't search past it.
		 * @returns The braced code (excluding the braces).
		 */
		SourceLineList getBracedCode( SourceLineList::const_iterator * it, SourceLineList::const_iterator end );
		/**
		 * Returns expression type.
		 * 0 = directly usable number (literal).
		 * 1 = variable.
		 * 2 = expression that needs evaluating.
		 */
		ExprType getExpressionType( const QString & expression );
		/**
		 * Examines the text to see if it looks like a literal, i.e. of the form
		 * "321890","021348","0x3C","b'0100110'","0101001b","h'43A'", or "2Ah".
		 * Everything else is considered non-literal.
		 * @see literalToInt.
		 */
		static bool isLiteral( const QString &text );
		/**
		 * Tries to convert the given literal string into a integer. If it fails,
		 * i.e. it is not any recognised literal, then it returns -1 and sets *ok to
		 * false. Else, *ok is set to true and the literal value is returned.
		 * @see isLiteral
		 */
		static int literalToInt( const QString & literal, bool * ok = 0l );
		/**
		 * Does the specified operation on the given numbers and returns the result.
		 */
		static int doArithmetic( int lvalue, int rvalue, Expression::Operation op );
		/**
		 * @return whether it was an assignment (which might not have been in
		 * the proper form).
		 */
		bool processAssignment(const QString &line);
	
		void compileConditionalExpression( const QString & expression, Code * ifCode, Code * elseCode ) const;
		QString processConstant(const QString &expression, bool * isConstant, bool suppressNumberTooBig = false) const;
	
	private:
		/**
		 * This is called when the bulk of the actual parsing has been carried
		 * out and is ready to be turned into assembly code.
		 * @param name Name of the statement to be processed
		 * @param fieldMap A map of named fields as appropriate to the statement
		 */
		void processStatement( const QString & name, const OutputFieldMap & fieldMap );

		DefinitionMap m_definitionMap;
		PIC14 * m_pPic;
		bool m_bPassedEnd;
		Microbe * mb;
		Code * m_code;
		SourceLine m_currentSourceLine;
		
	private: // Disable copy constructor and operator=
		Parser( const Parser & );
		Parser &operator=( const Parser & );
};

#endif
