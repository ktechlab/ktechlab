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

#include "traverser.h"
#include "pic14.h"

Traverser::Traverser(BTreeNode *root)
{
	m_root = root;
	m_current = root;
}

Traverser::~Traverser()
{
}

BTreeNode * Traverser::start()
{
	/* To find the start we will iterate, or possibly recurse
	down the tree, each time turning down the node that has children,
	if they both have no children we have reached the end and it shouldn't
	really matter which we pick (check this algorithm) */
	
	BTreeNode *n = m_root;
	bool found = false;
	
	while(!found)
	{
		if( !n->hasChildren()) found = true;
		else
		{
			if( !n->left()->hasChildren())
			{
				if( !n->right()->hasChildren()) found = true;
				n = n->right();
			}
			else n = n->left();
		}
	}
	//if(n->parent()) m_current = n->parent();
	//else m_current = n;
	m_current = n;
	return m_current;
}

BTreeNode * Traverser::next()
{
	// a.t.m we will just take the next thing to be the parent.
	if( m_current != m_root) m_current = m_current->parent();
	return m_current;
}

bool Traverser::onLeftBranch()
{
	return current()->parent()->left() == current();
}

BTreeNode * Traverser::oppositeNode()
{
	if( onLeftBranch())
		return current()->parent()->right();
	else
		return current()->parent()->left();
}

void Traverser::descendLeftwardToTerminal()
{
	bool done = false;
	while(!done)
	{
		if( !current()->hasChildren()) return;
		else
		{
			m_current = current()->left();
		}
	}
}

void Traverser::moveToParent()
{
	if(current()->parent()) m_current = current()->parent();
}

