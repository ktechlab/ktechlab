/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ASMINFO_H
#define ASMINFO_H

#include <QString>
#include <QStringList>
#include <QList>

/**
@author David Saxton
*/
struct Instruction
{
	QString operand;
	QString description;
	QString opcode;
};

typedef QList<Instruction> InstructionList;

/**
@short Base class for all instruction sets
@author David Saxton
*/
class AsmInfo
{
public:
	AsmInfo();
	virtual ~AsmInfo();

	enum Set
	{
		PIC12	= 1 << 0,
		PIC14	= 1 << 1,
		PIC16	= 1 << 2
	};
	enum { AsmSetAll = PIC12 | PIC14 | PIC16 };

	static QString setToString( Set set );
	static Set stringToSet( const QString & set );

	/**
	 * @return the instruction set in use
	 */
	virtual Set set() const = 0;
	/**
	 * Returns a list of instruction operands (all uppercase).
	 */
	QStringList operandList() const { return m_operandList; }
	/**
	 * @param operand is the name of the instruction - eg 'addwf' or 'clrwdt'.
	 * @param description is instruction description - eg 'Add W and f'.
	 * @param opcode is the 14-bit code for the instruction, eg
	 * '000111dfffffff'for addwf.
	 */
	void addInstruction( const QString &operand, const QString &description, const QString &opcode );
    void addInstruction( const char* operand, const char* description, const char* opcode ) {
        addInstruction(QString::fromLatin1(operand),QString::fromLatin1(description), QString::fromLatin1(opcode));
    }

private:
	InstructionList m_instructionList;
	QStringList m_operandList;
};

#endif
