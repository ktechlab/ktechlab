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

#include "pic14.h"
#include "variable.h"

Variable::Variable( VariableType type, const QString & name )
{
	m_type = type;
	m_name = name;
}


Variable::Variable()
{
	m_type = invalidType;
}


Variable::~Variable()
{
}


void Variable::setPortPinList( const PortPinList & portPinList )
{
	m_portPinList = portPinList;
}


bool Variable::isReadable() const
{
	switch (m_type)
	{
		case charType:
		case keypadType:
			return true;
		case sevenSegmentType:
		case invalidType:
			return false;
	}
	
	return false;
}


bool Variable::isWritable() const
{
	switch (m_type)
	{
		case charType:
		case sevenSegmentType:
			return true;
		case keypadType:
		case invalidType:
			return false;
	}
	
	return false;
}


