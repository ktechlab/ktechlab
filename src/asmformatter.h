/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ASMFORMATTER_H
#define ASMFORMATTER_H

#include <QStringList>

/**
@author David Saxton
 */
class InstructionParts
{
	public:
		/**
		 * Breaks up the line into parts.
		 */
		InstructionParts( QString line );
		
		QString label() const { return m_label; }
		QString operand() const { return m_operand; }
		QString operandData() const { return m_operandData; }
		QString comment() const { return m_comment; }
		
	protected:
		QString m_label;
		QString m_operand;
		QString m_operandData;
		QString m_comment; ///< includes the ";" part
};

/**
@author David Saxton
*/
class AsmFormatter
{
	public:
		AsmFormatter();
		~AsmFormatter();
	
		enum LineType
		{
			Equ,
			Instruction, // could include label
			Other // eg comments, __config
		};
	
		QString tidyAsm( QStringList lines );
	
		static LineType lineType( QString line );
	
	protected:
		QString tidyInstruction( const QString & line );
		QString tidyEqu( const QString & line );
		/**
		 * Appends spaces to the end of text until it is greater or equakl to
		 * length.
		 */
		static void pad( QString & text, int length );
	
		int m_indentAsmName;
		int m_indentAsmData;
		int m_indentEqu;
		int m_indentEquValue;
		int m_indentEquComment;
		int m_indentComment;
};

#endif
