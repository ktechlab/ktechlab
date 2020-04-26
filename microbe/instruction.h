/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke <daniel.jc@gmail.com>        *
 *                      2005 by David Saxton <david@bluehaze.org>          *
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

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QList>

class Code;
class CodeIterator;
class CodeConstIterator;
class Instruction;
class PIC14;

typedef QList<Instruction*> InstructionList;


/**
Abstraction for a Register - should be used instead of a register name. Contains
info like whether or not the adressing of the register depends on the bank
selection.

@author David Saxton
*/
class Register
{
	public:
		enum Type
		{
			TMR0,
			OPTION_REG,
			PCL,
			STATUS,
			FSR,
			PORTA,
			TRISA,
			PORTB,
			TRISB,
			EEDATA,
			EECON1,
			EEADR,
			EECON2,
			PCLATH,
			INTCON,
//modification start
			PORTC,
			PORTD,
			PORTE,
			TRISC,
			TRISD,
			TRISE,
			ADCON0,
			ADCON1,
//modification end
			// The following three are "special"
			WORKING, // Not a register that is addressable by an address
			GPR, // Refers to the collection of General Purpose Registers
//modification start
			PIR1,
			PIR2,
			TMR1L,
			TMR1H,
			T1CON,
			TMR2,
			T2CON,
			RCSTA,
			TXREG,
			RCREG,
			ADRESH,
			PIE1,
			TXSTA,
			ADRESL,
			EEDATH,
			EEADRH,
			SSPBUF,
			SSPCON,
			CCPR1L,
			CCPR1H,
			CCP1CON,
			CCPR2L,
			CCPR2H,
			CCP2CON,
			PIE2,
			PCON,
			SSPCON2,
			PR2,
			SSPADD,
			SSPSTAT,
			SPBRG,
			VRCON,
			CMCON,

//modification end
			none // used in default constructor
//TODO
//SSPBUF:SSPCON:CCPR1L:CCPR1H:CCP1CON:CCPR2L:CCPR2H:CCP2CON:--FOR BANK0
//PIE2:PCON:SSPCON2:PR2:SSPADD:SSPSTAT:SPBRG:--------FOR BANK1
		};

		// These banks are used for ORing together in the banks() function
		enum Banks
		{
			Bank0 = 1 << 0,
			Bank1 = 1 << 1
		};

		/**
		 * Creates a register of the given type, giving it the appropriate name.
		 * Note that this constructor should not be used for GPR.
		 */
		Register( Type type = none );
		/**
		 * Construct a Register with the given name. If the name is not
		 * recognized, then it is assumed to be a GPR register.
		 */
		Register( const QString & name );
		/**
		 * Construct a Register with the given name. If the name is not
		 * recognized, then it is assumed to be a GPR register.
		 */
		Register( const char * name );
		/**
		 * @return less-than-equality between registers; name is only compared
		 * if both registers have type GPR.
		 */
		bool operator < ( const Register & reg ) const;
		/**
		 * @return equality between registers; name is only compared if both
		 * registers have type GPR.
		 */
		bool operator == ( const Register & reg ) const;
		/**
		 * @return 0x1 and 0x2 for being addressable from banks 0 and 1
		 * respectively, OR'ed together.
		 */
		uchar banks() const;
		/**
		 * Convenience function.
		 * @see banks
		 */
		bool bankDependent() const;
		/**
		 * Returns the name of the register, or the alias for the GPR.
		 */
		QString name() const { return m_name; }
		/**
		 * @return the type of register.
		 */
		Type type() const { return m_type; }
		/**
		 * From the Optimizer's perspective, it is OK to remove, change or add
		 * any instruction so long as there are no visible external changes that
		 * go against the original intention of the microbe source (a general
		 * guiding principle). Therefore, this function returns true for PORT
		 * and TRIS registers, false for everything else.
		 */
		bool affectsExternal() const;

	protected:
		QString m_name;
		Type m_type;
};



class RegisterBit
{
	public:
		enum STATUS_bits
		{
			C			= 0, // Carry
			DC			= 1, // Digit carry
			Z			= 2, // Zero
			NOT_PD		= 3, // Power-down
			NOT_TO		= 4, // Time-out
			RP0			= 5, // Bank Select
			RP1			= 6,
			IRP			= 7
		};

		enum INTCON_bits
		{
			RBIF		= 0,
			INTF		= 1,
			T0IF		= 2,
			RBIE		= 3,
			INTE		= 4,
			T0IE		= 5,
			EEIE		= 6,
			GIE		= 7
		};

		enum OPTION_bits
		{
			PS0		= 0,
			PS1		= 1,
			PS2		= 2,
			PSA		= 3,
			T0SE		= 4,
			T0CS		= 5,
			INTEDG		= 6,
			NOT_RBPU	= 7
		};

		enum EECON1_bits
		{
			RD		= 0,
			WR		= 1,
			WREN		= 2,
			WRERR		= 3,
			EEIF		= 4,
			EEPGD           = 7
		};
		/**
		 * Constructs a bit of the given register type at the given position.
		 */
		RegisterBit( uchar bitPos = 0, Register::Type reg = Register::none );
		/**
		 * Construct a register bit with the given name.
		 */
		RegisterBit( const QString & name );
		/**
		 * Construct a register bit with the given name.
		 */
		RegisterBit( const char * name );
		/**
		 * @warning do not trust this value! actually, this function should be
		 * removed, or the constructors fixed so that this value can be trusted.
		 * @return the register type that the bit belongs to.
		 */
		Register::Type registerType() const { return m_registerType; }
		/**
		 * @return the position of the bit, e.g. "5" for RP0.
		 */
		uchar bitPos() const { return m_bitPos; }
		/**
		 * @return the bit, e.g. "0x20" for Z.
		 */
		uchar bit() const { return (1 << m_bitPos); }
		/**
		 * @return the name of the bit, e.g. "Z" for Z.
		 */
		QString name() const { return m_name; }


	protected:
		/**
		 * Determines the register type and bit pos from the bit name (m_name).
		 */
		void initFromName();

		Register::Type m_registerType;
		uchar m_bitPos:3;
		QString m_name;
};




/**
Contains information on the state of a register before an instruction is
executed.

Note that all the "uchar" values in this class should be considered as the 8
bits of a register. So for example, if known=0x2, then only the second bit of
the register is known, and its value is given in the second bit of value.

@author David Saxton
*/
class RegisterState
{
	public:
		RegisterState();

		/**
		 * Merges the known and values together, (possibly) reducing what is
		 * known.
		 */
		void merge( const RegisterState & state );
		/**
		 * Sets known to unknown and value to zero.
		 */
		void reset();
		/**
		 * Returns the bits that are definitely zero.
		 */
		uchar definiteZeros() const { return (~value) & known; }
		/**
		 * Returns the bits that are definitely one.
		 */
		uchar definiteOnes() const { return value & known; }
		/**
		 * Returns the bits that are unknown.
		 */
		uchar unknown() const { return ~known; }
		/**
		 * @return the largest possible value that this register might be
		 * storing, based on which bits are known and the value of those bits.
		 */
		uchar maxValue() const { return (value & known) | (~known); }
		/**
		 * @return the smallest possible value that this register might be
		 * storing, based on which bits are known and the value of those bits.
		 */
		uchar minValue() const { return (value & known); }
		/**
		 * @return whether the known and value uchars are equal
		 */
		bool operator == ( const RegisterState & state ) const;
		/**
		 * @return whether either of the known and value uchars are not equal.
		 */
		bool operator != ( const RegisterState & state ) const { return !( *this == state ); }
		/**
		 * Prints known and value.
		 */
		void print();

		/// Whether or not the value is known (for each bit).
		uchar known;

		/// The value of the register.
		uchar value;
};


/**
Setting and dependency information for register bits. See the respective member
descriptions for more information.

@author David Saxton
*/
class RegisterBehaviour
{
	public:
		RegisterBehaviour();
		/**
		 * Sets "depends", "indep" and "changes" to 0x0.
		 */
		void reset();

		/**
		 * The bits whose value before the instruction is executed will affect
		 * the processor state after execution. So for example,
		 *   in MOVLW this will be 0x0;
		 *   in ANDLW this will be the bits that are non-zero in the literal;
		 *   in BTFSC this will be the bit being tested (if this is the register
		 *      being tested).
		 */
		uchar depends;

		/**
		 * The bits whose value after the instruction is executed is independent
		 * of the value before execution. So for example,
		 *   in MOVLW, this will be 0xff;
		 *   in ANDLW this will be the bits that are zero in the literal;
		 *   in BTFSC this will be 0x0.
		 */
		uchar indep;
};



/**
Contains information on the state of a processor; e.g. register values

@author David Saxton
 */
class ProcessorState
{
	public:
		ProcessorState();
		/**
		 * Calls merge for each RegisterState.
		 */
		void merge( const ProcessorState & state );
		/**
		 * Calls reset() for each RegisterState.
		 */
		void reset();
		/**
		 * @return state for the given register.
		 */
		RegisterState & reg( const Register & reg );
		/**
		 * @return state for the given register.
		 */
		RegisterState reg( const Register & reg ) const;
		/**
		 * @return whether all the RegisterStates are identical
		 */
		bool operator == ( const ProcessorState & state ) const;
		/**
		 * @return whether any of the RegisterStates are not equal.
		 */
		bool operator != ( const ProcessorState & state ) const { return !( *this == state ); }
		/**
		 * Displays each register's name and calls RegisterState::print in turn.
		 */
		void print();

		/// The working register
		RegisterState working;

		/// The status register
		RegisterState status;

	protected:
		typedef QMap< Register, RegisterState > RegisterMap;
		/**
		 * All registers other than working and status. Entries are created on
		 * calls to reg with a new Register.
		 */
		RegisterMap m_registers;
};


/**
Contains behavioural information for each register.

@author David Saxton
*/
class ProcessorBehaviour
{
	public:
		ProcessorBehaviour();
		/**
		 * Calls reset() for each RegisterBehaviour.
		 */
		void reset();
		/**
		 * @return behaviour for the given register.
		 */
		RegisterBehaviour & reg( const Register & reg );

		/// The working register
		RegisterBehaviour working;

		/// The status register
		RegisterBehaviour status;

	protected:
		typedef QMap< Register, RegisterBehaviour > RegisterMap;
		/**
		 * All registers other than working and status. Entries are created on
		 * calls to reg with a new Register.
		 */
		RegisterMap m_registers;
};


/**
Contains information on whether a register is overwritten before its value is
used. Each uchar respresents the 8 bits of the register; if the bit is 1, then
the corresponding bit of the register is used by the Instruction or one
of its outputs before it is overwritten.

@author David Saxton
*/
class RegisterDepends
{
	public:
		RegisterDepends();
		/**
		 * Sets all the depends values to 0x0.
		 */
		void reset();
		/**
		 * @return behaviour for the given register.
		 */
		uchar & reg( const Register & reg );

		/// The working register
		uchar working;

		/// The status register
		uchar status;

	protected:
		typedef QMap< Register, uchar > RegisterMap;
		/**
		 * All registers other than working and status. Entries are created on
		 * calls to reg with a new Register.
		 */
		RegisterMap m_registers;
};



/**
Holds a program structure; an (ordered) list of blocks of code, each of which
contains a list of instructions. The structure is such as to provide easy
manipulation of the program, as well as aiding the optimizer.

@author David Saxton
*/
class Code
{
	public:
		Code();

		typedef CodeIterator iterator;
		typedef CodeConstIterator const_iterator;

		enum InstructionPosition
		{
			InterruptHandler	= 0,
			LookupTable			= 1,
			Middle				= 2, ///< Used for main code
			Subroutine			= 3, ///< Used for subroutines

			PositionCount		= 4 ///< This must remain the last item and be the number of valid positions
		};

		CodeIterator begin();
		CodeIterator end();
		CodeConstIterator begin() const;
		CodeConstIterator end() const;

		/**
		 * Queues a label to be given to the next instruction to be added in the
		 * given position
		 */
		void queueLabel( const QString & label, InstructionPosition position = Middle );
		/**
		 * Returns the list of queued labels for the given position. This is
		 * used in merging code, as we also need to merge any queued labels.
		 */
		QStringList queuedLabels( InstructionPosition position ) const { return m_queuedLabels[position]; }
		/**
		 * Adds the Instruction at the given position.
		 */
		void append( Instruction * instruction, InstructionPosition position = Middle );
		/**
		 * @returns the Instruction with the given label (or null if no such
		 * Instruction).
		 */
		Instruction * instruction( const QString & label ) const;
		/**
		 * Look for an Assembly instruction (other types are ignored).
		 * @return an iterator to the current instruction, or end if it wasn't
		 * found.
		 */
		iterator find( Instruction * instruction );
		/**
		 * Removes the Instruction (regardless of position).
		 * @warning You should always use only this function to remove an
		 * instruction as this function handles stuff such as pushing labels
		 * from this instruction onto the next before deletion.
		 */
		void removeInstruction( Instruction * instruction );
		/**
		 * Merges all the blocks output together with other magic such as adding
		 * variables, gpasm directives, etc.
		 */
		QString generateCode( PIC14 * pic ) const;
		/**
		 * Appends the InstructionLists to the end of the ones in this instance.
		 * @param middleInsertionPosition is the position where the middle code
		 * blocks of the given code will be merged at.
		 */
		void merge( Code * code, InstructionPosition middleInsertionPosition = Middle );
		/**
		 * @returns the InstructionList for the given insertion position.
		 */
		InstructionList * instructionList( InstructionPosition position ) { return & m_instructionLists[position]; }
		/**
		 * @returns the InstructionList for the given insertion position.
		 */
		const InstructionList * instructionList( InstructionPosition position ) const { return & m_instructionLists[position]; }
		/**
		 * Calls generateOutputLinks for each Instruction
		 */
		void generateLinksAndStates();
		/**
		 * Calls setUsed(false) for all instructions.
		 */
		void setAllUnused();
		/**
		 * Does any work that is needed to the code before it can be passed to
		 * the optimizer (such as flushing out queued labels). This is called
		 * after all the instructions have been added to the code.
		 */
		void postCompileConstruct();

	protected:
		/**
		 * Used when generating the code. Finds the list of general purpose
		 * registers that are referenced and returns their aliases.
		 */
		QStringList findVariables() const;

		InstructionList m_instructionLists[ PositionCount ]; ///< @see InstructionPosition
		QStringList m_queuedLabels[ PositionCount ]; ///< @see InstructionPosition

	private: // Disable copy constructor and operator=
		Code( const Code & );
		Code &operator=( const Code & );
};


/**
Iterates over all the instructions, going seamlessly between the different lists
and avoiding the non-assembly instructions.

@author David Saxton
 */
class CodeIterator
{
	public:
		bool operator != ( const CodeIterator & i ) const { return it != i.it; }
		bool operator == ( const CodeIterator & i ) const { return it == i.it; }
		CodeIterator & operator ++ ();
		Instruction * & operator * () { return *it; }
		/**
		 * Deletes the instruction that this iterator is currently pointing at
		 * (removing it from any lists), and increments the iterator to the next
		 * instruction.
		 */
		CodeIterator & removeAndIncrement();
		/**
		 * Inserts the given instruction before the instruction pointed at by
		 * this iterator.
		 */
		void insertBefore( Instruction * ins );

		InstructionList::iterator it;
		InstructionList::iterator listEnd;
		Code::InstructionPosition pos;
		Code * code;
		InstructionList * list;
};


/**
A const version of CodeIterator (cannot change instructions).

@author David Saxton
 */
class CodeConstIterator
{
	public:
		bool operator != ( const CodeConstIterator & i ) const { return it != i.it; }
		bool operator == ( const CodeConstIterator & i ) const { return it == i.it; }
		CodeConstIterator & operator ++ ();
		const Instruction * operator * () const { return *it; }

		InstructionList::const_iterator it;
		InstructionList::const_iterator listEnd;
		Code::InstructionPosition pos;
		const Code * code;
		const InstructionList * list;
};


/**
@author Daniel Clarke
@author David Saxton
*/
class Instruction
{
	public:
		enum InstructionType
		{
			Assembly,
			Raw, // User-inserted assembly
			Comment
		};
		/**
		 * Used in optimization. Note that this follows roughly, but not
		 * exactly, the Microchip classifications of similar categories.
		 */
		enum AssemblyType
		{
			/**
			 * Writes to a file (which can be obtained by calling outputReg().
			 */
			FileOriented,

			/**
			 * Writes to a file bit (so BCF or BSF).
			 */
			BitOriented,

			/**
			 * Affects the working register via a literal operation, with no
			 * branching (so excludes retlw).
			 */
			WorkingOriented,

			/**
			 * Assembly instructions that don't come under the above categories
			 * (so control and branching instructions).
			 */
			Other,

			/**
			 * The Instruction is not of Assembly InstructionType.
			 */
			None
		};

		Instruction();
		virtual ~Instruction();
		void setCode( Code * code ) { m_pCode = code; }

		/**
		 * This is used to decide how to output the instruction, and which
		 * instructions to avoid while optimizing.
		 */
		virtual InstructionType type() const { return Assembly; }
		/**
		 * @return the AssemblyType (None for non-Assembly instructions).
		 */
		virtual AssemblyType assemblyType() const = 0;
		/**
		 * The text to output to the generated assembly.
		 */
		virtual QString code() const = 0;
		/**
		 * The input processor state is used to generate the outputlinks and the
		 * output processor state.
		 */
		void setInputState( const ProcessorState & processorState ) { m_inputState = processorState; }
		/**
		 * By using the ProcessorState, the Instruction should:
		 * * Find all instructions that could be executed after this instruction.
		 * * Generate the output ProcessorState.
		 * The default behaviour of this function is to link to the next
		 * sequential instruction, and to generate an unknown ProcessorState.
		 * @warning if your instruction depends on any bits, then it must
		 * reinherit this function and say so.
		 * @param instruction points at this instruction
		 */
		virtual void generateLinksAndStates( Code::iterator instruction );
		/**
		 * @return the processor behaviour for this instruction.
		 */
		virtual ProcessorBehaviour behaviour() const;
		/**
		 * An input link is an instruction that might be executed immediately
		 * before this Instruction.
		 */
		void addInputLink( Instruction * inputLink );
		/**
		 * An output link is an instruction that might be executed immediately
		 * after this Instruction.
		 */
		void addOutputLink( Instruction * inputLink );
		/**
		 * The list of instructions that might be executed immediately before
		 * this instruction.
		 * @see addInputLink
		 */
		InstructionList inputLinks() const { return m_inputLinks; }
		/**
		 * The list of instructions that might be executed immediately after
		 * this instruction. Instruction does not generate these links; instead
		 * the list is generated Code::generateLinksAndStates function.
		 */
		InstructionList outputLinks() const { return m_outputLinks; }
		/**
		 * Remove the given input link from the instruction.
		 */
		void removeInputLink( Instruction * ins );
		/**
		 * Remove the given output link from the instruction.
		 */
		void removeOutputLink( Instruction * ins );
		/**
		 * Clears all input and output links from this instruction. This does
		 * not remove references to this instruction from other instructions.
		 */
		void clearLinks();
		/**
		 * An instruction may have zero, or more than zero labels associated
		 * with it - these will be printed before the instruction in the
		 * assembly output.
		 */
		QStringList labels() const { return m_labels; }
		/**
		 * @see labels
		 */
		void addLabels( const QStringList & labels );
		/**
		 * @see labels
		 */
		void setLabels( const QStringList & labels );
		/**
		 * @see used
		 */
		void setUsed( bool used ) { m_bUsed = used; }
		/**
		 * Used for optimization purposes in determining whether the instruction
		 * has been examined yet (to avoid infinite loops).
		 */
		bool isUsed() const { return m_bUsed; }
		/**
		 * Set by the optimizer to indicate whether this instruction or any of
		 * its outputs overwrite any of the bits of the given register.
		 */
		void setRegisterDepends( uchar depends, const Register & reg ) { m_registerDepends.reg(reg) = depends; }
		/**
		 * @see setOutputsOverwriteWorking
		 */
		uchar registerDepends( const Register & reg ) { return m_registerDepends.reg(reg); }
		/**
		 * Resets the overwrites.
		 */
		void resetRegisterDepends() { m_registerDepends.reset(); }
		/**
		 * @return the input processor state to this instruction.
		 * @see setInputState
		 */
		ProcessorState inputState() const { return m_inputState; }
		/**
		 * @return the output processor state from this instruction.
		 * @see generateLinksAndStates.
		 */
		ProcessorState outputState() const { return m_outputState; }
		/**
		 * Only applicable to Instructions that refer to a file.
		 */
		Register file() const { return m_file; }
		/**
		 * Only applicable to Instructions that refer to a bit (such as BCF).
		 */
		RegisterBit bit() const { return m_bit; }
		/**
		 * Only applicable to instructions that refer to a literal (such as
		 * XORLW).
		 */
		uchar literal() const { return m_literal; }
		/**
		 * Applicable only to instructions that save a result to working or file
		 * depending on the destination bit.
		 */
		Register outputReg() const { return (m_dest == 0) ? Register::WORKING : m_file; }
		/**
		 * Applicable only to instructions that use the destination flag.
		 */
		unsigned dest() const { return m_dest; }

	protected:
		/**
		 * This function is provided for convenience; it creates links to the
		 * first or second instructions after this one, depending on the value
		 * of firstOutput and secondOutput.
		 * @see generateOutputLinks
		 */
		void makeOutputLinks( Code::iterator current, bool firstOutput = true, bool secondOutput = false );
		/**
		 * This function is provided for instructions that jump to a label (i.e.
		 * call and goto).
		 */
		void makeLabelOutputLink( const QString & label );

		RegisterDepends m_registerDepends;
		bool m_bInputStateChanged;
		bool m_bUsed;
		bool m_bPositionAffectsBranching;
		InstructionList m_inputLinks;
		InstructionList m_outputLinks;
		QStringList m_labels;
		Code * m_pCode;

		// Commonly needed member variables for assembly instructions
		Register m_file;
		RegisterBit m_bit;
		QString m_raw; // Used by source code, raw asm, etc
		uchar m_literal;
		unsigned m_dest:1; // is 0 (W) or 1 (file).
		ProcessorState m_inputState;
		ProcessorState m_outputState;

	private: // Disable copy constructor and operator=
		Instruction( const Instruction & );
		Instruction &operator=( const Instruction & );
};



//BEGIN Byte-Oriented File Register Operations
class Instr_addwf : public Instruction
{
	public:
		Instr_addwf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


class Instr_andwf : public Instruction
{
	public:
		Instr_andwf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


class Instr_clrf : public Instruction
{
	public:
		Instr_clrf( const Register & file ) { m_file = file; m_dest = 1; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


//TODO CLRW
//TODO COMF


class Instr_decf : public Instruction
{
	public:
		Instr_decf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


class Instr_decfsz : public Instruction
{
	public:
		Instr_decfsz( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


class Instr_incf : public Instruction
{
	public:
		Instr_incf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


//TODO INCFSZ


class Instr_iorwf : public Instruction
{
	public:
		Instr_iorwf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


class Instr_movf : public Instruction
{
	public:
		Instr_movf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


class Instr_movwf : public Instruction
{
	public:
		Instr_movwf( const Register & file ) { m_file = file; m_dest = 1; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


//TODO NOP


class Instr_rlf : public Instruction
{
	public:
		Instr_rlf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


class Instr_rrf : public Instruction
{
	public:
		Instr_rrf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


class Instr_subwf : public Instruction
{
	public:
		Instr_subwf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


class Instr_swapf : public Instruction
{
	public:
		Instr_swapf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};


class Instr_xorwf : public Instruction
{
	public:
		Instr_xorwf( const Register & file, int dest ) { m_file = file; m_dest = dest; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return FileOriented; }
};
//END Byte-Oriented File Register Operations



//BEGIN Bit-Oriented File Register Operations
class Instr_bcf : public Instruction
{
	public:
		Instr_bcf( const Register & file, const RegisterBit & bit ) { m_file = file; m_bit = bit; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return BitOriented; }
};


class Instr_bsf : public Instruction
{
	public:
		Instr_bsf( const Register & file, const RegisterBit & bit ) { m_file = file; m_bit = bit; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return BitOriented; }
};


class Instr_btfsc : public Instruction
{
	public:
		Instr_btfsc( const Register & file, const RegisterBit & bit ) { m_file = file; m_bit = bit; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return Other; }
};


class Instr_btfss : public Instruction
{
	public:
		Instr_btfss( const Register & file, const RegisterBit & bit ) { m_file = file; m_bit = bit; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return Other; }
};
//END Bit-Oriented File Register Operations



//BEGIN Literal and Control Operations
class Instr_addlw : public Instruction
{
	public:
		Instr_addlw( int literal ) { m_literal = literal; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return WorkingOriented; }
};



class Instr_andlw : public Instruction
{
	public:
		Instr_andlw( int literal ) { m_literal = literal; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return WorkingOriented; }
};


class Instr_call : public Instruction
{
	public:
		Instr_call( const QString & label ) { m_label = label; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return Other; }
		/**
		 * Called from Code after all the output links have been generated. The
		 * instruction that is called has its output links followed, and any
		 * returns encountered are linked back to the instruction after this
		 * one.
		 * @param next the instruction after this one which the return points
		 * will be linked to.
		 */
		void makeReturnLinks( Instruction * next );

		QString label() const { return m_label; }
		void setLabel( const QString & label ) { m_label = label; }

	protected:
		/**
		 * Used by makeReturnLinks. Recursively follows the instruction's output
		 * links, until a return is found - then, link the return point back to
		 * the instruction after this one. Call instructions found while
		 * following the output are ignored.
		 * @param returnPoint the instruction to link back to on finding a
		 * return.
		 */
		void linkReturns( Instruction * current, Instruction * returnPoint );

		QString m_label;
};


//TODO CLRWDT


class Instr_goto : public Instruction
{
	public:
		Instr_goto( const QString & label ) { m_label = label; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return Other; }
		
		QString label() const { return m_label; }
		void setLabel( const QString & label ) { m_label = label; }

	protected:
		QString m_label;
};


class Instr_iorlw : public Instruction
{
	public:
		Instr_iorlw( int literal ) { m_literal = literal; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return WorkingOriented; }
};


class Instr_movlw : public Instruction
{
	public:
		Instr_movlw( int literal ) { m_literal = literal; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return WorkingOriented; }
};


class Instr_retfie : public Instruction
{
	public:
		Instr_retfie() {};
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return Other; }
};


class Instr_retlw : public Instruction
{
	public:
		Instr_retlw( int literal ) { m_literal = literal; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return Other; }
};


class Instr_return : public Instruction
{
	public:
		Instr_return() {};
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return Other; }
};


class Instr_sleep : public Instruction
{
	public:
		Instr_sleep() {};
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return Other; }
};


class Instr_sublw : public Instruction
{
	public:
		Instr_sublw( int literal ) { m_literal = literal; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return WorkingOriented; }
};


class Instr_xorlw : public Instruction
{
	public:
		Instr_xorlw( int literal ) { m_literal = literal; }
		QString code() const override;
		void generateLinksAndStates( Code::iterator current ) override;
		ProcessorBehaviour behaviour() const override;
		AssemblyType assemblyType() const override { return WorkingOriented; }
};
//END Literal and Control Operations



//BEGIN Microbe (non-assembly) Operations
class Instr_sourceCode : public Instruction
{
	public:
		Instr_sourceCode( const QString & source ) { m_raw = source; }
		QString code() const override;
		InstructionType type() const override { return Comment; }
		AssemblyType assemblyType() const override { return None; }
};


class Instr_asm : public Instruction
{
	public:
		Instr_asm( const QString & raw ) { m_raw = raw; }
		QString code() const override;
		InstructionType type() const override { return Raw; }
		AssemblyType assemblyType() const override { return None; }
};


// Like Instr_asm, but does not put ;asm {} in, used
// for internal things like gpasm directives etc...
class Instr_raw : public Instruction
{
	public:
		Instr_raw( const QString & raw ) { m_raw = raw; }
		QString code() const override;
		InstructionType type() const override { return Raw; }
		AssemblyType assemblyType() const override { return None; }
};
//END Microbe (non-assembly) Operations



#endif
