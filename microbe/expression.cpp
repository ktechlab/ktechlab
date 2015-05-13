/***************************************************************************
 *   Copyright (C) 2004-2005 by Daniel Clarke                              *
 *   daniel.jc@gmail.com                                                   *
 *									   *
 *   24-04-2007                                                            *
 *   Modified to add pic 16f877,16f627 and 16f628 			   *
 *   by george john george@space-kerala.org,az.j.george@gmail.com	   *
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
 
#include "btreebase.h"
#include "btreenode.h"
#include "expression.h"
#include "traverser.h"
#include "parser.h"
#include "pic14.h"

#include <kdebug.h>
#include <klocale.h>
#include <Qt/qregexp.h>

Expression::Expression( PIC14 *pic, Microbe *master, SourceLine sourceLine, bool suppressNumberTooBig )
	: m_sourceLine(sourceLine)
{
	m_pic = pic;
	mb = master;
	m_bSupressNumberTooBig = suppressNumberTooBig;
}

Expression::~Expression()
{
}

void Expression::traverseTree( BTreeNode *root, bool conditionalRoot )
{
	Traverser t(root);
	t.start();
	
	// special case: if we are starting at the root node then
	// we are dealing with something of the form variable = 6
	// or variable = portb
	///TODO reimplement assignments as two branched trees?
	if ( t.current() == root &&
			!root->hasChildren() &&
			t.current()->childOp() != pin &&
			t.current()->childOp() != notpin &&
			t.current()->childOp() != function &&
			t.current()->childOp() != read_keypad )
	{
		switch(root->type())
		{
			case number: m_pic->assignNum(root->value()); break;
			case variable: m_pic->assignVar(root->value()); break;
			default: break; // Should never get here
		}
		// no need to traverse the tree as there is none.
		return;
	}
	
	t.setCurrent(root);
	
	if(t.current()->hasChildren())
	{
		// Here we work out what needs evaulating, and in which order.
		// To minimize register usage, if only one branch needs traversing,
		// then that branch should be done first.
		bool evaluateLeft = t.current()->left()->needsEvaluating();
	
		BTreeNode *evaluateFirst;
		BTreeNode *evaluateSecond;
	
		// If both need doing, then it really doesn't matter which we do
		// first (unless we are looking to do really complex optimizations...
	
		// Cases: 
		// - Both need evaluating,
		// - or left needs doing first,
		// in both cases we evaluate left, then right.
		if( evaluateLeft )
		{
			evaluateFirst = t.current()->left();
			evaluateSecond = t.current()->right();
		}
		// Otherwise it is best to evaluate right first for reasons given above.
		else
		{
			evaluateFirst = t.current()->right();
			evaluateSecond = t.current()->left();
		}
		
		QString dest1 = mb->dest();
		mb->incDest();
		QString dest2 = mb->dest();
		mb->decDest();
	
		bool evaluated = false;
		if( evaluateFirst->hasChildren() )
		{	
			traverseTree(evaluateFirst);
			evaluated = true;
		}
		else if( isUnaryOp(evaluateFirst->childOp()) )
		{
			doUnaryOp( evaluateFirst->childOp(), evaluateFirst );
			evaluated = true;
		}
		if ( evaluated )
		{
			// We need to save the result if we are going tro traverse the other
			// branch, or if we are performing a subtraction in which case the
			// value wanted in working is not the current value.
			// But as the optimizer will deal with unnecessary variables anyway,
			// always save to a register
			
			evaluateFirst->setReg( dest1 );
			evaluateFirst->setType( variable );
			m_pic->saveToReg( dest1 );
		}
	
		evaluated = false;
		if( evaluateSecond->hasChildren() )
		{
			mb->incDest();
			mb->incDest();
			traverseTree(evaluateSecond);
			evaluated = true;
			mb->decDest();
			mb->decDest();
		}
		else if( isUnaryOp(evaluateSecond->childOp()) )
		{
			doUnaryOp( evaluateSecond->childOp(), evaluateSecond );
			evaluated = true;
		}
		if ( evaluated )
		{
			evaluateSecond->setReg( dest2 );
			evaluateSecond->setType( variable );
			m_pic->saveToReg( dest2 );
		}
	}
	
	if(t.current()->childOp()==divbyzero)
	{
		mistake( Microbe::DivisionByZero );
	}
	
	// If we are at the top level of something like 'if a == 3 then', then we are ready to put
	// in the if code, else the expression just evaluates to 0 or 1
	if(conditionalRoot && t.current() == root)
		m_pic->setConditionalCode(m_ifCode, m_elseCode);

	// Handle operations
	// (functions are not actually supported)
	if(isUnaryOp(t.current()->childOp()))
		doUnaryOp( t.current()->childOp(), t.current() );
	else
		doOp( t.current()->childOp(), t.current()->left(), t.current()->right() );

}

void Expression::doOp( Operation op, BTreeNode *left, BTreeNode *right )
{
	QString lvalue;
	if(left->reg().isEmpty())
		lvalue = left->value();
	else
		lvalue = left->reg();
	
	QString rvalue;
	if(right->reg().isEmpty())
		rvalue = right->value();
	else
		rvalue = right->reg();
	
	// Handle if stuff
	PIC14::LocationType leftType = PIC14::num;
	switch ( left->type() )
	{
		case number:
			leftType = PIC14::num;
			break;
			
		case variable:
			leftType = PIC14::var;
			break;
			
		case working:
			leftType = PIC14::work;
			break;
			
		case unset:
		case extpin:
		case keypad:
			kError() << k_funcinfo << "Bad left->type(): " << left->type() << endl;
	};
	
	PIC14::LocationType rightType = PIC14::work;
	switch ( right->type() )
	{
		case number:
			rightType = PIC14::num;
			break;
			
		case variable:
			rightType = PIC14::var;
			break;
			
		case working:
			rightType = PIC14::work;
			break;
			
		case unset:
		case extpin:
		case keypad:
			kError() << k_funcinfo << "Bad right->type(): " << right->type() << endl;
	};
	
	switch(op)
	{
		case equals:	m_pic->equal( lvalue, rvalue, leftType, rightType ); break;
		case notequals:	m_pic->notEqual( lvalue, rvalue, leftType, rightType ); break;
		case lt:		m_pic->lessThan( lvalue, rvalue, leftType, rightType ); break;
		case gt:		m_pic->greaterThan( lvalue, rvalue, leftType, rightType ); break;
		case le:		m_pic->lessOrEqual( lvalue, rvalue, leftType, rightType ); break;
		case ge:		m_pic->greaterOrEqual( lvalue, rvalue, leftType, rightType ); break;
		
		case addition:		m_pic->add( lvalue, rvalue, leftType, rightType ); break;
		case subtraction:	m_pic->subtract( lvalue, rvalue, leftType, rightType ); break;
		case multiplication:	m_pic->mul( lvalue, rvalue, leftType, rightType ); break;
		case division:		m_pic->div( lvalue, rvalue, leftType, rightType ); break;
		
		case bwand:	m_pic->bitwise( bwand, lvalue, rvalue, leftType, rightType ); break;
		case bwor:	m_pic->bitwise( bwor, lvalue, rvalue, leftType, rightType ); break;
		case bwxor:	m_pic->bitwise( bwxor, lvalue, rvalue, leftType, rightType ); break;
		case bwnot:	m_pic->bitwise( bwnot, lvalue, rvalue, leftType, rightType ); break;
		
		default: break;
	}
}

void Expression::buildTree( const QString & unstrippedExpression, BTreeBase *tree, BTreeNode *node, int level )
{
	int firstEnd = 0;
	int secondStart = 0;
	bool unary = false;
	Operation op = noop;
	QString expression = stripBrackets( unstrippedExpression );
	switch(level)
	{
		// ==, !=
		case 0:
		{
		int equpos = findSkipBrackets(expression, "==");
		int neqpos = findSkipBrackets(expression, "!=");
		if( equpos != -1 )
		{
			op = equals;
			firstEnd = equpos;
			secondStart = equpos + 2;
		}
		else if( neqpos != -1 )
		{
			op = notequals;
			firstEnd = neqpos;
			secondStart = neqpos + 2;
		}
		else op = noop;
		break;
		}

		// <, <=, >=, >
		case 1:
		{
		int ltpos = findSkipBrackets(expression, "<");
		int lepos = findSkipBrackets(expression, "<=");
		int gepos = findSkipBrackets(expression, ">=");
		int gtpos = findSkipBrackets(expression, ">");
		// Note: if (for example) "<=" is present, "<" will also be present. This
		// means that we have to check for "<=" before "<", etc.
		if( lepos != -1 )
		{
			op = le;
			firstEnd = lepos;
			secondStart = lepos + 2;
		}
		else if( gepos != -1 )
		{
			op = ge;
			firstEnd = gepos;
			secondStart = gepos + 2;
		}
		else if( ltpos != -1 )
		{
			op = lt;
			firstEnd = ltpos;
			secondStart = ltpos + 1;
		}
		else if( gtpos != -1 )
		{
			op = gt;
			firstEnd = gtpos;
			secondStart = gtpos + 1;
		}
		else op = noop;
		break;
		}

		// +,-
		case 2:
		{
		int addpos = findSkipBrackets(expression, '+');
		int subpos = findSkipBrackets(expression, '-');
		if( subpos != -1 )
		{
			op = subtraction;
			firstEnd = subpos;
			secondStart = subpos + 1;
		}
		else if( addpos != -1 )
		{
			op = addition;
			firstEnd = addpos;
			secondStart = addpos + 1;
		}
		else op = noop;
		break;
		}
		
		// *,/
		case 3:
		{
		int mulpos = findSkipBrackets(expression, '*');
		int divpos = findSkipBrackets(expression, '/');
		if( divpos != -1 )
		{
			op = division;
			firstEnd = divpos;
			secondStart = divpos + 1;
		}
		else if( mulpos != -1 )
		{
			op = multiplication;
			firstEnd = mulpos;
			secondStart = mulpos + 1;
		}
		else op = noop;
		break;
		}
		
		// ^
		case 4:
		{
		int exppos = findSkipBrackets(expression, '^');
		if( exppos != -1 )
		{
			op = exponent;
			firstEnd = exppos;
			secondStart = exppos + 1;
		}
		else op = noop;
		break;
		}
		
		// AND, OR, XOR
		case 5:
		{
		int bwAndPos = findSkipBrackets(expression, " AND ");
		int bwOrPos = findSkipBrackets(expression, " OR ");
		int bwXorPos = findSkipBrackets(expression, " XOR ");
		if( bwAndPos != -1 )
		{
			op = bwand;
			firstEnd = bwAndPos;
			secondStart = bwAndPos + 5;
		}
		else if( bwOrPos != -1 )
		{
			op = bwor;
			firstEnd = bwOrPos;
			secondStart = bwOrPos + 4;
		}
		else if( bwXorPos != -1 )
		{
			op = bwxor;
			firstEnd = bwXorPos;
			secondStart = bwXorPos + 5;
		}
		else op = noop;
		break;
		}
		
		// NOT
		case 6:
		{
		int bwNotPos = findSkipBrackets(expression, " NOT ");
		if( bwNotPos != -1 )
		{
			op = bwnot;
			unary = true;
			firstEnd = bwNotPos; // this line is not needed for unary things/
			secondStart = bwNotPos + 5;
		}
		else op = noop;
		break;
		}
	}
	
	node->setChildOp(op);
	
	QString tokens[2];
	tokens[0] = expression.left(firstEnd).trimmed();
	tokens[1] = expression.mid(secondStart).trimmed();
	
	if( op != noop )
	{	
		for( int j = 0; j < 2; j++ )
		{
			
			BTreeNode *newNode = new BTreeNode();
			tree->addNode( node, newNode, (j == 0) );
			// we need to strip any brackets from the sub-expression
			
			// try each token again at the same level, if they 
			// don't have any of this level's operators, then the function
			// will go to the next level as below.
			
			// For unary opertaions, e.g NOT, we have no special 
			// code for nodes with only one child, so we leave the left
			// hand child blank and put the rest in the right hand node.
			if( unary && j == 0 )
			{
				newNode->setValue("");
				newNode->setType(number);
			}
			else buildTree(tokens[j], tree, newNode, 0 );
		}
	}
	else
	{
		// if there was no relevant operation i.e. " 3*4 / 6" as opposed to " 3*4 + 6"
		// then just pass the node onto the next parsing level.
		// unless we are at the lowest level, in which case we have reached a final value.
		if( level == 6 ) expressionValue(expression,tree,node);
		else 
		{
			buildTree(expression,tree,node,level + 1);
		}
	}
}

void Expression::doUnaryOp(Operation op, BTreeNode *node)
{
	/* Note that this isn't for unary operations as such,
	 rather for things that are operations that have no direct children,
	 e.g. portx.n is high, and functionname(args)*/
	
	if ( op == pin || op == notpin )
		m_pic->Spin( m_pic->toPortPin( node->value() ), (op==notpin) );
	
	else if ( op == read_keypad )
		m_pic->Skeypad( mb->variable( node->value() ) );
}

void Expression::compileExpression( const QString & expression )
{
	// Make a tree to put the expression in.
	BTreeBase *tree = new BTreeBase();
	BTreeNode *root = new BTreeNode();

	// parse the expression into the tree
	buildTree(expression,tree,root,0);
	// compile the tree into assembly code
	tree->setRoot(root);
	tree->pruneTree(tree->root());
	traverseTree(tree->root());
	
	// Note deleting the tree deletes all nodes, so the root
	// doesn't need deleting separately.
	delete tree;
	return;
}

void Expression::compileConditional( const QString & expression, Code * ifCode, Code * elseCode )
{
	if( expression.contains(QRegExp("=>|=<|=!")) )
	{
		mistake( Microbe::InvalidComparison, expression );
		return;
	}
	if( expression.contains(QRegExp("[^=><!][=][^=]")))
	{
		mistake( Microbe::InvalidEquals );
		return;
	}
	// Make a tree to put the expression in.
	BTreeBase *tree = new BTreeBase();
	BTreeNode *root = new BTreeNode();

	// parse the expression into the tree
	buildTree(expression,tree,root,0);
	
	// Modify the tree so it is always at the top level of the form (kwoerpkwoep) == (qwopekqpowekp)
	if ( root->childOp() != equals &&
			root->childOp() != notequals &&
			root->childOp() != gt &&
			root->childOp() != lt &&
			root->childOp() != ge &&
			root->childOp() != le &&
			root->childOp() != pin &&
			root->childOp() != notpin &&
			root->childOp() != read_keypad )
	{
		BTreeNode *newRoot = new BTreeNode();
		
		BTreeNode *oneNode = new BTreeNode();
		oneNode->setChildOp(noop);
		oneNode->setType(number);
		oneNode->setValue("1");
		
		newRoot->setLeft(root);
		newRoot->setRight(oneNode);
		newRoot->setType(unset);
		newRoot->setChildOp(ge);
		
		tree->setRoot(newRoot);
		root = newRoot;
	}
	// compile the tree into assembly code
	tree->setRoot(root);
	tree->pruneTree(tree->root(),true);
	
	// We might have just a constant expression, in which case we can just always do if or else depending
	// on whether it is true or false.
	if( root->childOp() == noop )
	{
		if( root->value().toInt() == 0 )
			m_pic->mergeCode( elseCode );
		else
			m_pic->mergeCode( ifCode );
		return;
	}
	
	// traverse tree with argument conditionalRoot true
	// so that 3 == x gets integrated with code for if, repeat until etc...
	m_ifCode = ifCode;
	m_elseCode = elseCode;
	traverseTree(tree->root(),true);
	
	// Note deleting the tree deletes all nodes, so the root
	// doesn't need deleting separately.
	delete tree;
}

bool Expression::isUnaryOp(Operation op)
{
	return op == pin || op == notpin || op == function || op == read_keypad;
}


void Expression::mistake( Microbe::MistakeType type, const QString & context )
{
	mb->compileError( type, context, m_sourceLine );
}

int Expression::findSkipBrackets( const QString & expr, char ch, int startPos)
{
	bool found = false;
	int i = startPos;
	int bracketLevel = 0;
	while(!found)
	{
		if(expr[i].toLatin1() == '\'')
		{
			if( i + 2 < int(expr.length()) )
			{
				if( expr[i+2].toLatin1() == '\'' )
				{
				 i = i + 2;
				 found = true;
				}
			}
		}
		
		if(expr[i].toLatin1() == '(') bracketLevel++;
		else if(expr[i].toLatin1() == ')') bracketLevel--;
		
		if( bracketLevel == 0 )
		{
			if(expr[i].toLatin1() == ch) found = true;
			else i++;
		}
		else i++;
		
		if( i >= int(expr.length()) )
		{
			found = true;
			i = -1;
		}
	}
	return i;
}

int Expression::findSkipBrackets( const QString & expr, QString phrase, int startPos)
{
	bool found = false;
	int i = startPos;
	int bracketLevel = 0;
	while(!found)
	{	
		if(expr[i].toLatin1() == '\'')
		{
			if( i + 2 < int(expr.length()) )
			{
				if( expr[i+2].toLatin1() == '\'' )
				{
				 i = i + 2;
				 found = true;
				}
			}
		}
		
		if(expr[i].toLatin1() == '(') bracketLevel++;
		else if(expr[i].toLatin1() == ')') bracketLevel--;
		
		if( bracketLevel == 0 )
		{
			if(expr.mid(i,phrase.length()) == phrase) found = true;
			else i++;
		}
		else i++;
		
		if( i >= int(expr.length()) )
		{
			found = true;
			i = -1;
		}
	}
	return i;
}

QString Expression::stripBrackets( QString expression )
{
	bool stripping = true;
	int bracketLevel = 0;
	int i = 0;
	expression = expression.trimmed();
	while(stripping)
	{
		if( expression.at(i) == '(' ) bracketLevel++;
		else if( expression.at(i) == ')' )
		{
			if( i == int(expression.length() - 1) && bracketLevel == 1)
			{
				expression = expression.mid(1,expression.length() - 2).trimmed();
			}
			bracketLevel--;	
		}
		if( i == int(expression.length() - 1) && bracketLevel > 0 )
		{
			mistake( Microbe::MismatchedBrackets, expression );
			// Stray brackets might cause the expressionession parser some problems,
			// so we just avoid parsing anything altogether
			expression = "";
			stripping = false;
		}
		i++;
		if( bracketLevel == 0 ) stripping = false;
	}
	return expression;
}

void Expression::expressionValue( QString expr, BTreeBase */*tree*/, BTreeNode *node)
{
	/* The "end of the line" for the expression parsing, the
	expression has been broken down into the fundamental elements of expr.value()=="to"||
	variable, number, special etc... so we now just set value and type */
	
	
	
	/* Alternatively we might have a function call
	e.g. somefunction(3,potatoes,hairstyle + 6)
	In which case we need to call back to parseExpr to process the arguments,
	saving them on the basic stack then  making the function call.
	Of course we also need to mark the terminal node type as a function.
	*/
	expr = expr.trimmed();
	
	// My intention is so that these error checks are ordered
	// so that e.g. for x = 3 it picks up the = rather than the spaces first.
	
	
	expr = mb->alias(expr);
	ExprType t = expressionType(expr);

	
	// See if it is a single qouted character, e.g. 'A'
	if( expr.left(1) == "\'" && expr.right(1) == "\'" ) 
	{
		if( expr.length() == 3 ) // fall through to report as unknown variable if not of form 'x'
		{
			// If so, turn it into a number, and use the ASCII code as the value
			t = number;
			expr =  QString::number(expr[1].toLatin1());
		}
	}
	
	// Check for the most common mistake ever!
	if(expr.contains("="))
		mistake( Microbe::InvalidEquals );
	// Check for reserved keywords
	if(expr=="to"||expr=="step"||expr=="then")
		mistake( Microbe::ReservedKeyword, expr );

	// Check for empty expressions, or expressions contating spaces
	// both indicating a Mistake.
	if(expr.isEmpty())
		mistake( Microbe::ConsecutiveOperators );
	else if(expr.contains(QRegExp("\\s")) && t!= extpin)
		mistake( Microbe::MissingOperator );

//***************modified isValidRegister is included ***********************//	

	if( t == variable && !mb->isVariableKnown(expr) && !m_pic->isValidPort( expr ) && !m_pic->isValidTris( expr )&&!m_pic->isValidRegister( expr ) )
		mistake( Microbe::UnknownVariable, expr );

//modification ends

	if ( mb->isVariableKnown(expr) && !mb->variable(expr).isReadable() )
		mistake( Microbe::WriteOnlyVariable, expr );
	
	node->setType(t);
	
	// Since we currently only implement 8 bit unsigned integers, we should disallow
	// anything outside the range [0-255].
	if( t == number && !m_bSupressNumberTooBig && (expr.toInt() > 255) )
	{
		mistake( Microbe::NumberTooBig );
	}
	
	// if there was a pin, we need to decocde it.
	// For now and sacrificing syntax error checking
	// we just look for the word "is" then "high" or "low".
	if( t == extpin )
	{
		bool NOT;
		int i = expr.indexOf("is");
		if(i > 0)
		{
			NOT = expr.contains("low");
			if(!expr.contains("high") && !expr.contains("low"))
				mistake( Microbe::HighLowExpected, expr );
			expr = expr.left(i-1);
		}
		else NOT = false;
		node->setChildOp(NOT?notpin:pin);
	}
	
	else if ( t == keypad )
		node->setChildOp( read_keypad );
	
	node->setValue(expr);
}

ExprType Expression::expressionType( const QString & expression )
{
	// So we can't handle complex expressions yet anyway,
	// let's just decide whether it is a variable or number.
	
	// Thanks to the convention that variable names must not
	// begin with a number this is extremely simple to do!

	/* But now there is a catch, because there can also be
	things that have a first character alpha, but are of the form
	"portb.3 is high", general syntax: portx.n is <high|low>
	additionally, there can be things that are just porta.6, which just return the truth of that port.
	In reality it is just:
	portx.n is high === portx.n
	portx.n is low === !(portx.n)
	These types of expression can be identified by the fact 
	that they should be the only things that contain a '.'
	*/
	
	/* Note that at the moment, literalToInt returns -1 if it is
	not literal so isLiteral is redundant, but this may change if say
	negative numbers are implemented
	*/
	
	int value = Parser::literalToInt(expression);
	if ( value != -1 )
		return number;
	
	if( expression.contains('.') )
		return extpin;
	
	if ( mb->variable( expression ).type() == Variable::keypadType )
		return keypad;
	
	return variable;
}

QString Expression::processConstant( const QString & expr, bool * isConstant )
{
	bool temp;
	if (!isConstant)
		isConstant = &temp;
	
	QString code;
	
	// Make a tree to put the expression in.
	BTreeBase *tree = new BTreeBase();
	BTreeNode *root = new BTreeNode();

	// parse the expression into the tree
	buildTree(expr,tree,root,0);
	// compile the tree into assembly code
	tree->setRoot(root);
	tree->pruneTree(tree->root());
	//code = traverseTree(tree->root());
	// Look to see if it is a number
	if( root->type() == number )
	{
		code = root->value();
		*isConstant = true;
	}
	else
	{
		code = "";
		*isConstant = false;
	}
	
	// Note deleting the tree deletes all nodes, so the root
	// doesn't need deleting separately.
	delete tree;
	return code;
}
