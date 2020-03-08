/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke                              *
 *   daniel.jc@gmail.com                                               *
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

#include "btreenode.h"
#include "pic14.h"

BTreeNode::BTreeNode()
{
	m_parent = nullptr;
	m_left = nullptr;
	m_right = nullptr;
	m_type = unset;
}

BTreeNode::BTreeNode(BTreeNode *p, BTreeNode *l, BTreeNode *r)
{
	m_parent = p;
	m_left = l;
	m_right = r;
}

BTreeNode::~BTreeNode()
{
	// Must not delete children as might be unlinking!!! deleteChildren();
}

void BTreeNode::deleteChildren()
{
	if(m_left)
	{
		m_left->deleteChildren();
		delete m_left;
	}
	if(m_right)
	{
		m_right->deleteChildren();
		delete m_right;
	}
	
	m_left = nullptr;
	m_right = nullptr;
	
	return;
}

// void BTreeNode::printTree()
// {
// 	
// }
