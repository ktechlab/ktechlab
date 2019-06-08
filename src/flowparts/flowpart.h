/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FLOWPART_H
#define FLOWPART_H

#include "cnitem.h"

class ICNDocument;
class Node;
class FlowCode;
class FlowCodeDocument;
class FlowPart;
class FPNode;
class QPixmap;
class QSize;

typedef QList<FlowPart*> FlowPartList;

/**
All flow parts (eg 'CallSub', 'Read from Port' ,etc) should inherit from this class.
It provides basic functionality for creating commonly used nodes, as well as a virtual function
that you should reinherit for generating the assembly code.
@short Base class for all FlowParts
@author David Saxton
*/
class FlowPart : public CNItem
{
Q_OBJECT
public:
	enum FlowSymbol
	{
		ps_process, // Process - Plain rectangular box
		ps_call, // Call - Rectangular box with double vertical lines at either end
		ps_io, // I/O - Slanter rectangular box
		ps_round, // Start/End - Rounded rectangular box
		ps_decision, // Decision - Diamond shape
		ps_other // Custom shape, which is ignored by FlowPart
	};
	FlowPart( ICNDocument *icnDocument, bool newItem, const QString &id );
	~FlowPart() override;

	virtual void generateMicrobe( FlowCode */*code*/ ) = 0;
	/**
	 * Set a preset "orientation" of this item - 0 through 7
	 */
	void setOrientation( uint orientation );
	uint orientation() const { return m_orientation; }
	/**
	 * The allowed orientations, as bit positions of 0 through 7
	 */
	uint allowedOrientations() const;
	ItemData itemData() const override;
	void restoreFromItemData( const ItemData &itemData ) override;
	/**
	 * Sets the caption displayed in the flowpart, resizes the item as necessary
	 */
	virtual void setCaption( const QString &caption );
	/**
	 * Traces the FlowCode document route from the nodes with the given internal
	 * ids, and returns the FlowPart to which:
	 * @li all the routes from the given nodes are eventually connected to downwards
	 * @li their exists one (possibly internally branched) route for each node to that part
	 * @param ids The list of internal ids of the nodes for the paths to begin from - if empty,
	 *			  all nodes internal nodes are used
	 * @param previousParts A list of parts in the calling tree. This avoids infinite recursion.
	 * @returns The first FlowPart satisfying these conditions, or NULL if no such part exists
	 */
	FlowPart* endPart( QStringList ids, FlowPartList *previousParts = 0l );
	/**
	 * Handles the addition of a if-else statement to the given FlowCode. This will
	 * order the code as necessary, adding the branches in the appropriate places
	 * @param code The FlowCode where the if-else will be added
	 * @param case1Statement The statement (e.g. "PORTA.0 is high") used for the first case
	 * @param case2Statement The logically opposite statement (e.g. "PORTA.0 is low") (note
	 						 that only one of the two statements will be used.
	 * @param case1 The internal node id for case1
	 * @param case2 The internal node id for case2
	 */
	void handleIfElse( FlowCode *code, const QString &case1Statement, const QString &case2Statement,
					   const QString &case1, const QString &case2 );
	/**
	 * Returns a pointer to the FlowPart that is connected to the node with the
	 * given internal id, or NULL if no such node / no connected part
	 */
	FlowPart* outputPart( const QString& internalNodeId );
	/**
	 * Returns the FlowParts connected to the given node
	 * @see outputPart
	 */
	FlowPartList inputParts( const QString& id );
	/**
	 * Returns a list of parts that are connected to *all* input parts
	 */
	FlowPartList inputParts();
	/**
	 * Returns a list of parts that are connected to *all* output parts. Note that if
	 * the same FlowPart is connected to more than one output, that flowpart will appear
	 * in the FlowPartList the number of times it is connected.
	 */
	FlowPartList outputParts();
	/**
	 * Draw the picture of the flowpart in the given orientation onto the pixmap
	 */
	void orientationPixmap( uint orientation, QPixmap & pm ) const;
	Variant * createProperty( const QString & id, Variant::Type::Value type ) override;

public slots:
	/**
	 * Called when variable name data for MicroSettings changes, and so our
	 * data needs updating
	 */
	void updateVarNames();
	/**
	 * Called when a variable name has changed (from an entry box)
	 */
	void varNameChanged( QVariant newValue, QVariant oldValue );
	/**
	 * Called when some of the FlowPart-specific variables (e.g. Pin or
	 * SevenSegment) requiring updating, such as when the PIC type changes
	 * or the list of Pin Maps changes.
	 */
	void slotUpdateFlowPartVariables();

protected:
	void updateAttachedPositioning() override;
	/**
	 * Removes the node ids that shouldn't be used for finding the end part
	 */
	virtual void filterEndPartIDs( QStringList *ids ) { Q_UNUSED(ids); }
	/**
	 * Normally just passes the paint request onto CNItem::drawShape,
	 * although in the case of some FlowSymbols (e.g. decision), it will handle
	 * the drawing itself
	 */
	void drawShape( QPainter &p ) override;

	/**
	 * Returns the goto instruction that will goto the FlowPart that is connected
	 * to the node with the given internal id.
	 * For example, gotoCode("stdOutput") might return "goto delay__13"
	 */
	QString gotoCode( const QString& internalNodeId );
	/**
	 * Creates a FPNode with an internal id of "stdinput".
	 * The node is positioned half-way along the top of the FlowPart,
	 * as determined by width(), height(), x() and y()
	 */
	void createStdInput();
	/**
	 * Creates a FPNode with an internal id of "stdoutput".
	 * The node is positioned half-way along the bottom of the FlowPart,
	 * as determined by width(), height(), x() and y()
	 */
	void createStdOutput();
	/**
	 * Creates a FPNode with an internal id of "altoutput".
	 * The node is positioned half-way along the right of the FlowPart,
	 * as determined by width(), height(), x() and y()
	 */
	void createAltOutput();
	/**
	 * Initialises a symbol, by calling setItemPoints with the shape
	 */
	void initSymbol( FlowPart::FlowSymbol symbol, int width = 48 );

	void initProcessSymbol() { initSymbol( FlowPart::ps_process ); }
	void initCallSymbol() { initSymbol( FlowPart::ps_call ); }
	void initIOSymbol() { initSymbol( FlowPart::ps_io ); }
	void initRoundedRectSymbol() { initSymbol( FlowPart::ps_round ); }
	void initDecisionSymbol() { initSymbol( FlowPart::ps_decision ); }

	QString m_caption;
	uint m_orientation;
	FPNode *m_stdInput;
	FPNode *m_stdOutput;
	FPNode *m_altOutput;
	QPointer<FlowCodeDocument> m_pFlowCodeDocument;
	
	void postResize() override;
	void updateNodePositions();

private:
	FlowSymbol m_flowSymbol;
};
typedef QList<FlowPart*> FlowPartList;

#endif



