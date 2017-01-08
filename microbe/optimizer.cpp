/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "instruction.h"
#include "optimizer.h"

#include <kdebug.h>
#include <klocalizedstring.h>

#include <cassert>
#include <iostream>
using namespace std;


QString binary( uchar val )
{
	QString bin = QString::number( val, 2 );
	QString pad;
	pad.fill( '0', 8-bin.length() );
	return pad + bin; 
}


Optimizer::Optimizer()
{
	m_pCode = 0l;
}


Optimizer::~Optimizer()
{
}


void Optimizer::optimize( Code * code )
{
// 	return;
	m_pCode = code;

    const int maxIterations = 10000; // selected randomly

	bool changed;
    int iterationNumber = 0;
	do
	{
        ++iterationNumber;
		changed = false;
		
		// Repeatedly generate links and states until
		// we know as much as possible about the system.
		propagateLinksAndStates();
		
		// Remove instructions without input links
		changed |= pruneInstructions();
		
		// Perform optimizations based on processor states
		changed |= optimizeInstructions();
	}
	while ( changed && (iterationNumber < maxIterations) );

    if (iterationNumber >= maxIterations) {
        QString warnMessage( i18n(
            "Internal issue: Optimization has not finished in %1 iterations.",
            iterationNumber) );
        //qDebug() << warnMessage; // qDebug or qWarning generates "compilation failed" message in ktechlab
        std::cout << warnMessage.toStdString();
    }
}


void Optimizer::propagateLinksAndStates()
{
	int count = 0;
	
	do
	{
		count++;
		m_pCode->generateLinksAndStates();
	}
	while ( giveInputStates() );
	
// 	cout << "count="<<count<<endl;
}


bool Optimizer::giveInputStates()
{
	bool changed = false;
	
	Code::iterator end = m_pCode->end();
	for ( Code::iterator it = m_pCode->begin(); it != end; ++it )
	{
		// Now, build up the most specific known processor state from the instructins
		// that could be executed immediately before this instruction.
		// This is done by taking the output state of the first input link, and
		// then reducing it to the greatest common denominator of all the input states.
		
		const InstructionList list = (*it)->inputLinks();
		if ( list.isEmpty() )
			continue;
		
		InstructionList::const_iterator inputIt = list.begin();
		InstructionList::const_iterator inputsEnd = list.end();
		
		ProcessorState input = (*(inputIt++))->outputState();
		
		while ( inputIt != inputsEnd )
			input.merge( (*inputIt++)->outputState() );
		
		if ( !changed )
		{
			ProcessorState before = (*it)->inputState();
			bool stateChanged = ( before != input );
			changed |= stateChanged;
		}
		
		(*it)->setInputState( input );
	}
	return changed;
}


bool Optimizer::pruneInstructions()
{
	bool removed = false;
	
	//BEGIN remove instructions without any input links
	Code::iterator it = m_pCode->begin();
	Code::iterator end = m_pCode->end();
	
	// Jump past the first instruction, as nothing (necessarily) points to that
	if ( it != end )
		++it;
	
	while ( it != end )
	{
		if ( (*it)->inputLinks().isEmpty() )
		{
// 			cout << "Removing: " << (*it)->code() << endl;
			it.removeAndIncrement();
			removed = true;
		}
		else
			++it;
	}
	end = m_pCode->end(); // Reset end as instructions may have been removed
	//END remove instructions without any input links
	
	
	//BEGIN remove labels without any reference to them
	// First: build up a list of labels which are referenced
	QStringList referencedLabels;
	for ( it = m_pCode->begin(); it != end; ++it )
	{
		if ( Instr_goto * ins = dynamic_cast<Instr_goto*>(*it) )
			referencedLabels << ins->label();
		else if ( Instr_call * ins = dynamic_cast<Instr_call*>(*it) )
			referencedLabels << ins->label();
	}
	
	// Now remove labels from instructions that aren't in the referencedLabels list
	for ( it = m_pCode->begin(); it != end; ++it )
	{
		QStringList labels = (*it)->labels();
		
		for ( QStringList::iterator labelsIt = labels.begin(); labelsIt != labels.end(); )
		{
			if ( !referencedLabels.contains( *labelsIt ) )
			{
				labelsIt = labels.erase( labelsIt );
				removed = true;
			}
			else
				++labelsIt;
		}
		
		(*it)->setLabels( labels);
	}
	//END remove labels without any reference to them
	
	return removed;
}


bool Optimizer::optimizeInstructions()
{
	//BEGIN Optimization 1: Concatenate chained GOTOs
	// We go through the instructions looking for GOTO statements. If we find any, then
	// we trace back through their input links to any other GOTO statements - any that
	// are found are then redirected to point to the label that the original GOTO statement
	// was pointing at.
	Code::iterator end = m_pCode->end();
	for ( Code::iterator it = m_pCode->begin(); it != end; ++it )
	{
		Instr_goto * gotoIns = dynamic_cast<Instr_goto*>(*it);
		if ( !gotoIns )
			continue;
		
		if ( redirectGotos( gotoIns, gotoIns->label() ) )
			return true;
		m_pCode->setAllUnused();
	}
	//END Optimization 1: Concatenate chained GOTOs
	
	
	//BEGIN Optimization 2: Remove GOTOs when jumping to the subsequent instruction
	// Any GOTO instructions that just jump to the next instruction can be removed.
	for ( Code::iterator it = m_pCode->begin(); it != end; ++it )
	{
		Instruction * next = *(++Code::iterator(it));
		Instruction * gotoIns = dynamic_cast<Instr_goto*>(*it);
		if ( !gotoIns || !next || (gotoIns->outputLinks().first() != next) )
			continue;
		
// 		cout << "Removing: " << gotoIns->code() << endl;
		it.removeAndIncrement();
		return true;
	}
	end = m_pCode->end();
	//END Optimization 2: Remove GOTOs when jumping to the subsequent instruction
	
	
	//BEGIN Optimization 3: Replace MOVWF with CLRF with W is 0
	// We look for MOVWF instructions where the working register holds zero.
	// We then replace the MOVWf instruction with a CLRF instruction.
	for ( Code::iterator it = m_pCode->begin(); it != end; ++it )
	{
		Instr_movwf * ins = dynamic_cast<Instr_movwf*>(*it);
		if ( !ins )
			continue;
		
		ProcessorState inputState = ins->inputState();
		RegisterState working = inputState.working;
		if ( (working.value != 0x0) || (working.known != 0xff) )
			continue;
		
		// CLRF sets the Z flag of STATUS to 1, but MOVWF does not set any flags.
		// So we need to check for dependence of the Z flag if we are possibly
		// changing the flag by replacing the instruction.
		if ( !(inputState.status.definiteOnes() & (1 << RegisterBit::Z)) )
		{
			// Input state of Z flag is either unknown or low.
			
			uchar depends = generateRegisterDepends( *it, Register::STATUS );
			if ( depends & (1 << RegisterBit::Z) )
			{
				// Looks like there's some instruction that depends on the zero bit,
				// and we about potentially about to change it.
				continue;
			}
		}
		
		
		Instr_clrf * instr_clrf = new Instr_clrf( ins->file() );
// 		cout << "Replacing \""<<(*it)->code()<<"\" with \""<<instr_clrf->code()<<"\"\n";
		it.insertBefore( instr_clrf );
		it.removeAndIncrement();
		return true;
	}
	//END Optimization 3: Replace MOVWF with CLRF with W is 0
	
	
	//BEGIN Optimization 4: Replace writes to W with MOVLW when value is known
	// We look for instructions with AssemblyType either WorkingOriented, or FileOriented
	// and writing to W. Then, if the value is known and there are no instructions that
	// depend on the STATUS bits set by the instruction, then we replace it with a MOVLW
	for ( Code::iterator it = m_pCode->begin(); it != end; ++it )
	{
		if ( dynamic_cast<Instr_movlw*>(*it) )
		{
			// If we don't catch this condition, we'll end up in an infinite loop,
			// repeatedly replacing the first MOVLW that we come across.
			continue;
		}
		
		bool workingOriented = (*it)->assemblyType() == Instruction::WorkingOriented;
		bool fileOriented = (*it)->assemblyType() == Instruction::FileOriented;
		if ( !workingOriented && (!fileOriented || ((*it)->dest() != 0)) )
			continue;
		
		// So can now assume that workingOriented and fileOriented are logical opposites
		
		RegisterState outputState = (*it)->outputState().working;
		if ( outputState.known != 0xff )
			continue;
		
		ProcessorBehaviour behaviour = (*it)->behaviour();
		
		// MOVLW does not set any STATUS flags, but the instruction that we are replacing
		// might. So we must check if any of these STATUS flags are depended upon, and if so
		// only allow replacement if the STATUS flags are not being changed.
		if ( !canRemove( *it, Register::STATUS, behaviour.reg( Register::STATUS ).indep ) )
			continue;
		
		Instr_movlw * movlw = new Instr_movlw( outputState.value );
// 		cout << "Replacing \""<<(*it)->code()<<"\" with \""<<movlw->code()<<"\"\n";
		it.insertBefore( movlw );
		it.removeAndIncrement();
		return true;
	}
	//END Optimization 4: Replace writes to W with MOVLW when value is known
	
	
	//BEGIN Optimization 5: Remove writes to a bit when the value is ignored and overwritten again
	// We go through the instructions looking for statements that write to a bit (bcf, bsf).
	//  If we find any, then we trace through their output links to see if their value is
	// overwritten before it is used - and if so, the instruction can be removed.
	for ( Code::iterator it = m_pCode->begin(); it != end; ++it )
	{
		if ( (*it)->assemblyType() != Instruction::BitOriented )
			continue;
		
		const Register regSet = (*it)->file();
		
		if ( regSet.affectsExternal() )
			continue;
		
		uchar bitPos = (*it)->bit().bitPos();
		
		ProcessorState inputState = (*it)->inputState();
		ProcessorState outputState = (*it)->outputState();
		ProcessorBehaviour behaviour = (*it)->behaviour();
		
		// Are we rewriting over a bit that already has the same value?
		// (Note this check is just for the bit changing instructions, as there is a similar
		// check for register changing actions later on when we know which bits care about
		// being overwritten).
		if ( inputState.reg( regSet ).known & (1 << bitPos) )
		{
			bool beforeVal = (inputState.reg( regSet ).value & (1 << bitPos));
			bool afterVal = (outputState.reg( regSet ).value & (1 << bitPos));
			if ( beforeVal == afterVal )
			{
// 				cout << "Removing: " << (*it)->code() << endl;
				it.removeAndIncrement();
				return true;
			}
		}
			
		uchar depends = generateRegisterDepends( *it, regSet );
		if ( !(depends & (1 << bitPos)) )
		{
			// Bit is overwritten before being used - so lets remove this instruction :)
// 			cout << "Removing: " << (*it)->code() << endl;
			it.removeAndIncrement();
			return true;
		}
	}
	m_pCode->setAllUnused();
	//END Optimization 5: Remove writes to a bit when the value is ignored and overwritten again
	
	
	//BEGIN Optimization 6: Remove writes to a register when the value is ignored and overwritten again
	// We go through the instructions looking for statements that write to a register (such as MOVLW).
	// If we find any, then we trace through their output links to see if their value is
	// overwritten before it is used - and if so, the instruction can be removed.
	for ( Code::iterator it = m_pCode->begin(); it != end; ++it )
	{
		bool noFile = false;
		
		switch ( (*it)->assemblyType() )
		{
			case Instruction::WorkingOriented:
				noFile = true;
				// (no break)
				
			case Instruction::FileOriented:
				break;
				
			case Instruction::BitOriented:
			case Instruction::Other:
			case Instruction::None:
				continue;
		}
		
		const Register regSet = noFile ? Register( Register::WORKING ) : (*it)->outputReg();
		
		if ( regSet.affectsExternal() )
			continue;
		
		ProcessorState inputState = (*it)->inputState();
		ProcessorState outputState = (*it)->outputState();
		ProcessorBehaviour behaviour = (*it)->behaviour();
		
		// All ins_file instructions will affect at most two registers; the
		// register it is writing to (regSet) and the status register.
		// In i==0, test regSet
		// In i==1, test STATUS
		bool ok = true;
		for ( unsigned i = 0; i < 2; ++ i)
		{
			// If we are testing STATUS, then we assume that the bits changed
			// are only those that are marked as independent.
			uchar bitmask = ( i == 1 ) ? behaviour.reg( Register::STATUS ).indep : 0xff;
			if ( !canRemove( *it, (i == 0) ? regSet : Register::STATUS, bitmask ) )
			{
				ok = false;
				break;
			}
		}
			
		if ( !ok )
			continue;
		
		// Looks like we're free to remove the instruction :);
// 		cout << "Removing: " << (*it)->code() << endl;
		it.removeAndIncrement();
		return true;
	}
	m_pCode->setAllUnused();
	//END Optimization 6: Remove writes to a register when the value is ignored and overwritten again
	
	return false;
}


bool Optimizer::redirectGotos( Instruction * current, const QString & label )
{
	if ( current->isUsed() )
		return false;
	
	current->setUsed( true );
	
	bool changed = false;
	
	const InstructionList list = current->inputLinks();
	InstructionList::const_iterator end = list.end();
	for ( InstructionList::const_iterator it = list.begin(); it != end; ++it )
	{
		Instr_goto * gotoIns = dynamic_cast<Instr_goto*>(*it);
		if ( !gotoIns || (gotoIns->label() == label) )
			continue;
				
// 		cout << "Redirecting goto to label \"" << label << "\" : " << gotoIns->code() << endl;
		gotoIns->setLabel( label );
		changed = true;
	}
	
	return changed;
}


uchar Optimizer::generateRegisterDepends( Instruction * current, const Register & reg )
{
	m_pCode->setAllUnused();
	
	const InstructionList list = current->outputLinks();
	InstructionList::const_iterator listEnd = list.end();
	
	uchar depends = 0x0;
	
	for ( InstructionList::const_iterator listIt = list.begin(); listIt != listEnd; ++listIt )
		depends |= registerDepends( *listIt, reg );
	
	return depends;
}


uchar Optimizer::registerDepends( Instruction * current, const Register & reg )
{
	if ( current->isUsed() )
		return current->registerDepends( reg );
	
	current->setUsed( true );
	
	uchar depends = 0x0;
	
	const InstructionList list = current->outputLinks();
	InstructionList::const_iterator end = list.end();
	for ( InstructionList::const_iterator it = list.begin(); it != end; ++it )
		depends |= registerDepends( *it, reg );
	
	RegisterBehaviour behaviour = current->behaviour().reg( reg );
	depends &= ~(behaviour.indep); // Get rid of depend bits that are set in this instruction
	depends |= behaviour.depends; // And add the ones that are dependent in this instruction
	
	current->setRegisterDepends( depends, reg );
	return depends;
}


bool Optimizer::canRemove( Instruction * ins, const Register & reg, uchar bitMask )
{
	// The bits that are depended upon in the future for this register
	uchar depends = generateRegisterDepends( ins, reg );
	
	// Only interested in those bits allowed by the bit mask
	depends &= bitMask;
	
	RegisterState inputState = ins->inputState().reg( reg );
	RegisterState outputState = ins->outputState().reg( reg );
	
	if ( inputState.unknown() & depends )
	{
		// There's at least one bit whose value is depended on, but is not known before this
		// instruction is executed. Therefore, it is not safe to remove this instruction.
		return false;
	}
	
	if ( outputState.unknown() & depends )
	{
		// There's at least one bit whose value is depended on, but is not known after this
		// instruction is executed. Therefore, it is not safe to remove this instruction.
		return false;
	}
			
	uchar dependsInput = inputState.value & depends;
	uchar dependsOutput = outputState.value & depends;
	if ( dependsInput != dependsOutput )
	{
		// At least one bit whose value is depended upon was changed.
		return false;
	}
	
	return true;
}

