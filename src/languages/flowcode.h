/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FLOWCODE_H
#define FLOWCODE_H

#include "language.h"

#include <QPointer>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>

class CNItem;
class FlowPart;
class Item;
class MicroSettings;

typedef QList<FlowPart*> FlowPartList;
typedef QList<QPointer<Item> > ItemList;

/**
"FlowCode" can possibly be considered a misnomer, as the output is actually Microbe.
However, the function of this class is to take a set of FlowParts, and generate the
basic from the code that they create. The 3 simple steps for usage of this function:
(1) Create an instance of this class, giving the Start point and setings
(2) Add all the subroutines present using addSubroutine()
(3) Call generateMicrobe() to get the Microbe code.
@author David Saxton
*/
class FlowCode : public Language
{
public:
	FlowCode( ProcessChain *processChain );

	void processInput( ProcessOptions options ) override;
	ProcessOptions::ProcessPath::Path outputPath( ProcessOptions::ProcessPath::Path inputPath ) const override;

	/**
	 * You must set the start part
	 */
	void setStartPart( FlowPart *startPart );
	~FlowCode() override;
	/**
	 * You must add all top level subroutines using this function
	 */
	void addSubroutine( FlowPart *part );
	/**
	 * Adds code at the current insertion point
	 */
	void addCode( const QString& code );
	/**
	 * Adds a code branch to the current insertion point. This will stop when the level gets
	 * below the original starting level (so for insertion of the contents of a for loop,
	 * insertion will stop at the end of that for loop).
	 * @param flowPart The next FlowPart to get code from
	 */
	void addCodeBranch( FlowPart *flowPart );
	/**
	 * Designates a FlowPart as a stopping part (i.e. will refuse requests to addCodeBranch
	 * for that FlowPart until removeStopPart is called
	 */
	void addStopPart( FlowPart *part );
	/**
	 * Undesignates a FlowPart as a stopping part
	 */
	void removeStopPart( FlowPart *part );
	/**
	 * Generates and returns the microbe code
	 */
	QString generateMicrobe( const ItemList &itemList, MicroSettings *settings );
	/**
	 * Returns true if the FlowPart is a valid one for adding a branch
	 */
	bool isValidBranch( FlowPart *flowPart );
	/**
	 * Generates a nice label name from the string, e.g. genLabel("callsub")
	 * returns "__label_callsub".
	 */
	static QString genLabel( const QString &id );

protected:
	/**
	 * Performs indenting, removal of unnecessary labels, etc.
	 */
	void tidyCode();

	QStringList m_gotos; // Gotos used
	QStringList m_labels; // Labels used
	FlowPartList m_subroutines;
	FlowPartList m_addedParts;
	FlowPartList m_stopParts;
	FlowPart *p_startPart;
	QString m_code;
	int m_curLevel;
};

#endif
