/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke                              *
 *   daniel.jc@gmail.com                                                   *
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
#ifndef BTREEBASE_H
#define BTREEBASE_H
#include "microbe.h"
#include "btreenode.h"

/**
@short This holds a pointer to the start of the tree, and provides the traversal code.
@author Daniel Clarke
*/
class BTreeBase{
public:
    BTreeBase();
    ~BTreeBase();
    
    /** Return a pointer to the root node of the tree */
    BTreeNode *root() const { return m_root; }
    
    /** Set the root node of the tree */
    void setRoot(BTreeNode *root){m_root = root; }
    
    /** Link the node into the tree. a.t.m all this really
    does it sets the parent/child relationship pointers, 
    but is used in case something needs to be changed in the future 
    Added to the left if left == true or the right if left == false */
    void addNode(BTreeNode *parent, BTreeNode *node, bool left);

    /** Deletes all nodes in tree and zeros pointer to root node */
    void deleteTree();
    
	/** Tidies the tree up; merging constants and removing redundant branches */
    void pruneTree(BTreeNode *root, bool conditionalRoot = true);
    
    /** Put a node in place of another, linking it correctly into the parent. */
    void replaceNode(BTreeNode *node, BTreeNode *replacement);
    
protected:
    BTreeNode *m_root;    
};

#endif
