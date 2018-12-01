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
 
#include "btreebase.h"
#include "expression.h"
#include "instruction.h"
#include "parser.h"
#include "pic14.h"
#include "traverser.h"

#include <cassert>
#include <kdebug.h>
#include <klocale.h>
#include <qfile.h>
#include <qregexp.h>
#include <qstring.h>

#include <iostream>
using namespace std;


//BEGIN class Parser
Parser::Parser( Microbe * _mb )
{
	m_code = 0;
	m_pPic = 0;
	mb = _mb;
	// Set up statement definitions.
	StatementDefinition definition;
	
	definition.append( Field(Field::Label, "label") );
	m_definitionMap["goto"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Label, "label") );
	m_definitionMap["call"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Expression, "expression") );
	definition.append( Field(Field::Code, "code") );
	m_definitionMap["while"] = definition;
	definition.clear();
	
	m_definitionMap["end"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Label, "label") );
	definition.append( Field(Field::Code, "code") );
	// For backwards compataibility
	m_definitionMap["sub"] = definition;
	m_definitionMap["subroutine"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Label, "label") );
	definition.append( Field(Field::Code, "code") );
	m_definitionMap["interrupt"] = definition;
	definition.clear();

	definition.append( Field(Field::Label, "alias") );
	definition.append( Field(Field::Label, "dest") );
	m_definitionMap["alias"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Expression, "expression") );
	definition.append( Field(Field::FixedString, 0, "then", true) );
	definition.append( Field(Field::Code, "ifCode") );
	definition.append( Field(Field::Newline) );
	definition.append( Field(Field::FixedString, 0, "else", false) );
	definition.append( Field(Field::Code, "elseCode") );
	m_definitionMap["if"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Expression, "initExpression") );
	definition.append( Field(Field::FixedString, 0, "to", true) );
	definition.append( Field(Field::Expression, "toExpression") );
	definition.append( Field(Field::FixedString, 0, "step", false) );
	definition.append( Field(Field::Expression, "stepExpression") );
	definition.append( Field(Field::Code, "code") );
	m_definitionMap["for"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Variable, "variable") );
	m_definitionMap["decrement"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Variable, "variable") );
	m_definitionMap["increment"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Variable, "variable") );
	m_definitionMap["rotateleft"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Variable, "variable") );
	m_definitionMap["rotateright"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Code, "code") );
	m_definitionMap["asm"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Expression, "expression") );
	m_definitionMap["delay"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Code, "code") );
	definition.append( Field(Field::Newline) );
	definition.append( Field(Field::FixedString, 0, "until", true) );
	definition.append( Field(Field::Expression, "expression") );
	m_definitionMap["repeat"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Name, "name") );
	definition.append( Field(Field::PinList, "pinlist") );
	m_definitionMap["sevenseg"] = definition;
	definition.clear();
	
	definition.append( Field(Field::Name, "name") );
	definition.append( Field(Field::PinList, "pinlist") );
	m_definitionMap["keypad"] = definition;
	definition.clear();
}

Parser::~Parser()
{
}

Parser* Parser::createChildParser()
{
	Parser * parser = new Parser( mb );
	
	return parser;
}


Code * Parser::parseWithChild( const SourceLineList & lines )
{
	Parser * p = createChildParser();
	Code * code = p->parse(lines);
	delete p;
	return code;
}


Code * Parser::parse( const SourceLineList & lines )
{
	StatementList sList;
	m_pPic = mb->makePic();
	m_code = new Code();
	m_pPic->setCode( m_code );
	m_pPic->setParser(this);
	m_bPassedEnd = false;
	
	/* First pass
	   ==========
	   
	   Here we go through the code making each line into a statement object,
	   looking out for braced code as we go, if we find it then we put then
	   we make attach the braced code to the statment.
	*/   

	SourceLineList::const_iterator end = lines.end();
	for ( SourceLineList::const_iterator slit = lines.begin(); slit != end; ++slit )
	{
		Statement s;
		s.content = *slit;
		
		// Check to see if the line after next is a brace
		SourceLineList::const_iterator previous = slit;
		if ( (++slit != end) && (*slit).text() == "{" )
			s.bracedCode = getBracedCode( & slit, end );
		else
			slit = previous;
		
		if ( !s.text().isEmpty() )
			sList.append(s);
	}
	
	mb->resetDest();
	
	for( StatementList::Iterator sit = sList.begin(); sit != sList.end(); ++sit )
	{
		m_currentSourceLine = (*sit).content;
		
		QString line = (*sit).text();
		
		QString command; // e.g. "delay", "for", "subroutine", "increment", etc
		{
			int spacepos = line.indexOf(' ');
			if ( spacepos >= 0 )
				command = line.left( spacepos );
			else
				command = line;
		}
		OutputFieldMap fieldMap;
		
		if ( (*sit).content.line() >= 0 )
			m_code->append( new Instr_sourceCode( QString("#MSRC\t%1; %2\t%3").arg( (*sit).content.line() + 1 ).arg( (*sit).content.url() ).arg( (*sit).content.text() ) ));
		bool showBracesInSource = (*sit).hasBracedCode();
		if ( showBracesInSource )
			m_code->append(new Instr_sourceCode("{"));

		// Use the first token in the line to look up the statement type
		DefinitionMap::Iterator dmit = m_definitionMap.find(command);
		if(dmit == m_definitionMap.end())
		{
			if( !processAssignment( (*sit).text() ) )
			{
				// Not an assignement, maybe a label
				if( (*sit).isLabel() ) 
				{
					QString label = (*sit).text().left( (*sit).text().length() - 1 );
					///TODO sanity check label name and then do error like "Bad label"
					m_pPic->Slabel( label );
				}
				else
					mistake( Microbe::Microbe::UnknownStatement );
			}
			
			continue; // Give up on the current statement
		}
		StatementDefinition definition = dmit.value();
		
		// Start at the first white space character following the statement name
		int newPosition = 0;
		int position = command.length() + 1;
		
		// Temporaries for use inside the switch
		Field nextField;
		Statement nextStatement;
		
		bool errorInLine = false;
		bool finishLine = false;

		for( StatementDefinition::Iterator sdit = definition.begin(); sdit != definition.end(); ++sdit )
		{
			// If there is an error, or we have finished the statement,
			// the stop. If we are at the end of a line in a multiline, then
			// break to fall through to the next line
			if( errorInLine || finishLine) break;
		
			Field field = (*sdit);
			QString token;
			
			bool saveToken = false;
			bool saveBraced = false;
			bool saveSingleLine = false;
			
			switch(field.type())
			{	
				case (Field::Label):
				case (Field::Variable):
				case (Field::Name):
				{
					newPosition = line.indexOf( ' ', position );
					if(position == newPosition)
					{
						newPosition = -1;
						token = line.mid(position);
					}
					else token = line.mid(position, newPosition - position);
					if( token.isEmpty() )
					{
						if(field.type() == Field::Label)
							mistake( Microbe::Microbe::LabelExpected );
						else if (field.type() == Field::Variable)
							mistake( Microbe::VariableExpected );
						else // field.type() == Field::Name
							mistake( Microbe::NameExpected );
						errorInLine = true;
						continue;
					}
					position = newPosition;
					saveToken = true;
					break;
				}
					
				case (Field::Expression):
				{
					// This is slightly different, as there is nothing
					// in particular that delimits an expression, we just have to
					// look at what comes next and hope we can use that.
					StatementDefinition::Iterator it(sdit);
					++it;
					if( it != definition.end() )
					{
						nextField = (*it);
						if(nextField.type() == Field::FixedString) 
							newPosition = line.indexOf(QRegExp("\\b" + nextField.string() + "\\b"));
						// Although code is not neccessarily braced, after an expression it is the only
						// sensilbe way to have it.
						else if(nextField.type() == Field::Code)
						{
							newPosition = line.indexOf("{");
							if(newPosition == -1) newPosition = line.length() + 1;
						}
						else if(nextField.type() == Field::Newline)
							newPosition = line.length()+1;
						else kDebug() << "Bad statement definition - awkward field type after expression";
					}
					else newPosition = line.length() + 1;
					if(newPosition == -1)
					{
						// Something was missing, we'll just play along for now,
						// the next iteration will catch whatever was supposed to be there
					}
					token = line.mid(position, newPosition - position);
					position = newPosition;
					saveToken = true;
				}
				break;
					
				case (Field::PinList):
				{
					// For now, just assume that the list of pins will continue to the end of the tokens.
					// (we could check until we come across a non-pin, but no command has that format at
					// the moment).
					
					token = line.mid( position + 1 );
					position = line.length() + 1;
					if ( token.isEmpty() )
						mistake( Microbe::PinListExpected );
					else
						saveToken = true;
					
					break;
				}
					
				case (Field::Code):
					if ( !(*sit).hasBracedCode() )
					{
						saveSingleLine = true;
						token = line.mid(position);
						position = line.length() + 1;
					}
					else if( position != -1  && position <= int(line.length()) )
					{
						mistake( Microbe::UnexpectedStatementBeforeBracket );
						errorInLine = true;
						continue;
					}
					else					
					{
						// Because of the way the superstructure parsing works there is no
						// 'next line' as it were, the braced code is attached to the current line.
						saveBraced = true;
					}
					break;
					
				case (Field::FixedString):
				{
					// Is the string found, and is it starting in the right place?
					int stringPosition  = line.indexOf(QRegExp("\\b"+field.string()+"\\b"));
					if( stringPosition != position || stringPosition == -1 )
					{
						if( !field.compulsory() )
						{
							position = -1;
							// Skip the next field
							++sdit;
							continue;
						}
						else
						{
							// Otherwise raise an appropriate error
							mistake( Microbe::FixedStringExpected, field.string() );
							errorInLine = true;
							continue;
						}
					}
					else
					{
						position += field.string().length() + 1;
					}
				}
					break;
					
				case (Field::Newline):
					// It looks like the best way to handle this is to just actually
					// look at the next line, and see if it begins with an expected fixed
					// string.
					
					// Assume there is a next field, it would be silly if there weren't.
					nextField = *(++StatementDefinition::Iterator(sdit));
					if( nextField.type() == Field::FixedString )
					{
						nextStatement = *(++StatementList::Iterator(sit));
						newPosition = nextStatement.text().indexOf(QRegExp("\\b" + nextField.string() + "\\b"));
						if(newPosition != 0)
						{
							// If the next field is optional just carry on as nothing happened,
							// the next line will be processed as a new statement
							if(!nextField.compulsory()) continue;
							
						}
						position = 0;
						line = (*(++sit)).text();
						m_currentSourceLine = (*sit).content;
					}
					
					break;
					
				case (Field::None):
					// Do nothing
					break;
			}
			
			if ( saveToken )
				fieldMap[field.key()] = OutputField( token );
			
			if ( saveSingleLine )
			{
				SourceLineList list;
				list << SourceLine( token, 0, -1 );
				fieldMap[field.key()] = OutputField( list );
			}
			
			if ( saveBraced )
				fieldMap[field.key()] = OutputField( (*sit).bracedCode );
			// If position = -1, we have reached the end of the line.
		}
		
		// See if we got to the end of the line, but not all fields had been
		// processed.
		if( position != -1  && position <= int(line.length()) )
		{
			mistake( Microbe::TooManyTokens );
			errorInLine = true;
		}
		
		if( errorInLine ) continue;
			
		// Everything has been parsed up, so send it off for processing.
		processStatement( command, fieldMap );

		if( showBracesInSource )
			m_code->append(new Instr_sourceCode("}"));
	}
	
	delete m_pPic;
	return m_code;
}

bool Parser::processAssignment(const QString &line)
{
	QStringList tokens = Statement::tokenise(line);
	
	// Have to have at least 3 tokens for an assignment;
	if ( tokens.size() < 3 )
		return false;
	
	QString firstToken = tokens[0];

	firstToken = mb->alias(firstToken);
	// Well firstly we look to see if it is a known variable. 
	// These can include 'special' variables such as ports
	// For now, the processor subclass generates ports it self
	// and puts them in a list for us.
	

	// Look for port variables first.
	if ( firstToken.contains(".") )
	{
		PortPin portPin = m_pPic->toPortPin( firstToken );
		
		// check port is valid
		if ( portPin.pin() == -1 )
			mistake( Microbe::InvalidPort, firstToken );
		// more error checking
		if ( tokens[1] != "=" )
			mistake( Microbe::UnassignedPin );
		
		QString state = tokens[2];
		if( state == "high" )
			m_pPic->Ssetlh( portPin, true );
		else if( state == "low" )
			m_pPic->Ssetlh( portPin, false );
		else
			mistake( Microbe::NonHighLowPinState );
	}
	// no dots, lets try for just a port name
	else if( m_pPic->isValidPort( firstToken ) )
	{
		// error checking
		if ( tokens[1] != "=" )
			mistake( Microbe::UnassignedPort, tokens[1] );
		
		Expression( m_pPic, mb, m_currentSourceLine, false ).compileExpression(line.mid(line.indexOf("=")+1));
		m_pPic->saveResultToVar( firstToken );
	}
	else if ( m_pPic->isValidTris( firstToken ) )
	{
		if( tokens[1] == "=" )
		{
			Expression( m_pPic, mb, m_currentSourceLine, false ).compileExpression(line.mid(line.indexOf("=")+1));
			m_pPic->Stristate(firstToken);
		}
	}
	else
	{
		// Is there an assignment?
		if ( tokens[1] != "=" )
			return false;
		
		if ( !mb->isValidVariableName( firstToken ) )
		{
			mistake( Microbe::InvalidVariableName, firstToken );
			return true;
		}
		
		// Don't care whether or not the variable is new; Microbe will only add it if it
		// hasn't been defined yet.
		mb->addVariable( Variable( Variable::charType, firstToken ) );
		
		Expression( m_pPic, mb, m_currentSourceLine, false ).compileExpression(line.mid(line.indexOf("=")+1));
		
		Variable v = mb->variable( firstToken );
		switch ( v.type() )
		{
			case Variable::charType:
				m_pPic->saveResultToVar( v.name() );
				break;
				
			case Variable::keypadType:
				mistake( Microbe::ReadOnlyVariable, v.name() );
				break;
				
			case Variable::sevenSegmentType:
				m_pPic->SsevenSegment( v );
				break;
				
			case Variable::invalidType:
				// Doesn't happen, but include this case to avoid compiler warnings
				break;
		}
	}
	
	return true;
}


SourceLineList Parser::getBracedCode( SourceLineList::const_iterator * it, SourceLineList::const_iterator end )
{
	// Note: The sourceline list has the braces on separate lines.
	
	// This function should only be called when the parser comes across a line that is a brace.
	assert( (**it).text() == "{" );
	
	SourceLineList braced;
	
	// Jump past the first brace
	unsigned level = 1;
	++(*it);
	
	for ( ; *it != end; ++(*it) )
	{
		if ( (**it).text() == "{" )
			level++;
		
		else if ( (**it).text() == "}" )
			level--;
		
		if ( level == 0 )
			return braced;
		
		braced << **it;
	}
	
	// TODO Error: mismatched bracing
	return braced;
}


void Parser::processStatement( const QString & name, const OutputFieldMap & fieldMap )
{
	// Name is guaranteed to be something known, the calling
	// code has taken care of that. Also fieldMap is guaranteed to contain
	// all required fields.

	if ( name == "goto" )
		m_pPic->Sgoto(fieldMap["label"].string());
	
	else if ( name == "call" )
		m_pPic->Scall(fieldMap["label"].string());
	
	else if ( name == "while" )
		m_pPic->Swhile( parseWithChild(fieldMap["code"].bracedCode() ), fieldMap["expression"].string() );
	
	else if ( name == "repeat" )
		m_pPic->Srepeat( parseWithChild(fieldMap["code"].bracedCode() ), fieldMap["expression"].string() );
	
	else if ( name == "if" )
		m_pPic->Sif(
				parseWithChild(fieldMap["ifCode"].bracedCode() ),
				parseWithChild(fieldMap["elseCode"].bracedCode() ),
				fieldMap["expression"].string() );
	
	else if ( name == "sub" || name == "subroutine" )
	{	
		if(!m_bPassedEnd)
		{
			mistake( Microbe::InterruptBeforeEnd );
		}
		else
		{
			m_pPic->Ssubroutine( fieldMap["label"].string(), parseWithChild( fieldMap["code"].bracedCode() ) );
		}
	}
	else if( name == "interrupt" )
	{	
		QString interrupt = fieldMap["label"].string();
		
		if(!m_bPassedEnd)
		{
			mistake( Microbe::InterruptBeforeEnd );
		}
		else if( !m_pPic->isValidInterrupt( interrupt ) )
		{
			mistake( Microbe::InvalidInterrupt );
		}
		else if ( mb->isInterruptUsed( interrupt ) )
		{
			mistake( Microbe::InterruptRedefined );
		}
		else
		{
			mb->setInterruptUsed( interrupt );
			m_pPic->Sinterrupt( interrupt, parseWithChild( fieldMap["code"].bracedCode() ) );
		}
	}
	else if( name == "end" )
	{
		///TODO handle end if we are not in the top level
		m_bPassedEnd = true;
		m_pPic->Send();
	}
	else if( name == "for" )
	{
		QString step = fieldMap["stepExpression"].string();
		bool stepPositive;
		
		if( fieldMap["stepExpression"].found() )
		{
			if(step.left(1) == "+")
			{
				stepPositive = true;
				step = step.mid(1).trimmed();
			}
			else if(step.left(1) == "-")
			{
				stepPositive = false;
				step = step.mid(1).trimmed();
			}
			else stepPositive = true;
		}
		else
		{
			step = "1";
			stepPositive = true;
		}
		
		QString variable = fieldMap["initExpression"].string().mid(0,fieldMap["initExpression"].string().indexOf("=")).trimmed();
		QString endExpr = variable+ " <= " + fieldMap["toExpression"].string().trimmed();
		
		if( fieldMap["stepExpression"].found() )
		{	
			bool isConstant;
			step = processConstant(step,&isConstant);
			if( !isConstant )
				mistake( Microbe::NonConstantStep );
		}
		
		SourceLineList tempList;
		tempList << SourceLine( fieldMap["initExpression"].string(), 0, -1 );
		
		m_pPic->Sfor( parseWithChild( fieldMap["code"].bracedCode() ), parseWithChild( tempList ), endExpr, variable, step, stepPositive );
	}
	else if( name == "alias" )
	{
		// It is important to get this the right way round!
		// The alias should be the key since two aliases could
		// point to the same name.
	
		QString alias = fieldMap["alias"].string().trimmed();
		QString dest = fieldMap["dest"].string().trimmed();
		
		// Check to see whether or not we've already aliased it...
// 		if ( mb->alias(alias) != alias )
// 			mistake( Microbe::AliasRedefined );
// 		else
			mb->addAlias( alias, dest );
	}
	else if( name == "increment" )
	{
		QString variableName = fieldMap["variable"].string();
		
		if ( !mb->isVariableKnown( variableName ) )
			mistake( Microbe::UnknownVariable );
		else if ( !mb->variable( variableName ).isWritable() )
			mistake( Microbe::ReadOnlyVariable, variableName );
		else
			m_pPic->SincVar( variableName );
	}
	else if( name == "decrement" )
	{
		QString variableName = fieldMap["variable"].string();
		
		if ( !mb->isVariableKnown( variableName ) )
			mistake( Microbe::UnknownVariable );
		else if ( !mb->variable( variableName ).isWritable() )
			mistake( Microbe::ReadOnlyVariable, variableName );
		else
			m_pPic->SdecVar( variableName );
	}
	else if( name == "rotateleft" )
	{
		QString variableName = fieldMap["variable"].string();
		
		if ( !mb->isVariableKnown( variableName ) )
			mistake( Microbe::UnknownVariable );
		else if ( !mb->variable( variableName ).isWritable() )
			mistake( Microbe::ReadOnlyVariable, variableName );
		else
			m_pPic->SrotlVar( variableName );
	}
	else if( name == "rotateright" )
	{
		QString variableName = fieldMap["variable"].string();
		
		if ( !mb->isVariableKnown( variableName ) )
			mistake( Microbe::UnknownVariable );
		else if ( !mb->variable( variableName ).isWritable() )
			mistake( Microbe::ReadOnlyVariable, variableName );
		else
			m_pPic->SrotrVar( variableName );
	}
	else if( name == "asm" )
	{	
		m_pPic->Sasm( SourceLine::toStringList( fieldMap["code"].bracedCode() ).join("\n") );
	}
	else if( name == "delay" )
	{
		// This is one of the rare occasions that the number will be bigger than a byte,
		// so suppressNumberTooBig must be used
		bool isConstant;
		QString delay = processConstant(fieldMap["expression"].string(),&isConstant,true);
		if (!isConstant)
			mistake( Microbe::NonConstantDelay );
// 		else m_pPic->Sdelay( fieldMap["expression"].string(), "");
		else
		{
			// TODO We should use the "delay" string returned by processConstant - not the expression (as, e.g. 2*3 won't be ok)
			int length_ms = literalToInt( fieldMap["expression"].string() );
			if ( length_ms >= 0 )
				m_pPic->Sdelay( length_ms * 1000 ); // Pause the delay length in microseconds
			else
				mistake( Microbe::NonConstantDelay );
		}
	}
	else if ( name == "keypad" || name == "sevenseg" )
	{
		//QStringList pins = QStringList::split( ' ', fieldMap["pinlist"].string() );
        QStringList pins = fieldMap["pinlist"].string().split(' ');
		QString variableName = fieldMap["name"].string();
		
		if ( mb->isVariableKnown( variableName ) )
		{
			mistake( Microbe::VariableRedefined, variableName );
			return;
		}
		
		PortPinList pinList;
		
		QStringList::iterator end = pins.end();
		for ( QStringList::iterator it = pins.begin(); it != end; ++it )
		{
			PortPin portPin = m_pPic->toPortPin(*it);
			if ( portPin.pin() == -1 )
			{
				// Invalid port/pin
				//TODO mistake
				return;
			}
			pinList << portPin;
		}
		
		if ( name == "keypad" )
		{
			Variable v( Variable::keypadType, variableName );
			v.setPortPinList( pinList );
			mb->addVariable( v );
		}
		
		else // name == "sevenseg"
		{
			if ( pinList.size() != 7 )
				mistake( Microbe::InvalidPinMapSize );
			else
			{
				Variable v( Variable::sevenSegmentType, variableName );
				v.setPortPinList( pinList );
				mb->addVariable( v );
			}
		}
	}
}


void Parser::mistake( Microbe::MistakeType type, const QString & context )
{
	mb->compileError( type, context, m_currentSourceLine );
}


// static function
QStringList Statement::tokenise(const QString &line)
{
	QStringList result;
	QString current;
	int count = 0;
	
	for(int i = 0; i < int(line.length()); i++)
	{
		QChar nextChar = line[i];
		if( nextChar.isSpace() )
		{
			if( count > 0 )
			{
				result.append(current);
				current = "";
				count = 0;
			}
		}
		else if( nextChar == '=' )
		{
			if( count > 0 ) result.append(current);
			current = "";
			count = 0;
			result.append("=");
		}
		else if( nextChar == '{' )
		{
			if( count > 0 ) result.append(current);
			current = "";
			count = 0;
			result.append("{");
		}
		else
		{	
			count++;
			current.append(nextChar);
		}
	}
	if( count > 0 ) result.append(current);
	return result;
}

int Parser::doArithmetic(int lvalue, int rvalue, Expression::Operation op)
{
	switch(op)
	{
		case Expression::noop: return 0;
		case Expression::addition: return lvalue + rvalue;
		case Expression::subtraction: return lvalue - rvalue;
		case Expression::multiplication: return lvalue * rvalue;
		case Expression::division: return lvalue / rvalue;
		case Expression::exponent: return lvalue ^ rvalue;
		case Expression::equals: return lvalue == rvalue;
		case Expression::notequals: return !(lvalue == rvalue);
		case Expression::bwand: return lvalue & rvalue;
		case Expression::bwor: return lvalue | rvalue;
		case Expression::bwxor: return lvalue ^ rvalue;
		case Expression::bwnot: return !rvalue;
		case Expression::le: return lvalue <= rvalue;
		case Expression::ge: return lvalue >= rvalue;
		case Expression::lt: return lvalue < rvalue;
		case Expression::gt: return lvalue > rvalue;
		
		case Expression::pin:
		case Expression::notpin:
		case Expression::function:
		case Expression::divbyzero:
		case Expression::read_keypad:
			// Not applicable actions.
			break;
	}
	return -1;
}

bool Parser::isLiteral( const QString &text )
{
	bool ok;
	literalToInt( text, & ok );
	return ok;
}

/*
Literal's in form:
-> 321890
-> 021348
-> 0x3C
-> b'0100110'
-> 0101001b
-> h'43A'
-> 2Ah

Everything else is non-literal...
*/
int Parser::literalToInt( const QString &literal, bool * ok )
{
	bool temp;
	if ( !ok )
		ok = & temp;
	*ok = true;
	
	int value = -1;
	
	// Note when we use toInt, we don't have to worry about checking 
	// that literal.mid() is convertible, as toInt returns this in ok anyway.
	
	// Try binary first, of form b'n...n'
	if( literal.left(2) == "b'" && literal.right(1) == "'" )
	{
		value = literal.mid(2,literal.length() - 3).toInt(ok,2);
		return *ok ? value : -1;
	}
		
	// Then try hex of form h'n...n'
	if( literal.left(2) == "h'" && literal.right(1) == "'" )
	{
		value = literal.mid(2,literal.length() - 3).toInt(ok,16);
		return *ok ? value : -1;
	}
	
	// otherwise, let QString try and convert it
	// base 0 == automatic base guessing
	value = literal.toInt( ok, 0 );
	return *ok ? value : -1;
}


void Parser::compileConditionalExpression( const QString & expression, Code * ifCode, Code * elseCode ) const
{
	///HACK ///TODO this is a little improper, I don't think we should be using the pic that called us...

	Expression( m_pPic, mb, m_currentSourceLine, false ).compileConditional(expression,ifCode,elseCode);
}


QString Parser::processConstant(const QString &expression, bool * isConstant, bool suppressNumberTooBig) const
{
	return Expression( m_pPic, mb, m_currentSourceLine, suppressNumberTooBig ).processConstant(expression, isConstant);
}
//END class Parser



//BEGIN class Field
Field::Field()
{
	m_type = None;
	m_compulsory = false;
}


Field::Field( Type type, const QString & key )
{
	m_type = type;
	m_compulsory = false;
	m_key = key;
}


Field::Field( Type type, const QString & key, const QString & string, bool compulsory )
{
	m_type = type;
	m_compulsory = compulsory;
	m_key = key;
	m_string = string;
}
//END class Field



//BEGIN class OutputField
OutputField::OutputField()
{
	m_found = false;
}


OutputField::OutputField( const SourceLineList & bracedCode )
{
	m_bracedCode = bracedCode;
	m_found = true;
}

OutputField::OutputField( const QString & string/*, int lineNumber*/ )
{
	m_string = string;
	m_found = true;
}
//END class OutputField



#if 0
// Second pass

		else if( firstToken == "include" )
		{
			// only cope with 'sane' strings a.t.m.
			// e.g. include "filename.extenstion"
			QString filename = (*sit).content.mid( (*sit).content.find("\"") ).trimmed();
			// don't strip whitespace from within quotes as you
			// can have filenames composed entirely of spaces (kind of weird)...
			// remove quotes.
			filename = filename.mid(1); 
			filename = filename.mid(0,filename.length()-1);
			QFile includeFile(filename);
			if( includeFile.open(QIODevice::ReadOnly) )
			{
				QTextStream stream( &includeFile );
        			QStringList includeCode;
				while( !stream.atEnd() )
				{
					includeCode += stream.readLine();
				}
				///TODO make includes work
				//output += parse(includeCode);
				includeFile.close();
    			}
    			else
    			mistake( Microbe::UnopenableInclude, filename );
 		}		
#endif


