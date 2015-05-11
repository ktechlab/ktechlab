/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "delay.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocale.h>

Item* Delay::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new Delay( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* Delay::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/delay")),
		i18n("Delay"),
		i18n("Functions"),
		"delay.png",
		LibraryItem::lit_flowpart,
		Delay::construct );
}

Delay::Delay( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "delay" )
{
	m_name = i18n("Delay");
	initProcessSymbol();
	createStdInput();
	createStdOutput();
	
	createProperty( "delay_length", Variant::Type::Double );
	property("delay_length")->setCaption( i18n("Pause Length") );
	property("delay_length")->setUnit("sec");
	property("delay_length")->setValue(1.0);
}

Delay::~Delay()
{
}

void Delay::dataChanged()
{
	double delay = dataDouble("delay_length");
	setCaption( i18n("Delay for %1 sec").arg(QString::number( delay / getMultiplier(delay), 'g', 3 )+getNumberMag(delay)) );
}

void Delay::generateMicrobe( FlowCode *code )
{
	const double delayLength_ms = dataDouble("delay_length")*1e3;
	code->addCode( "delay "+QString::number(delayLength_ms) );
	code->addCodeBranch( outputPart("stdoutput") );
	
// 	code->addVariable("COUNT_REPEAT");
	
#if 0
	// Code for pauses less than 769uS
	if ( pauseLength < 769 )
	{
		code->addCodeBlock( id(),	"movlw " + QString::number(pauseLength/3) + "\n"
									"movwf COUNT_REPEAT\n"
									"call count_3uS\n"
									+ gotoCode("stdoutput") );
									
		code->addCodeBlock( "count_3uS",	"decfsz COUNT_REPEAT,1\n"
											"goto count_3uS\n"
											"return" );
	}
	else if ( pauseLength < 196609 )
	{
		code->addVariable("COUNT_LOOP_1");
		
		code->addCodeBlock( id(),	"movlw " + QString::number(pauseLength/(3*256)) + "\n"
									"movwf COUNT_REPEAT\n"
									"call count_768uS\n"
									+ gotoCode("stdoutput") );
									
		code->addCodeBlock( "count_768uS",	"decfsz	COUNT_LOOP_1,1\n"
											"goto count_768uS\n"
											"decfsz COUNT_REPEAT,1\n"
											"goto count_768uS\n"
											"return" );
	}
	else if ( pauseLength < 50331649 )
	{
		code->addVariable("COUNT_LOOP_1");
		code->addVariable("COUNT_LOOP_2");
		
		code->addCodeBlock( id(),	"movlw " + QString::number(pauseLength/(3*256*256)) + "\n"
									"movwf COUNT_REPEAT\n"
									"call count_200mS\n"
									+ gotoCode("stdoutput") );
									
		code->addCodeBlock( "count_200mS",	"decfsz	COUNT_LOOP_1,1\n"
											"goto count_200mS\n"
											"decfsz COUNT_LOOP_2,1\n"
											"goto count_200mS\n"
											"decfsz COUNT_REPEAT,1\n"
											"goto count_200mS\n"
											"return" );
	}
	else/* if ( pauseLength < 12884901889 )*/
	{
		code->addVariable("COUNT_LOOP_1");
		code->addVariable("COUNT_LOOP_2");
		code->addVariable("COUNT_LOOP_3");
		
		code->addCodeBlock( id(),	"movlw " + QString::number(pauseLength/(3*256*256*256)) + "\n"
									"movwf COUNT_REPEAT\n"
									"call count_50S\n"
									+ gotoCode("stdoutput") );
									
		code->addCodeBlock( "count_50S",	"decfsz	COUNT_LOOP_1,1\n"
											"goto count_50S\n"
											"decfsz COUNT_LOOP_2,1\n"
											"goto count_50S\n"
											"decfsz COUNT_LOOP_3,1\n"
											"goto count_50S\n"
											"decfsz COUNT_REPEAT,1\n"
											"goto count_50S\n"
											"return" );
	}
#endif
}

