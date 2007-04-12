/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecnode.h"
#include "icndocument.h"
#include "libraryitem.h"
#include "multiinputgate.h"
#include "logic.h"

#include <cmath>
#include <klocale.h>
#include <qpainter.h>

//BEGIN class MultiInputGate
MultiInputGate::MultiInputGate( ICNDocument *icnDocument, bool newItem, const char *id, int baseWidth )
	: Component( icnDocument, newItem, id )
{
	b_doneInit = false;
	m_numInputs = 0;
	if ( baseWidth == -1 ) {
		baseWidth = 32;
	}
	m_baseWidth = baseWidth;
	
	for ( int i=0; i<maxGateInput; ++i )
	{
		inLogic[i] = 0;
		inNode[i] = 0;
	}
	
	updateInputs(2);
	
	init1PinRight(16);
	m_pOut = createLogicOut( m_pPNode[0], false );
	
	
	createProperty( "numInput", Variant::Type::Int );
	property("numInput")->setCaption( i18n("Number Inputs") );
	property("numInput")->setMinValue(2);
	property("numInput")->setMaxValue(maxGateInput);
	property("numInput")->setValue(2);
	
	b_doneInit = true;
}


MultiInputGate::~MultiInputGate()
{
}


void MultiInputGate::dataChanged()
{
	updateInputs( QMIN( maxGateInput, dataInt("numInput") ) );
}


void MultiInputGate::updateInputs( int newNum )
{
	if ( newNum == m_numInputs ) {
		return;
	}
	
	if ( newNum < 2 ) {
		newNum = 2;
	}
	else if ( newNum > maxGateInput ) {
		newNum = maxGateInput;
	}
	
	const int newWidth = m_baseWidth;
	
	setSize( -newWidth/2, -8*newNum, newWidth, 16*newNum, true );
	
	const bool added = ( newNum > m_numInputs );
	if (added)
	{
		for ( int i = m_numInputs; i<newNum; ++i )
		{
			ECNode *node = createPin( 0, 0, 0, "in"+QString::number(i) );
			inNode[i] = node;
			inLogic[i] = createLogicIn(node);
			inLogic[i]->setCallback( this, (CallbackPtr)(&MultiInputGate::inStateChanged) );
		}
	}
	else
	{
		for ( int i=newNum; i<m_numInputs; ++i )
		{
			removeNode("in"+QString::number(i));
			removeElement( inLogic[i], false );
			inNode[i] = 0;
			inLogic[i] = 0;
		}
	}
	
	m_numInputs = newNum;
	
	// We can't call a pure-virtual function if we haven't finished our constructor yet...
	if (b_doneInit)
		inStateChanged(!added);
	
	updateAttachedPositioning();
}


void MultiInputGate::updateAttachedPositioning()
{
	// Check that our ndoes have been created before we attempt to use them
	if ( !m_nodeMap.contains("p1") || !m_nodeMap.contains("in"+QString::number(m_numInputs-1)) )
		return;
	
	int _x = offsetX()+8;
	int _y = offsetY()+8;
	
	m_nodeMap["p1"].x = m_baseWidth/2 + 8;
	m_nodeMap["p1"].y = 0;
	
	for ( int i=0; i< m_numInputs; ++i )
	{
		m_nodeMap["in"+QString::number(i)].x = _x - 16;
		m_nodeMap["in"+QString::number(i)].y = _y + 16*i;
	}
	
	if (b_doneInit)
		Component::updateAttachedPositioning();
}
//END class MultiInputGate


//BEGIN class ECXNor
Item* ECXnor::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECXnor( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECXnor::libraryItem()
{
	return new LibraryItem(
		QString::QString("ec/xnor"),
		i18n("XNOR gate"),
		i18n("Logic"),
		"xnor.png",
		LibraryItem::lit_component,
		ECXnor::construct );
}

ECXnor::ECXnor( ICNDocument *icnDocument, bool newItem, const char *id )
	: MultiInputGate( icnDocument, newItem, (id) ? id : "xnor", 48 )
{
	m_name = i18n("XNOR gate");
	m_desc = i18n("Exclusive NOR gate. Output is low when exactly one input is high.");
	
	inStateChanged(false);
}

ECXnor::~ECXnor()
{
}

void ECXnor::inStateChanged(bool)
{
	int highCount = 0;
	for ( int i=0; i<m_numInputs; ++i )
	{
		if ( inLogic[i]->isHigh() )
			highCount++;
	}
	
	m_pOut->setHigh( highCount != 1 );
}

void ECXnor::drawShape( QPainter &p )
{
	initPainter(p);
	int _x = (int)x()+offsetX();
	int _y = (int)y()+offsetY();
	
	p.save();
	p.setPen( Qt::NoPen );
	p.drawChord( _x-width()+22, _y, 2*width()-28, height(), -16*81, 16*162 );
	p.restore();
	
	p.drawArc( _x-width()+22, _y, 2*width()-28, height(), -16*90, 16*180 );
	p.drawArc( _x-8, _y, 16, height(), -16*90, 16*180 );
	p.drawArc( _x, _y, 16, height(), -16*90, 16*180 );
	
	p.drawEllipse( _x+width()-6, _y+(height()/2)-3, 6, 6 );
	
	const int n = m_numInputs;
	for ( int i=0; i<n; ++i )
	{
		p.setPen( inNode[i]->isSelected() ? m_selectedCol : Qt::black );
		int pin_x = (int)std::sqrt((double)(64*n*n - (8*n-8-16*i)*(8*n-8-16*i)))/n;
		p.drawLine( _x, _y+16*i+8, _x+pin_x, _y+16*i+8 );
	}
	
	deinitPainter(p);
}
//END class ECXnor


//BEGIN class ECXor
Item* ECXor::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECXor( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECXor::libraryItem()
{
	return new LibraryItem(
		QString::QString("ec/xor"),
		i18n("XOR gate"),
		i18n("Logic"),
		"xor.png",
		LibraryItem::lit_component,
		ECXor::construct );
}

ECXor::ECXor( ICNDocument *icnDocument, bool newItem, const char *id )
	: MultiInputGate( icnDocument, newItem, (id) ? id : "xor", 48 )
{
	m_name = i18n("XOR gate");
	m_desc = i18n("Exclusive OR gate. Output is high when exactly one input is high.");
	
	inStateChanged(false);
}

ECXor::~ECXor()
{
}

void ECXor::inStateChanged(bool)
{
	int highCount = 0;
	for ( int i=0; i<m_numInputs; ++i )
	{
		if ( inLogic[i]->isHigh() )
			highCount++;
	}
	
	m_pOut->setHigh( highCount == 1 );
}

void ECXor::drawShape( QPainter &p )
{
	initPainter(p);
	int _x = (int)x()+offsetX();
	int _y = (int)y()+offsetY();
	
	p.save();
	p.setPen( Qt::NoPen );
	p.drawChord( _x-width()+16, _y, 2*width()-16, height(), -16*81, 16*162 );
	p.restore();
	
	p.drawArc( _x-width()+16, _y, 2*width()-16, height(), -16*90, 16*180 );
	p.drawArc( _x-8, _y, 16, height(), -16*90, 16*180 );
	p.drawArc( _x, _y, 16, height(), -16*90, 16*180 );
	
	const int n = m_numInputs;
	for ( int i=0; i<n; ++i )
	{
		p.setPen( inNode[i]->isSelected() ? m_selectedCol : Qt::black );
		int pin_x = (int)std::sqrt((double)(64*n*n - (8*n-8-16*i)*(8*n-8-16*i)))/n;
		p.drawLine( _x, _y+16*i+8, _x+pin_x, _y+16*i+8 );
	}
	
	deinitPainter(p);
}
//END class ECXor


//BEGIN class EXOr
Item* ECOr::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECOr( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECOr::libraryItem()
{
	return new LibraryItem(
		QString::QString("ec/or"),
		i18n("OR gate"),
		i18n("Logic"),
		"or.png",
		LibraryItem::lit_component,
		ECOr::construct );
}

ECOr::ECOr( ICNDocument *icnDocument, bool newItem, const char *id )
	: MultiInputGate( icnDocument, newItem, (id) ? id : "or", 48 )
{
	m_name = i18n("OR gate");
	m_desc = i18n("The output is high when at least one of the inputs is high; or low when all of the inputs are off");
	
	inStateChanged(false);
}

ECOr::~ECOr()
{
}

void ECOr::inStateChanged(bool)
{
	bool allLow = true;
	for ( int i=0; i<m_numInputs && allLow; ++i )
	{
		if ( inLogic[i]->isHigh() )
			allLow = false;
	}
	
	m_pOut->setHigh(!allLow);
}

void ECOr::drawShape( QPainter &p )
{
	initPainter(p);
	int _x = (int)x()+offsetX();
	int _y = (int)y()+offsetY();
	
	p.save();
	p.setPen( Qt::NoPen );
// 	p.setBrush( Qt::red );
	p.drawChord( _x-width(), _y, 2*width(), height(), -16*81, 16*162 );
// 	p.drawPie( _x-width()+16, _y, 2*width()-16, height(), -16*100, 16*200 );
	p.restore();
	
	p.drawArc( _x-width(), _y, 2*width(), height(), -16*90, 16*180 );
	p.drawArc( _x-8, _y, 16, height(), -16*90, 16*180 );
	
	const int n = m_numInputs;
	for ( int i=0; i<n; ++i )
	{
		p.setPen( inNode[i]->isSelected() ? m_selectedCol : Qt::black );
		int pin_x = (int)std::sqrt((double)(64*n*n - (8*n-8-16*i)*(8*n-8-16*i)))/n;
		p.drawLine( _x, _y+16*i+8, _x+pin_x, _y+16*i+8 );
	}
	
	deinitPainter(p);
}
//END class ECOr


//BEGIN class ECNor
Item* ECNor::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECNor( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECNor::libraryItem()
{
	return new LibraryItem(
		QString::QString("ec/nor"),
		i18n("NOR gate"),
		i18n("Logic"),
		"nor.png",
		LibraryItem::lit_component,
		ECNor::construct );
}

ECNor::ECNor( ICNDocument *icnDocument, bool newItem, const char *id )
	: MultiInputGate( icnDocument, newItem, (id) ? id : "nor", 48 )
{
	m_name = i18n("NOR Gate");
	m_desc = i18n("The output is high when all inputs are low.");
	
	inStateChanged(false);
}

ECNor::~ECNor()
{
}

void ECNor::inStateChanged(bool)
{
	bool allLow = true;
	for ( int i=0; i<m_numInputs && allLow; ++i )
	{
		if ( inLogic[i]->isHigh() )
			allLow = false;
	}
	
	m_pOut->setHigh(allLow);
}

void ECNor::drawShape( QPainter &p )
{
	initPainter(p);
	int _x = (int)x()+offsetX();
	int _y = (int)y()+offsetY();
	
	p.save();
	p.setPen( Qt::NoPen );
	p.drawChord( _x-width()+6, _y, 2*width()-12, height(), -16*81, 16*162 );
	p.restore();
	
	p.drawArc( _x-width()+6, _y, 2*width()-12, height(), -16*90, 16*180 );
	p.drawArc( _x-8, _y, 16, height(), -16*90, 16*180 );
	
	p.drawEllipse( _x+width()-6, _y+(height()/2)-3, 6, 6 );
	
	const int n = m_numInputs;
	for ( int i=0; i<n; ++i )
	{
		p.setPen( inNode[i]->isSelected() ? m_selectedCol : Qt::black );
		int pin_x = (int)std::sqrt((double)(64*n*n - (8*n-8-16*i)*(8*n-8-16*i)))/n;
		p.drawLine( _x, _y+16*i+8, _x+pin_x, _y+16*i+8 );
	}
	
	deinitPainter(p);
}
//END class ECNor


//BEGIN class ECNand
Item* ECNand::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECNand( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECNand::libraryItem()
{
	return new LibraryItem(
		QString::QString("ec/nand"),
		i18n("NAND gate"),
		i18n("Logic"),
		"nand.png",
		LibraryItem::lit_component,
		ECNand::construct );
}

ECNand::ECNand( ICNDocument *icnDocument, bool newItem, const char *id )
	: MultiInputGate( icnDocument, newItem, id ? id : "nand" )
{
	m_name = i18n("NAND Gate");
	m_desc = i18n("The output is low only when all of the inputs are high.");
	
	inStateChanged(false);
}

ECNand::~ECNand()
{
}

void ECNand::inStateChanged(bool)
{
	bool allHigh = true;
	for ( int i=0; i<m_numInputs && allHigh; ++i )
	{
		if ( !inLogic[i]->isHigh() )
			allHigh = false;
	}
	
	m_pOut->setHigh(!allHigh);
}

void ECNand::drawShape( QPainter &p )
{
	initPainter(p);
	int _x = (int)x()+offsetX();
	int _y = (int)y()+offsetY();
	p.drawChord( _x-width()+6, _y, 2*width()-12, height(), -16*90, 16*180 );
	p.drawEllipse( _x+width()-6, _y+(height()/2)-3, 6, 6 );
	deinitPainter(p);
}
//END class ECNand


//BEGIN class ECAnd
Item* ECAnd::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECAnd( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECAnd::libraryItem()
{
	QStringList idList;
	idList << "ec/and" << "ec/and_2";
	return new LibraryItem(
		idList,
		i18n("AND gate"),
		i18n("Logic"),
		"and.png",
		LibraryItem::lit_component,
		ECAnd::construct );
}

ECAnd::ECAnd( ICNDocument *icnDocument, bool newItem, const char *id )
	: MultiInputGate( icnDocument, newItem, id ? id : "and" )
{
	m_name = i18n("AND Gate");
	m_desc = i18n("The output is high if and only if all of the inputs are high.");
	
	inStateChanged(false);
}

ECAnd::~ECAnd()
{
}

void ECAnd::inStateChanged(bool)
{
	bool allHigh = true;
	for ( int i=0; i<m_numInputs && allHigh; ++i )
	{
		if ( !inLogic[i]->isHigh() )
			allHigh = false;
	}
	
	m_pOut->setHigh(allHigh);
}

void ECAnd::drawShape( QPainter &p )
{
	initPainter(p);
	
	int _x = (int)x()+offsetX();
	int _y = (int)y()+offsetY();
	p.drawChord( _x-width(), _y, 2*width(), height(), -16*90, 16*180 );
	
	deinitPainter(p);
}
//END class ECAnd
