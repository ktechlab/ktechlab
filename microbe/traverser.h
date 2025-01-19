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
#ifndef TRAVERSER_H
#define TRAVERSER_H

#include "btreenode.h"

/**
Keeps persistent information needed and the algorithm for traversing the binary trees made of BTreeNodes, initialise either by passing a BTreeBase or BTreeNode to traverse a sub tree.

Note that this is designed for traversing in the *reverse* way starting at the end of each branch
in order to calculate the expression contained in the tree.

@author Daniel Clarke
*/
class Traverser
{
public:
	Traverser(BTreeNode *root);
	~Traverser();
	
	/** Find where to start in the tree and return it also resets all the data related to the traversal. */
	BTreeNode *start();
	
	/** Finds the next node to move to and returns it. */
	BTreeNode *next();
	
	/** Returns true if we are on the left branch, false otherwise. */
	bool onLeftBranch();
	
	/** Returns the node on the opposite branch of the parent. */
	BTreeNode * oppositeNode();
	
	BTreeNode * current() const { return m_current; }

	void setCurrent(BTreeNode *current){m_current = current;}
	
	/** From the current position, go down the tree taking a left turn always, 
	 and stopping when reaching the left terminal node.
	 */
	void descendLeftwardToTerminal();
	
	/** It might occur in the future that next() does not just move to the parent,
	 so use this for moving to parent
	 */
	void moveToParent();
	
	BTreeNode *root() const {return m_root;}
	
protected:
	BTreeNode *m_root;
	BTreeNode *m_current;
};

#endif
