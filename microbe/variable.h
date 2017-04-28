/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke   daniel.jc@gmail.com        *
 *   Copyright (C)      2005 by David Saxton                               *
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

#ifndef VARIABLE_H
#define VARIABLE_H

#include <qstring.h>
#include <qlist.h>

class PortPin;
typedef QList<PortPin> PortPinList;


/**
@author Daniel Clarke
@author David Saxton
*/
class Variable
{
	public:
		enum VariableType
		{
			charType, // 8 bit file register
			sevenSegmentType, // A pin configuration for a seven segment is represented by a write-only variable.
			keypadType, // A pin configuration for a keypad has 4 rows and n columns (typically n = 3 or 4) - a read-only variable
			invalidType
		};
		
		Variable( VariableType type, const QString & name );
		Variable();
		~Variable();

		VariableType type() const { return m_type; }
		QString name() const { return m_name; }
		
		/**
		 * @returns whether the variable can be read from (e.g. the seven
		 * segment variable cannot).
		 */
		bool isReadable() const;
		/**
		 * @returns whether the variable can be written to (e.g. the keypad
		 * variable cannot).
		 */
		bool isWritable() const;
		/**
		 * @see portPinList
		 */
		void setPortPinList( const PortPinList & portPinList );
		/**
		 * Used in seven-segments and keypads,
		 */
		PortPinList portPinList() const { return m_portPinList; }
		
	protected:
		VariableType m_type;
		QString m_name;
		PortPinList m_portPinList;
};
typedef QList<Variable> VariableList;

#endif
