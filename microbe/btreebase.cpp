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

#include "btreebase.h"
#include "traverser.h"
#include "parser.h"
#include "pic14.h"

BTreeBase::BTreeBase()
{
	m_root = nullptr;
}

void BTreeBase::deleteTree()
{
	if(m_root) m_root->deleteChildren();
	delete m_root;
	m_root = nullptr;
}

BTreeBase::~BTreeBase()
{
	deleteTree();
}
 

void BTreeBase::addNode(BTreeNode *parent, BTreeNode *node, bool left)
{
	// Debugging lines, remove when expression parsing has been completed.
	//if(!parent) cerr<<"Null parent pointer!\n";
	//if(!node) cerr<<"Null node pointer!\n");
	
	if(left) parent->setLeft(node);
	else parent->setRight(node);
}

void BTreeBase::pruneTree(BTreeNode *root, bool /*conditionalRoot*/)
{
	Traverser t(root);
	
	t.descendLeftwardToTerminal();
	bool done = false;
	while(!done)
	{
	//t.descendLeftwardToTerminal();
	if( t.current()->parent() )
	{
		if( t.oppositeNode()->hasChildren() ) pruneTree(t.oppositeNode());
	}
	
	t.moveToParent();
	if( !t.current()->hasChildren() )
	{
		//if(t.current() == t.root()) done = true;
		if(!t.current()->parent()) done = true;
		continue;
	}

	BTreeNode *l = t.current()->left();
	BTreeNode *r = t.current()->right();
	BTreeNode *n = nullptr;
	BTreeNode *z = nullptr;
	

	// Deal with situations where there are two constants so we want
	// to evaluate at compile time
	if( (l->type() == number && r->type() == number) ) // && !(t.current()==root&&conditionalRoot) )
	{
		if(t.current()->childOp() == Expression::division && r->value() == "0" ) 
		{
			t.current()->setChildOp(Expression::divbyzero);
			return;
		}
		QString value = QString::number(Parser::doArithmetic(l->value().toInt(),r->value().toInt(),t.current()->childOp()));
		t.current()->deleteChildren();
		t.current()->setChildOp(Expression::noop);
		t.current()->setType(number);
		t.current()->setValue(value);
	}
	
	// Addition and subtraction
	else if(t.current()->childOp() == Expression::addition || t.current()->childOp() == Expression::subtraction)
	{
	// See if one of the nodes is 0, and set n to the node that actually has data,
	// z to the one containing zero.
	bool zero = false;
	if( l->value() == "0" )
	{
		zero = true;
		n = r;
		z = l;
	}
	else if( r->value() == "0" )
	{
		zero = true;
		n = l;
		z = r;
	}
	// Now get rid of the useless nodes
	if(zero)
	{
		BTreeNode *p = t.current(); // save in order to delete after

		replaceNode(p,n);
		t.setCurrent(n);
		// Delete the old nodes
		delete p;
		delete z;
	}
	}
	
	// Multiplication and division
	else if(t.current()->childOp() == Expression::multiplication || t.current()->childOp() == Expression::division)
	{
	// See if one of the nodes is 0, and set n to the node that actually has data,
	// z to the one containing zero.
	bool zero = false;
	bool one = false;
	if( l->value() == "1" )
	{
		one = true;
		n = r;
		z = l;
	}
	else if( r->value() == "1" )
	{
		one = true;
		n = l;
		z = r;
	}
	if( l->value() == "0" )
	{
		zero = true;
		n = r;
		z = l;
	}
	else if( r->value() == "0" )
	{
		
		// since we can't call compileError from in this class, we have a special way of handling it:
		// Leave the children as they are, and set childOp to divbyzero
		if( t.current()->childOp() == Expression::division )
		{
			t.current()->setChildOp(Expression::divbyzero);
			return; // no point doing any more since we are going to raise a compileError later anyway.
		}
		zero = true;
		n = l;
		z = r;
	}
	// Now get rid of the useless nodes
	if(one)
	{
		BTreeNode *p = t.current(); // save in order to delete after
		replaceNode(p,n);
		t.setCurrent(n);
		// Delete the old nodes
		delete p;
		delete z;
	}
	if(zero)
	{
		BTreeNode *p = t.current();
		p->deleteChildren();
		p->setChildOp(Expression::noop);
		p->setType(number);
		p->setValue("0");
		
	}
	}
	else if( t.current()->childOp() == Expression::bwand || t.current()->childOp() == Expression::bwor || t.current()->childOp() == Expression::bwxor )
	{
	bool zero = false;
	if( l->value() == "0" )
	{
		zero = true;
		n = r;
		z = l;
	}
	else if( r->value() == "0" )
	{
		zero = true;
		n = l;
		z = r;
	}
	// Now get rid of the useless nodes
	if(zero)
	{
		BTreeNode *p = t.current();
		QString value;
		if( p->childOp() == Expression::bwand )
		{
			value = "0";
			p->deleteChildren();
			p->setChildOp(Expression::noop);
			p->setType(number);
		}
		if( p->childOp() == Expression::bwor || p->childOp() == Expression::bwxor )
		{
			value = n->value();
			BTreeNode *p = t.current(); // save in order to delete after
			replaceNode(p,n);
			t.setCurrent(n);
			// Delete the old nodes
			delete p;
			delete z;
		}
		p->setValue(value);
	}
	}
	
	if(!t.current()->parent() || t.current() == root) done = true;
	else
	{

	}
	}
}

void BTreeBase::replaceNode(BTreeNode *node, BTreeNode *replacement)
{
	// (This works under the assumption that a node is not linked to two places at once).
	if( !node->parent() )
	{
		setRoot(replacement);
		replacement->setParent(nullptr);
		return;
	}
	if( node->parent()->left() == node ) node->parent()->setLeft(replacement);
	if( node->parent()->right() == node ) node->parent()->setRight(replacement);
}
