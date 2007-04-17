/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke <daniel.jc@gmail.com>        *
 *                      2005 by David Saxton <david@bluehaze.org>          *
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
#include "optimizer.h"
#include "pic14.h"

#include <kdebug.h>
#include <qstringlist.h>

#include <assert.h>
#include <iostream>
using namespace std;


//BEGIN class Register
Register::Register( Type type )
{
	m_type = type;
	
	switch ( m_type )
	{
		case TMR0:
			m_name = "TMR0";
			break;
		case OPTION_REG:
			m_name = "OPTION_REG";
			break;
		case PCL:
			m_name = "PCL";
			break;
		case STATUS:
			m_name = "STATUS";
			break;
		case FSR:
			m_name = "FSR";
			break;
		case PORTA:
			m_name = "PORTA";
			break;
		case TRISA:
			m_name = "TRISA";
			break;
		case PORTB:
			m_name = "PORTB";
			break;
		case TRISB:
			m_name = "TRISB";
			break;
		case EEDATA:
			m_name = "EEDATA";
			break;
		case EECON1:
			m_name = "EECON1";
			break;
		case EEADR:
			m_name = "EEADR";
			break;
		case EECON2:
			m_name = "EECON2";
			break;
		case PCLATH:
			m_name = "PCLATH";
			break;
		case INTCON:
			m_name = "INTCON";
			break;
		case WORKING:
			m_name = "<working>";
			break;
		case GPR:
		case none:
			break;
	}
}


Register::Register( const QString & name )
{
	m_name = name.stripWhiteSpace();
	QString upper = m_name.upper();
	
	if ( upper == "TMR0" )
		m_type = TMR0;
	else if ( upper == "OPTION_REG" )
		m_type = OPTION_REG;
	else if ( upper == "PCL" )
		m_type = PCL;
	else if ( upper == "STATUS")
		m_type = STATUS;
	else if ( upper == "FSR")
		m_type = FSR;
	else if ( upper == "PORTA")
		m_type = PORTA;
	else if ( upper == "TRISA")
		m_type = TRISA;
	else if ( upper == "PORTB")
		m_type = PORTB;
	else if ( upper == "TRISB")
		m_type = TRISB;
	else if ( upper == "EEDATA")
		m_type = EEDATA;
	else if ( upper == "EECON1")
		m_type = EECON1;
	else if ( upper == "EEADR")
		m_type = EEADR;
	else if ( upper == "EECON2")
		m_type = EECON2;
	else if ( upper == "PCLATH")
		m_type = PCLATH;
	else if ( upper == "INTCON")
		m_type = INTCON;
	else
		m_type = GPR;
}


Register::Register( const char * name )
{
	*this = Register( QString(name) );
}


bool Register::operator < ( const Register & reg ) const
{
	if ( (type() != GPR) || (reg.type() != GPR) )
		return type() < reg.type();
	
	return name() < reg.name();
}


bool Register::operator == ( const Register & reg ) const
{
	if ( type() != reg.type() )
		return false;
	
	return name() == reg.name();
}


uchar Register::banks() const
{
	switch ( m_type )
	{
		case TMR0: return Bank0;
		case OPTION_REG: return Bank1;
		case PCL: return Bank0 | Bank1;
		case STATUS: return Bank0 | Bank1;
		case FSR: return Bank0 | Bank1;
		case PORTA: return Bank0;
		case TRISA: return Bank1;
		case PORTB: return Bank0;
		case TRISB: return Bank1;
		case EEDATA: return Bank0;
		case EECON1: return Bank1;
		case EEADR: return Bank0;
		case EECON2: return Bank1;
		case PCLATH: return Bank0 | Bank1;
		case INTCON: return Bank0 | Bank1;
		
		case GPR: return Bank0 | Bank1;
		case WORKING: return Bank0 | Bank1;
		case none: return Bank0 | Bank1;
	}
	
	return Bank0 | Bank1; // Vacously true (and useful too) - a non-existent bank can be accessed anywhere
}


bool Register::bankDependent() const
{
	return ( banks() != (Bank0 | Bank1) );
}


bool Register::affectsExternal() const
{
	switch ( m_type )
	{
		case PORTA:
		case TRISA:
		case PORTB:
		case TRISB:
			return true;
			
		case TMR0:
		case OPTION_REG:
		case PCL:
		case STATUS:
		case FSR:
		case EEDATA:
		case EECON1:
		case EEADR:
		case EECON2:
		case PCLATH:
		case INTCON:
		case GPR:
		case WORKING:
		case none:
			return false;
	}
	return false;
}
//END class Register



//BEGIN class RegisterBit
RegisterBit::RegisterBit( uchar bitPos, Register::Type reg )
{
	m_bitPos = bitPos;
	m_registerType = reg;
	
	switch ( m_registerType )
	{
		case Register::STATUS:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "C"; break;
				case 1: m_name = "DC"; break;
				case 2: m_name = "Z"; break;
				case 3: m_name = "NOT_PD"; break;
				case 4: m_name = "NOT_TO"; break;
				case 5: m_name = "RP0"; break;
				case 6: m_name = "RP1"; break;
				case 7: m_name = "IRP"; break;
			}
			break;
		}
		case Register::INTCON:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "RBIF"; break;
				case 1: m_name = "INTF"; break;
				case 2: m_name = "T0IF"; break;
				case 3: m_name = "RBIE"; break;
				case 4: m_name = "INTE"; break;
				case 5: m_name = "T0IE"; break;
				case 6: m_name = "EEIE"; break;
				case 7: m_name = "GIE"; break;
			}
			break;
		}
		case Register::OPTION_REG:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "PS0"; break;
				case 1: m_name = "PS1"; break;
				case 2: m_name = "PS2"; break;
				case 3: m_name = "PSa"; break;
				case 4: m_name = "T0SE"; break;
				case 5: m_name = "T0CS"; break;
				case 6: m_name = "INTEDG"; break;
				case 7: m_name = "NOT_RBPU"; break;
			}
			break;
		}
		case Register::EECON1:
		{
			switch ( m_bitPos )
			{
				case 0: m_name = "RD"; break;
				case 1: m_name = "WR"; break;
				case 2: m_name = "WREN"; break;
				case 3: m_name = "WRERR"; break;
				case 4: m_name = "EEIF"; break;
			}
			break;
		}
			
		case Register::TMR0:
		case Register::PCL:
		case Register::FSR:
		case Register::PORTA:
		case Register::TRISA:
		case Register::PORTB:
		case Register::TRISB:
		case Register::EEDATA:
		case Register::EEADR:
		case Register::EECON2:
		case Register::PCLATH:
		case Register::GPR:
		case Register::WORKING:
		case Register::none:
		{
// 			kdError() << k_funcinfo << "Bad register: " << reg << endl;
		}
	}
}


RegisterBit::RegisterBit( const QString & name )
{
	m_name = name.upper().stripWhiteSpace();
	initFromName();
}


RegisterBit::RegisterBit( const char * name )
{
	m_name = QString(name).upper().stripWhiteSpace();
	initFromName();
}


void RegisterBit::initFromName()
{
	bool ok;
	m_bitPos = m_name.toInt( & ok, 0 );
	if ( ok )
		m_registerType = Register::none; // hmm it should be unknown - not none.
	
	else if ( m_name == "C" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 0;
	}
	else if ( m_name == "DC" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 1;
	}
	else if ( m_name == "Z" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 2;
	}
	else if ( m_name == "NOT_PD" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 3;
	}
	else if ( m_name == "NOT_TO" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 4;
	}
	else if ( m_name == "RP0" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 5;
	}
	else if ( m_name == "RP1" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 6;
	}
	else if ( m_name == "IRP" )
	{
		m_registerType = Register::STATUS;
		m_bitPos = 7;
	}
	else if ( m_name == "RBIF" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 0;
	}
	else if ( m_name == "INTF" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 1;
	}
	else if ( m_name == "T0IF" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 2;
	}
	else if ( m_name == "RBIE" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 3;
	}
	else if ( m_name == "INTE" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 4;
	}
	else if ( m_name == "T0IE" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 5;
	}
	else if ( m_name == "EEIE" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 6;
	}
	else if ( m_name == "GIE" )
	{
		m_registerType = Register::INTCON;
		m_bitPos = 7;
	}
	else if ( m_name == "PS0" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 0;
	}
	else if ( m_name == "PS1" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 1;
	}
	else if ( m_name == "PS2" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 2;
	}
	else if ( m_name == "PSA" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 3;
	}
	else if ( m_name == "T0SE" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 4;
	}
	else if ( m_name == "T0CS" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 5;
	}
	else if ( m_name == "INTEDG" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 6;
	}
	else if ( m_name == "NOT_RBPU" )
	{
		m_registerType = Register::OPTION_REG;
		m_bitPos = 7;
	}
	else if ( m_name == "RD" )
	{
		m_registerType = Register::EECON1;
		m_bitPos = 0;
	}
	else if ( m_name == "WR" )
	{
		m_registerType = Register::EECON1;
		m_bitPos = 1;
	}
	else if ( m_name == "WREN" )
	{
		m_registerType = Register::EECON1;
		m_bitPos = 2;
	}
	else if ( m_name == "WRERR" )
	{
		m_registerType = Register::EECON1;
		m_bitPos = 3;
	}
	else if ( m_name == "EEIF" )
	{
		m_registerType = Register::EECON1;
		m_bitPos = 4;
	}
	else
	{
		m_registerType = Register::none;
		m_bitPos = 0;
		kdError() << k_funcinfo << "Unknown bit: " << m_name << endl;
	}
}
//END class RegisterBit




//BEGIN class RegisterState
RegisterState::RegisterState()
{
	reset();
}


void RegisterState::reset()
{
	known = 0x0;
	value = 0x0;
}


void RegisterState::merge( const RegisterState & state )
{
	known &= state.known;
	known &= ~( value ^ state.value );
}


bool RegisterState::operator == ( const RegisterState & state ) const
{
	return (known == state.known) && (value == state.value);
}


void RegisterState::print()
{
	cout << "   known="<<binary(known)<<endl;
	cout << "   value="<<binary(value)<<endl;
}
//END class RegisterState



//BEGIN class RegisterBehaviour
RegisterBehaviour::RegisterBehaviour()
{
	reset();
}


void RegisterBehaviour::reset()
{
	depends = 0x0;
	indep = 0x0;
}
//END class RegisterBehaviour



//BEGIN class ProcessorState
ProcessorState::ProcessorState()
{
}


void ProcessorState::reset()
{
	working.reset();
	status.reset();
	
	RegisterMap::iterator end = m_registers.end();
	for ( RegisterMap::iterator it = m_registers.begin(); it != end; ++it )
		(*it).reset();
}


void ProcessorState::merge( const ProcessorState & state )
{
	working.merge( state.working );
	status.merge( state.status );
	
	RegisterMap::iterator this_it = m_registers.begin();
	RegisterMap::const_iterator other_it = state.m_registers.begin();
	
	RegisterMap::iterator this_end = m_registers.end();
	RegisterMap::const_iterator other_end = state.m_registers.end();
	
	while ( true )
	{
		if ( this_it == this_end )
		{
			// So remaining registers of this are default
			while ( other_it != other_end )
			{
				m_registers[ other_it.key() ].merge( *other_it );
				++other_it;
			}
			return;
		}
		
		if ( other_it == other_end )
		{
			// So remaining registers of other are default
			while ( this_it != this_end )
			{
				(*this_it).merge( RegisterState() );
				++this_it;
			}
			return;
		}
		
		RegisterState thisReg = *this_it;
		RegisterState otherReg = *other_it;
		
		if ( this_it.key() == other_it.key() )	
		{
			(*this_it).merge( *other_it );
			++this_it;
			++other_it;
		}
		else if ( this_it.key() < other_it.key() )
		{
			(*this_it).merge( RegisterState() );
			++this_it;
		}
		else // other_it.key() < this_it.key()
		{
			m_registers[ other_it.key() ].merge( *other_it );
			++other_it;
		}
	}
}


RegisterState & ProcessorState::reg( const Register & reg )
{
	if ( reg.type() == Register::WORKING )
		return working;
	
	if ( reg.type() == Register::STATUS )
		return status;
	
	return m_registers[ reg ];
}


RegisterState ProcessorState::reg( const Register & reg ) const
{
	if ( reg.type() == Register::WORKING )
		return working;
	
	if ( reg.type() == Register::STATUS )
		return status;
	
	return m_registers[ reg ];
}


bool ProcessorState::operator == ( const ProcessorState & state ) const
{
	if ( working != state.working )
		return false;
	
	if ( status != state.status )
		return false;
	
	RegisterMap::const_iterator this_it = m_registers.begin();
	RegisterMap::const_iterator other_it = state.m_registers.begin();
	
	RegisterMap::const_iterator this_end = m_registers.end();
	RegisterMap::const_iterator other_end = state.m_registers.end();
	
	while ( true )
	{
		if ( this_it == this_end )
		{
			// So remaining registers of this are default
			while ( other_it != other_end )
			{
				if ( *other_it != RegisterState() )
					return false;
				++other_it;
			}
			return true;
		}
		
		if ( other_it == other_end )
		{
			// So remaining registers of other are default
			while ( this_it != this_end )
			{
				if ( *this_it != RegisterState() )
					return false;
				++this_it;
			}
			return true;
		}
		
		RegisterState thisReg = *this_it;
		RegisterState otherReg = *other_it;
		
		if ( this_it.key() == other_it.key() )	
		{
			if ( *this_it != *other_it )
				return false;
			++this_it;
			++other_it;
		}
		else if ( this_it.key() < other_it.key() )
		{
			if ( *this_it != RegisterState() )
				return false;
			++this_it;
		}
		else // other_it.key() < this_it.key()
		{
			if ( *other_it != RegisterState() )
				return false;
			++other_it;
		}
	}
}


void ProcessorState::print()
{
	cout << " WORKING:\n";
	working.print();
	cout << " STATUS:\n";
	working.print();
	RegisterMap::iterator end = m_registers.end();
	for ( RegisterMap::iterator it = m_registers.begin(); it != end; ++it )
	{
		cout << " " << it.key().name() << ":\n";
		it.data().print();
	}
}
//END class ProcessorState



//BEGIN class ProcessorBehaviour
ProcessorBehaviour::ProcessorBehaviour()
{
}


void ProcessorBehaviour::reset()
{
	working.reset();
	status.reset();
	
	RegisterMap::iterator end = m_registers.end();
	for ( RegisterMap::iterator it = m_registers.begin(); it != end; ++it )
		(*it).reset();
}


RegisterBehaviour & ProcessorBehaviour::reg( const Register & reg )
{
	if ( reg.type() == Register::WORKING )
		return working;
	
	if ( reg.type() == Register::STATUS )
		return status;
	
	return m_registers[ reg ];
}
//END class ProcessorBehaviour



//BEGIN class RegisterDepends
RegisterDepends::RegisterDepends()
{
	reset();
}


void RegisterDepends::reset()
{
	working = 0x0;
	status = 0x0;
	
	RegisterMap::iterator end = m_registers.end();
	for ( RegisterMap::iterator it = m_registers.begin(); it != end; ++it )
		(*it) = 0x0;
}


uchar & RegisterDepends::reg( const Register & reg )
{
	if ( reg.type() == Register::WORKING )
		return working;
	
	if ( reg.type() == Register::STATUS )
		return status;
	
	// If we don't already have the register, we need to reset it first
	if ( !m_registers.contains( reg.name() ) )
		m_registers[ reg ] = 0xff;
	
	return m_registers[ reg ];
}
//END class RegisterDepends



//BEGIN clas Code
Code::Code()
{
}


void Code::merge( Code * code, InstructionPosition middleInsertionPosition )
{
	if ( code == this )
	{
		cout << k_funcinfo << "identical\n";
		return;
	}
	
	if ( !code )
		return;
	
	// Reparent instructions
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		InstructionList * list = code->instructionList( (InstructionPosition)i );
		InstructionList::const_iterator end = list->end();
		for ( InstructionList::const_iterator it = list->begin(); it != end; ++it )
			append( *it, ( (i == Middle) ? middleInsertionPosition : (InstructionPosition)i ) );
		
		// Queue any labels that the other code has queued
		m_queuedLabels[i] += code->queuedLabels( (InstructionPosition)i );
	}
}


void Code::queueLabel( const QString & label, InstructionPosition position )
{
// 	cout << k_funcinfo << "label="<<label<<" position="<<position<<'\n';
	m_queuedLabels[ position ] << label;
}


void Code::removeInstruction( Instruction * instruction, bool pushLabels )
{
	if ( !instruction )
		return;
	
	// If the instruction could potentially be jumped over by a BTFSS or a
	// BTFSC intsruction, then we must also remove the bit test instruction,
	// else the next instruction will be jumped over instead after removal.
	// Removing the bit test instruction is perfectly safe as it only does
	// branching (not setting of any bits, etc).
	
	// Any labels that the instruction has must be given to the next
	// instruction.
	
	iterator e = end();
	iterator previous = e; // Refers to the previous instruction if it was a bit test instruction
	for ( iterator i = begin(); i != e; ++i )
	{
		if ( *i != instruction )
		{
			if ( dynamic_cast<Instr_btfss*>(*i) || dynamic_cast<Instr_btfsc*>(*i) )
				previous = i;
			else
				previous = e;
			continue;
		}
		
		iterator next = ++iterator(i);
		
		QStringList labels = instruction->labels();
		i.list->remove( i.it );
		
		if ( previous != e )
		{
			labels += (*previous)->labels();
			previous.list->remove( previous.it );
		}
		
		if ( pushLabels )
		{
			if ( next != e )
				(*next)->addLabels( labels );
		}
		
		return;
	}
}


void Code::append( Instruction * instruction, InstructionPosition position )
{
	if ( !instruction )
		return;
	
// 	cout << k_funcinfo << instruction->code() << '\n';
	
	removeInstruction( instruction, false );
	m_instructionLists[position].append( instruction );
	
	instruction->setCode( this );
	
	if ( instruction->type() == Instruction::Assembly /*||
			instruction->type() == Instruction::Raw*/ )
	{
// 		if ( !m_queuedLabels[position].isEmpty() )
// 			cout << "adding queued labels for 1: " << m_queuedLabels[position].join(",") << '\n';
		instruction->addLabels( m_queuedLabels[position] );
		m_queuedLabels[position].clear();
	}
}


Instruction * Code::instruction( const QString & label ) const
{
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		InstructionList::const_iterator end = m_instructionLists[i].end();
		for ( InstructionList::const_iterator it = m_instructionLists[i].begin(); it != end; ++it )
		{
			if ( (*it)->labels().contains( label ) )
				return *it;
		}
	}
	return 0l;
}


Code::iterator Code::find( Instruction * instruction )
{
	iterator e = end();
	iterator i = begin();
	for ( ; i != e; ++i )
	{
		if ( *i == instruction )
			break;
	}
	return i;
}


void Code::postCompileConstruct()
{
	// Give any queued labels to the instructions in the subsequent code block
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		if ( m_queuedLabels[i].isEmpty() )
			continue;
		
		QStringList labels = m_queuedLabels[i];
		m_queuedLabels[i].clear();
		
		// Find an instruction to dump them onto
		for ( unsigned block = i+1; block < PositionCount; ++block )
		{
			bool added = false;
			
			InstructionList::iterator end = m_instructionLists[block].end();
			for ( InstructionList::iterator it = m_instructionLists[block].begin(); it != end; ++it )
			{
				if ( (*it)->type() == Instruction::Assembly )
				{
// 					cout << "shoving labels onto next block.\n";
					(*it)->addLabels( labels );
					added = true;
					break;
				}
			}
			
			if ( added )
				break;
		}
	}
}


QString Code::generateCode( PIC14 * pic, bool showLinks ) const
{
	QString code;
	
	const QStringList variables = findVariables();
	if ( !variables.isEmpty() )
	{
		code += "; Variables\n";
		uchar reg = pic->gprStart();
		QStringList::const_iterator end = variables.end();
		for ( QStringList::const_iterator it = variables.begin(); it != end; ++it )
			code += QString("%1\tequ\t0x%2\n").arg( *it ).arg( QString::number( reg++, 16 ) );
		
		code += "\n";
	}
	
	QString picString = pic->minimalTypeString();
	code += QString("list p=%1\n").arg( picString );
	code += QString("include \"p%2.inc\"\n\n").arg( picString.lower() );
	
	code += "; Config options\n";
	code += "  __config _WDT_OFF\n\n";
	
	code += "START\n\n";
	
	
	CodeConstIterator e = end();
	
	typedef QMap< const Instruction *, int > InstructionIntMap;
	InstructionIntMap instructionLines;
	if ( showLinks )
	{
		unsigned at = 0;
		for ( CodeConstIterator it = begin(); it != e; ++it )
			instructionLines[ *it ] = at++;
	}
	
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		InstructionList::const_iterator end = m_instructionLists[i].end();
		for ( InstructionList::const_iterator it = m_instructionLists[i].begin(); it != end; ++it )
		{
			const QStringList labels = (*it)->labels();
			if ( !labels.isEmpty() )
			{
				code += '\n';
				QStringList::const_iterator labelsEnd = labels.end();
				for ( QStringList::const_iterator labelsIt = labels.begin(); labelsIt != labelsEnd; ++labelsIt )
					code += *labelsIt + '\n';
			}
		
			if ( showLinks )
			{
			//BEGIN input links
				QString inputs("(");
			
				const InstructionList inputLinks = (*it)->inputLinks();
				InstructionList::const_iterator end = inputLinks.end();
				for ( InstructionList::const_iterator linksIt = inputLinks.begin(); linksIt != end; ++linksIt )
					inputs += QString("%1,").arg( instructionLines[ *linksIt ] );
			
				if ( inputs.length() > 1 )
					inputs.remove( inputs.length()-1, 1 );
				inputs += ")";
			//END input links
			
			
			//BEGIN output links
				QString outputs("(");
			
				const InstructionList outputLinks = (*it)->outputLinks();
				end = outputLinks.end();
				for ( InstructionList::const_iterator linksIt = outputLinks.begin(); linksIt != end; ++linksIt )
					outputs += QString("%1,").arg( instructionLines[ *linksIt ] );
			
				if ( outputs.length() > 1 )
					outputs.remove( inputs.length()-1, 1 );
				outputs += ")";
			//END output links
			
			
				code += QString("#%1) %3 -> %4")
						.arg( instructionLines[ *it ], 4 )
// 					.arg( inputs, 50 )
						.arg( (*it)->code().replace( '\t', "   " ), -40 )
						.arg( outputs, -12 );
			}
			else
			{
				if ( (*it)->type() == Instruction::Assembly )
					code += '\t';
		
				code += (*it)->code();
			}
		
			code += '\n';
		}
	}
	
	return code;
}


QStringList Code::findVariables() const
{
	QStringList variables;
	
	const_iterator e = end();
	for ( const_iterator i = begin(); i != e; ++i )
	{
		if ( (*i)->file().type() != Register::GPR )
			continue;
		
		QString alias = (*i)->file().name();
		if ( !variables.contains( alias ) )
			variables << alias;
	}
	
	return variables;
}


void Code::generateLinksAndStates()
{
	CodeIterator e = end();
	
	for ( CodeIterator it = begin(); it != e; ++it )
			(*it)->clearLinks();
	
	for ( CodeIterator it = begin(); it != e; ++it )
		(*it)->generateLinksAndStates( it );
	
	// Generate return links for call instructions
	// This cannot be done from the call instructions as we need to have
	// generated the links first.
	for ( CodeIterator it = begin(); it != e; ++it )
	{
		Instr_call * ins = dynamic_cast<Instr_call*>(*it);
		if ( !ins )
			continue;
		
		Instruction * next = *(++Code::iterator(it));
		ins->makeReturnLinks( next );
	}
}


void Code::setAllUnused()
{
	CodeIterator e = end();
	for ( CodeIterator it = begin(); it != e; ++it )
	{
		(*it)->setUsed( false );
		(*it)->resetRegisterDepends();
	}
}


CodeIterator Code::begin()
{
	// Following code is very similar to the  version of this function.
	// Make sure any changes are applied to both (when applicable).
	
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		if ( m_instructionLists[i].isEmpty() )
			continue;
		
		CodeIterator codeIterator;
		codeIterator.code = this;
		codeIterator.it = m_instructionLists[i].begin();
		codeIterator.pos = (Code::InstructionPosition)i;
		codeIterator.list = & m_instructionLists[i];
		codeIterator.listEnd = m_instructionLists[i].end();
		
		return codeIterator;
	}
	
	return end();
}


CodeIterator Code::end()
{
	// Following code is very similar to the  version of this function.
	// Make sure any changes are applied to both (when applicable).
	
	CodeIterator codeIterator;
	codeIterator.code = this;
	codeIterator.it = m_instructionLists[ PositionCount - 1 ].end();
	codeIterator.pos = (Code::InstructionPosition)(Code::PositionCount - 1);
	codeIterator.list = & m_instructionLists[ PositionCount - 1 ];
	codeIterator.listEnd = m_instructionLists[ PositionCount - 1 ].end();
	return codeIterator;
}


CodeConstIterator Code::begin() const
{
	// Following code is very similar to the non-const version of this function.
	// Make sure any changes are applied to both (when applicable).
	
	for ( unsigned i = 0; i < PositionCount; ++i )
	{
		if ( m_instructionLists[i].isEmpty() )
			continue;
		
		CodeConstIterator codeIterator;
		codeIterator.code = this;
		codeIterator.it = m_instructionLists[i].begin();
		codeIterator.pos = (Code::InstructionPosition)i;
		codeIterator.list = & m_instructionLists[i];
		codeIterator.listEnd = m_instructionLists[i].end();
		
		return codeIterator;
	}
	
	return end();
}


CodeConstIterator Code::end() const
{
	// Following code is very similar to the non-const version of this function.
	// Make sure any changes are applied to both (when applicable).
	
	CodeConstIterator codeIterator;
	codeIterator.code = this;
	codeIterator.it = m_instructionLists[ PositionCount - 1 ].end();
	codeIterator.pos = (Code::InstructionPosition)(Code::PositionCount - 1);
	codeIterator.list = & m_instructionLists[ PositionCount - 1 ];
	codeIterator.listEnd = m_instructionLists[ PositionCount - 1 ].end();
	return codeIterator;
}
//END class Code



//BEGIN class CodeIterator
CodeIterator & CodeIterator::operator ++ ()
{
	// NOTE: This code is very similar to the const version.
	// Any changes to thsi code should be applied there as well (when applicable).
	
	do
	{
		if ( ++it == listEnd && pos < (Code::PositionCount - 1) )
		{
			bool found = false;
			for ( pos = (Code::InstructionPosition)(pos+1); pos < Code::PositionCount; pos = (Code::InstructionPosition)(pos+1) )
			{
				list = code->instructionList( pos );
				listEnd = list->end();
				if ( list->isEmpty() )
					continue;
		
				it = list->begin();
				found = true;
				break;
			}
	
			if ( !found )
				it = listEnd;
		}
	}
	while ( (it != listEnd) && ((*it)->type() != Instruction::Assembly) );
	
	return *this;
}


CodeIterator & CodeIterator::operator -- ()
{
	CodeIterator prev = code->begin();
	CodeIterator end = code->end();
	for ( CodeIterator it = prev; it != end; ++it )
	{
		if ( *it == **this )
			break;
		prev = it;
	}
	
	assert( ++ CodeIterator( prev ) == *this );
	
	*this = prev;
	return *this;
}


CodeIterator & CodeIterator::removeAndIncrement()
{
	Instruction * i = *it;
	++(*this);
	code->removeInstruction( i, true );
	return *this;
}


void CodeIterator::insertBefore( Instruction * ins )
{
	if ( *this == code->end() )
		list->append( ins );
	else
		list->insert( it, ins );
}
//END class CodeIterator



//BEGIN class CodeConstIterator
CodeConstIterator & CodeConstIterator::operator ++ ()
{
	// NOTE: This code is very similar to the non-const version.
	// Any changes to thsi code should be applied there as well (when applicable).
	
	do
	{
		if ( ++it == listEnd && pos < (Code::PositionCount - 1) )
		{
			bool found = false;
			for ( pos = (Code::InstructionPosition)(pos+1); pos < Code::PositionCount; pos = (Code::InstructionPosition)(pos+1) )
			{
				list = code->instructionList( pos );
				listEnd = list->end();
				if ( list->isEmpty() )
					continue;
		
				it = list->begin();
				found = true;
				break;
			}
	
			if ( !found )
				it = listEnd;
		}
	}
	while ( (it != listEnd) && ((*it)->type() != Instruction::Assembly) );
	
	return *this;
}
//END class CodeConstIterator




//BEGIN class Instruction
Instruction::Instruction()
{
	m_bInputStateChanged = true;
	m_bPositionAffectsBranching = false;
	m_bUsed = false;
	m_literal = 0;
	m_dest = 0;
}


Instruction::~ Instruction()
{
}


void Instruction::addLabels( const QStringList & labels )
{
	m_labels += labels;
// 	if ( !labels.isEmpty() )
// 		cout << k_funcinfo << "added labels: " << labels.join(",") << '\n';
}


void Instruction::setLabels( const QStringList & labels )
{
	m_labels = labels;
}


void Instruction::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	m_outputState.reset();
}


ProcessorBehaviour Instruction::behaviour() const
{
	return ProcessorBehaviour();
}


void Instruction::makeOutputLinks( Code::iterator current, bool firstOutput, bool secondOutput )
{
	if ( !firstOutput && !secondOutput )
		return;
	
	++current;
	if ( !*current )
	{
		kdWarning() << k_funcinfo << "current+1 is null"<<endl;
		return;
	}
	if ( firstOutput )
		(*current)->addInputLink( this );
	
	if ( !secondOutput )
		return;
	
	++current;
	(*current)->addInputLink( this );
}


void Instruction::makeLabelOutputLink( const QString & label )
{
	Instruction * output = m_pCode->instruction( label );
	if ( output )
		output->addInputLink( this );
}


void Instruction::addInputLinks( const InstructionList & instructions )
{
	InstructionList::const_iterator end = instructions.end();
	for ( InstructionList::const_iterator it = instructions.begin(); it != end; ++it )
		addInputLink( *it );
}


void Instruction::addInputLink( Instruction * instruction )
{
	// Don't forget that a link to ourself is valid!
	if ( !instruction || m_inputLinks.contains( instruction ) )
		return;
	
	m_inputLinks << instruction;
	instruction->addOutputLink( this );
}


void Instruction::addOutputLinks( const InstructionList & instructions )
{
	InstructionList::const_iterator end = instructions.end();
	for ( InstructionList::const_iterator it = instructions.begin(); it != end; ++it )
		addOutputLink( *it );
}


void Instruction::addOutputLink( Instruction * instruction )
{
	// Don't forget that a link to ourself is valid!
	if ( !instruction || m_outputLinks.contains( instruction ) )
		return;
	
	m_outputLinks << instruction;
	instruction->addInputLink( this );
}


void Instruction::removeInputLink( Instruction * instruction )
{
	m_inputLinks.remove( instruction );
}


void Instruction::removeOutputLink( Instruction * instruction )
{
	m_outputLinks.remove( instruction );
}


void Instruction::clearLinks()
{
	m_inputLinks.clear();
	m_outputLinks.clear();
}
//END class Instruction



//BEGIN Byte-Oriented File Register Operations
QString Instr_addwf::code() const
{
	return QString("addwf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_addwf::generateLinksAndStates( Code::iterator current )
{
	m_outputState = m_inputState;
	
	m_outputState.reg( outputReg() ).value = (m_inputState.working.value + m_inputState.reg( m_file ).value) & 0xff;
	m_outputState.reg( outputReg() ).known = ((m_inputState.working.known == 0xff) && (m_inputState.reg( m_file ).known == 0xff)) ? 0xff : 0x0;
	
	m_outputState.status.known &= ~( (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z) );
	
	if ( m_file.type() != Register::PCL || m_dest == 0 )
	{
		makeOutputLinks( current );
		return;
	}
	
	++current; // Don't have a link to ourself
	
	// maxInc is the greatest possibly value that we might have incremented the program counter by.
	// It is generated by ORing the known bits of the working register with the greatest value
	// of the unknown bits;
	uchar maxInc = m_inputState.working.maxValue();
	if ( maxInc < 0xff )
		maxInc++;
// 	cout << "m_inputState.working.known="<<int(m_inputState.working.known)<<" maxInc="<<int(maxInc)<<'\n';
	Code::iterator end = m_pCode->end();
	for ( int i = 0; current != end && i < maxInc; ++i, ++current )
	{
		(*current)->addInputLink( this );
// 		if ( i != maxInc-1 )
// 			(*current)->setPositionAffectsBranching( true );
	}
}

ProcessorBehaviour Instr_addwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Depend on W and f
	behaviour.working.depends = 0xff;
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z);
	return behaviour;
}



QString Instr_andwf::code() const
{
	return QString("andwf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_andwf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	m_outputState = m_inputState;
	
	uchar definiteOnes = m_inputState.reg( m_file ).definiteOnes() & m_outputState.working.definiteOnes();
	m_outputState.reg( outputReg() ).value = definiteOnes;
	m_outputState.reg( outputReg() ).known = m_inputState.reg( m_file ).definiteZeros() | m_inputState.working.definiteZeros() | definiteOnes;
	
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
}

ProcessorBehaviour Instr_andwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Depend on W and f
	behaviour.working.depends = 0xff;
	behaviour.reg( m_file ).depends = 0xff;
	
	if ( m_dest == 0 )
		behaviour.working.indep = m_inputState.reg( m_file ).known & ~( m_inputState.reg( m_file ).value);
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_clrf::code() const
{
	return QString("clrf\t%1").arg( m_file.name() );
}

void Instr_clrf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.reg( m_file ).known = 0xff;
	m_outputState.reg( m_file ).value = 0x0;
	
	m_outputState.status.known |= (1 << RegisterBit::Z);
	m_outputState.status.value |= (1 << RegisterBit::Z);
}

ProcessorBehaviour Instr_clrf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.reg( m_file ).indep = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	
	return behaviour;
}


//TODO CLRW
//TODO COMF


QString Instr_decf::code() const
{
	return QString("decf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_decf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_decf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	
	return behaviour;
}


QString Instr_decfsz::code() const
{
	return QString("decfsz\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_decfsz::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current, true, true );
	
	m_outputState = m_inputState;
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_decfsz::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	return behaviour;
}


QString Instr_incf::code() const
{
	return QString("incf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_incf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_incf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}


//TODO INCFSZ


QString Instr_iorwf::code() const
{
	return QString("iorwf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_iorwf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_iorwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Depend on W and f
	behaviour.working.depends = 0xff;
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_movf::code() const
{
	return QString("movf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_movf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	
	if ( m_inputState.reg( m_file ).known == 0xff )
	{
		m_outputState.status.known |= (1 << RegisterBit::Z);
		bool isZero = (m_inputState.reg( m_file ).value == 0x0);
		if ( isZero )
			m_outputState.status.value |= (1 << RegisterBit::Z);
		else
			m_outputState.status.value &= ~(1 << RegisterBit::Z);
	}
	else
		m_outputState.status.known &= ~(1 << RegisterBit::Z);
	
	if ( m_dest == 0 )
	{
		// Writing to the working register
		m_outputState.working.known = m_inputState.reg( m_file ).known;
		m_outputState.working.value = m_inputState.reg( m_file ).value;
	}
}

ProcessorBehaviour Instr_movf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	if ( m_dest == 0 )
		behaviour.working.indep = 0xff;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_movwf::code() const
{
	return QString("movwf\t%1").arg( m_file.name() );
}

void Instr_movwf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.reg( m_file ).known = m_inputState.working.known;
	m_outputState.reg( m_file ).value = m_inputState.working.value;
}

ProcessorBehaviour Instr_movwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.reg( m_file ).indep = 0xff;
	behaviour.working.depends = 0xff;
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	
	return behaviour;
}


//TODO NOP



QString Instr_rlf::code() const
{
	return QString("rlf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_rlf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::C);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_rlf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Is the value written to W or f?
	if ( m_dest == 0 )
		behaviour.working.indep = 0xff;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = (1 << RegisterBit::C) | (m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0);
	behaviour.status.indep = (1 << RegisterBit::C);
	return behaviour;
}


QString Instr_rrf::code() const
{
	return QString("rrf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_rrf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::C);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_rrf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	if ( m_dest == 0 )
		behaviour.working.indep = 0xff;
	
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = (1 << RegisterBit::C) | (m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0);
	behaviour.status.indep = (1 << RegisterBit::C);
	return behaviour;
}


QString Instr_subwf::code() const
{
	return QString("subwf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_subwf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	
	if ( (m_inputState.working.known == 0xff) && (m_inputState.reg( m_file ).known == 0xff) )
	{
		m_outputState.reg( outputReg() ).known = 0xff;
		m_outputState.reg( outputReg() ).value = (m_inputState.reg( m_file ).value - m_inputState.working.value) & 0xff;
	}
	else
		m_outputState.reg( outputReg() ).known = 0x0;
	
	
	m_outputState.status.known &= ~( (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z) );
	
	if ( m_inputState.working.minValue() > m_inputState.reg( m_file ).maxValue() )
	{
		m_outputState.status.value &= ~(1 << RegisterBit::C);
		m_outputState.status.known |= (1 << RegisterBit::C);
	}
	else if ( m_inputState.working.maxValue() <= m_inputState.reg( m_file ).minValue() )
	{
		m_outputState.status.value |= (1 << RegisterBit::C);
		m_outputState.status.known |= (1 << RegisterBit::C);
	}
	
	if ( (m_inputState.working.known == 0xff) && (m_inputState.reg( m_file ).known == 0xff) )
	{
		bool isZero = (m_inputState.working.value == m_inputState.reg( m_file ).value);
		if ( isZero )
			m_outputState.status.value |= (1 << RegisterBit::Z);
		else
			m_outputState.status.value &= ~(1 << RegisterBit::Z);
		m_outputState.status.known |= (1 << RegisterBit::Z);
	}
}

ProcessorBehaviour Instr_subwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Depend on W and f
	behaviour.working.depends = 0xff;
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_swapf::code() const
{
	return QString("swapf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_swapf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	if ( m_dest == 0 )
	{
		// Writing to the working register
		m_outputState.working.known = 0x0;
	}
}

ProcessorBehaviour Instr_swapf::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.reg( m_file ).depends = 0xff;
	behaviour.working.indep = ( m_dest == 0 ) ? 0xff : 0x0;
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	return behaviour;
}


QString Instr_xorwf::code() const
{
	return QString("xorwf\t%1,%2").arg( m_file.name() ).arg( m_dest );
}

void Instr_xorwf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
	
	m_outputState.reg( outputReg() ).known = 0x0;
}

ProcessorBehaviour Instr_xorwf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	// Depend on W and f
	behaviour.working.depends = 0xff;
	behaviour.reg( m_file ).depends = 0xff;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}
//END Byte-Oriented File Register Operations



//BEGIN Bit-Oriented File Register Operations
QString Instr_bcf::code() const
{
	return QString("bcf\t\t%1,%2").arg( m_file.name() ).arg( m_bit.name() );
}

void Instr_bcf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.reg( m_file ).value &= ~uchar(1 << m_bit.bitPos());
	m_outputState.reg( m_file ).known |= uchar(1 << m_bit.bitPos());
}

ProcessorBehaviour Instr_bcf::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.reg( m_file ).indep = 1 << m_bit.bitPos();
	return behaviour;
}


QString Instr_bsf::code() const
{
	return QString("bsf\t\t%1,%2").arg( m_file.name() ).arg( m_bit.name() );
}

void Instr_bsf::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.reg( m_file ).value |= uchar(1 << m_bit.bitPos());
	m_outputState.reg( m_file ).known |= uchar(1 << m_bit.bitPos());
}

ProcessorBehaviour Instr_bsf::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.status.depends = m_file.bankDependent() ? (1 << RegisterBit::RP0) : 0x0;
	behaviour.reg( m_file ).indep = 1 << m_bit.bitPos();
	return behaviour;
}


QString Instr_btfsc::code() const
{
	return QString("btfsc\t%1,%2").arg( m_file.name() ).arg( m_bit.name() );
}

void Instr_btfsc::generateLinksAndStates( Code::iterator current )
{
	m_outputState = m_inputState;
	
	if ( m_inputState.reg( m_file ).known & (1 << m_bit.bitPos()) )
	{
		bool bit = m_inputState.reg( m_file ).value & (1 << m_bit.bitPos());
		makeOutputLinks( current, bit, !bit );
	}
	else
		makeOutputLinks( current, true, true );
}

ProcessorBehaviour Instr_btfsc::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.reg( m_file ).depends = 1 << m_bit.bitPos();
	behaviour.status.depends = (m_file.type() == Register::STATUS) ? m_bit.bit() : 0x0;
	return behaviour;
}


QString Instr_btfss::code() const
{
	return QString("btfss\t%1,%2").arg( m_file.name() ).arg( m_bit.name() );
}

void Instr_btfss::generateLinksAndStates( Code::iterator current )
{
	m_outputState = m_inputState;
	
	if ( m_inputState.reg( m_file ).known & (1 << m_bit.bitPos()) )
	{
		bool bit = m_inputState.reg( m_file ).value & (1 << m_bit.bitPos());
		makeOutputLinks( current, !bit, bit );
	}
	else
		makeOutputLinks( current, true, true );
}

ProcessorBehaviour Instr_btfss::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.reg( m_file ).depends = 1 << m_bit.bitPos();
	behaviour.status.depends = (m_file.type() == Register::STATUS) ? m_bit.bit() : 0x0;
	return behaviour;
}
//END Bit-Oriented File Register Operations



//BEGIN Literal and Control Operations
QString Instr_addlw::code() const
{
	return QString("addlw\t%1").arg( m_literal );
}

void Instr_addlw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.working.value = (m_inputState.working.value + m_literal) & 0xff;
	m_outputState.working.known = (m_inputState.working.known == 0xff) ? 0xff : 0x0;
	m_outputState.status.known &= ~( (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z) );
}

ProcessorBehaviour Instr_addlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.working.depends = 0xff;
	
	behaviour.status.indep = (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z);
	
	return behaviour;
}


QString Instr_andlw::code() const
{
	return QString("andlw\t%1").arg( m_literal );
}

void Instr_andlw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.working.value = (m_inputState.working.value & m_literal) & 0xff;
	m_outputState.working.known |= ~m_literal; // Now know any bits that are zero in value
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
}

ProcessorBehaviour Instr_andlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.working.indep = ~m_literal;
	behaviour.working.depends = m_literal;
	
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_call::code() const
{
	return QString("call\t%1").arg( m_label );
}

void Instr_call::generateLinksAndStates( Code::iterator current )
{
	(void)current;
	makeLabelOutputLink( m_label );
	
	m_outputState = m_inputState;
}

ProcessorBehaviour Instr_call::behaviour() const
{
	ProcessorBehaviour behaviour;
	return behaviour;
}

void Instr_call::makeReturnLinks( Instruction * next )
{
	m_pCode->setAllUnused();
	linkReturns( m_pCode->instruction( m_label ), next );
}


void Instr_call::linkReturns( Instruction * current, Instruction * returnPoint )
{
	while (true)
	{
		if ( !current || current->isUsed() )
			return;
		
		current->setUsed( true );
		if ( dynamic_cast<Instr_return*>(current) || dynamic_cast<Instr_retlw*>(current) )
		{
// 			cout << "Added return link" << endl;
// 			cout << "   FROM: " << current->code() << endl;
// 			cout << "   TO:   " << returnPoint->code() << endl;
			returnPoint->addInputLink( current );
			return;
		}
		if ( dynamic_cast<Instr_call*>(current) )
		{
			// Jump over the call instruction to its return point,
			// which will be the instruction after current.
			current = *(++m_pCode->find( current ));
			continue;
		}
		
		const InstructionList outputs = current->outputLinks();
		
		if ( outputs.isEmpty() )
			return;
		
		if ( outputs.size() == 1 )
			current = outputs.first();
		
		else
		{
			// Can't avoid function recursion now.
			InstructionList::const_iterator end = outputs.end();
			for ( InstructionList::const_iterator it = outputs.begin(); it != end; ++it )
				linkReturns( *it, returnPoint );
			return;
		}
	};
}


//TODO CLRWDT


QString Instr_goto::code() const
{
	return QString("goto\t%1").arg( m_label );
}

void Instr_goto::generateLinksAndStates( Code::iterator current )
{
	(void)current;
	
	makeLabelOutputLink( m_label );
	
	m_outputState = m_inputState;
}

ProcessorBehaviour Instr_goto::behaviour() const
{
	ProcessorBehaviour behaviour;
	return behaviour;
}


QString Instr_iorlw::code() const
{
	return QString("iorlw\t%1").arg( m_literal );
}

void Instr_iorlw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.working.value = (m_inputState.working.value | m_literal) & 0xff;
	m_outputState.working.known |= m_literal; // Now know any bits that are one in value
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
}

ProcessorBehaviour Instr_iorlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	
	behaviour.working.indep = m_literal;
	behaviour.working.depends = ~m_literal;
	
	behaviour.status.indep = (1 << RegisterBit::Z);;
	return behaviour;
}


QString Instr_movlw::code() const
{	
	return QString("movlw\t%1").arg( m_literal );
}

void Instr_movlw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	m_outputState = m_inputState;
	m_outputState.working.known = 0xff;
	m_outputState.working.value = m_literal;
}

ProcessorBehaviour Instr_movlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.working.indep = 0xff;
	return behaviour;
}


QString Instr_retfie::code() const
{
	return "retfie";
}

void Instr_retfie::generateLinksAndStates( Code::iterator current )
{
	// Don't generate any output links
	(void)current;
	
	m_inputState = m_outputState;
}

ProcessorBehaviour Instr_retfie::behaviour() const
{
	ProcessorBehaviour behaviour;
	return behaviour;
}


QString Instr_retlw::code() const
{
	return QString("retlw\t%1").arg( m_literal );
}

void Instr_retlw::generateLinksAndStates( Code::iterator current )
{
	(void)current;
	
	m_outputState = m_inputState;
	m_outputState.working.known = 0xff;
	m_outputState.working.value = m_literal;
}

ProcessorBehaviour Instr_retlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.working.indep = 0xff;
	return behaviour;
}



QString Instr_return::code() const
{
	return "return";
}

void Instr_return::generateLinksAndStates( Code::iterator current )
{
	(void)current;
	
	m_outputState = m_inputState;
}

ProcessorBehaviour Instr_return::behaviour() const
{
	ProcessorBehaviour behaviour;
	return behaviour;
}


QString Instr_sleep::code() const
{
	return "sleep";
}

void Instr_sleep::generateLinksAndStates( Code::iterator current )
{
	// Don't generate any output links
	(void)current;
	
	m_outputState = m_inputState;
	m_outputState.status.value &= ~(1 << RegisterBit::NOT_PD);
	m_outputState.status.value |= (1 << RegisterBit::NOT_TO);
	m_outputState.status.known |= (1 << RegisterBit::NOT_TO) | (1 << RegisterBit::NOT_PD);
}

ProcessorBehaviour Instr_sleep::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.status.indep = (1 << RegisterBit::NOT_TO) | (1 << RegisterBit::NOT_PD);
	return behaviour;
}


QString Instr_sublw::code() const
{
	return QString("sublw\t%1").arg( m_literal );
}

void Instr_sublw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	
	m_outputState = m_inputState;
	m_outputState.working.value = (m_literal - m_inputState.working.value) & 0xff;
	m_outputState.working.known = (m_inputState.working.known == 0xff) ? 0xff : 0x00;
	m_outputState.status.known &= ~( (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z) );
	
	if ( m_inputState.working.minValue() > m_literal )
	{
		m_outputState.status.value &= ~(1 << RegisterBit::C);
		m_outputState.status.known |= (1 << RegisterBit::C);
	}
	else if ( m_inputState.working.maxValue() <= m_literal )
	{
		m_outputState.status.value |= (1 << RegisterBit::C);
		m_outputState.status.known |= (1 << RegisterBit::C);
	}
	
	if ( m_inputState.working.known == 0xff )
	{
		bool isZero = (m_inputState.working.value == m_literal);
		if ( isZero )
			m_outputState.status.value |= (1 << RegisterBit::Z);
		else
			m_outputState.status.value &= ~(1 << RegisterBit::Z);
		m_outputState.status.known |= (1 << RegisterBit::Z);
	}
}

ProcessorBehaviour Instr_sublw::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.working.depends = 0xff;
	behaviour.status.indep = (1 << RegisterBit::C) | (1 << RegisterBit::DC) | (1 << RegisterBit::Z);
	return behaviour;
}


QString Instr_xorlw::code() const
{
	return QString("xorlw\t%1").arg( m_literal );
}

void Instr_xorlw::generateLinksAndStates( Code::iterator current )
{
	makeOutputLinks( current );
	m_outputState = m_inputState;
	m_outputState.working.value = (m_inputState.working.value ^ m_literal) & 0xff;
	m_outputState.working.known = m_inputState.working.known;
	m_outputState.status.known &= ~(1 << RegisterBit::Z);
}

ProcessorBehaviour Instr_xorlw::behaviour() const
{
	ProcessorBehaviour behaviour;
	behaviour.working.depends = 0xff;
	behaviour.status.indep = (1 << RegisterBit::Z);
	return behaviour;
}
//END Literal and Control Operations



//BEGIN Microbe (non-assembly) Operations
QString Instr_sourceCode::code() const
{
	QStringList sourceLines = QStringList::split("\n",m_raw);
	return ";" + sourceLines.join("\n;");
}


QString Instr_asm::code() const
{
	return "; asm {\n" + m_raw + "\n; }";
}


QString Instr_raw::code() const
{
	return m_raw;
}
//END Microbe (non-assembly) Operations

