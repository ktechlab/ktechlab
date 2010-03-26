/***************************************************************************
 *   Copyright (C) 2004-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MULTIINPUTGATE_H
#define MULTIINPUTGATE_H

#include "simplecomponent.h"
#include "logic.h"

const int maxGateInput = 256;

/**
@author David Saxton
*/
class MultiInputGate : public CallbackClass, public SimpleComponent {

public:
	/**
	 * @param rectangularShapeText is the text displayed in the logic symbol
	 * when drawing in rectangular mode, e.g. "&" for AND and "=1" for XOR.
	 * @param invertedOutput true for NAND, NOR, XNOR; false for AND, OR, XOR
	 * @param baseWidth is the width of the logic gate when drawing in
	 * distinctive mode.
	 * @param likeOR true for OR, NOR, XOR, XNOR (specifically, this value
	 * used in computer the length of input pins, as OR types have a curvy
	 * base when the shape is distinctive).
	 */
	MultiInputGate(ICNDocument *icnDocument, bool newItem, const char *id,
                        const QString & rectangularShapeText,
                        bool invertedOutput,
                        int baseWidth, bool likeOR);
	~MultiInputGate();

protected:
	enum LogicSymbolShape { Distinctive, Rectangular };

	virtual void updateAttachedPositioning();
	virtual void slotUpdateConfiguration();
	virtual void inStateChanged(bool newState) = 0;
	/**
	 * This will draw the shape if the logic symbol is currently
	 * rectangular. Distinctive shapes should be drawn in the respective
	 * subclasses.
	 */
	virtual void drawShape(QPainter &p);
	void dataChanged();
	void updateInputs(int newNum);
	/**
	 * @return what the width should be according to the current logic
	 * symbol shape.
	 */
	int logicSymbolShapeToWidth() const;
	/**
	 * Updates the display text for the logic symbol depending on its shape.
	 */
	void updateSymbolText();

	int m_distinctiveWidth;

// might pay to bundle these as a new class, or a container of structs or something.
	int m_numInputs;
	LogicIn *inLogic[maxGateInput];
	ECNode *inNode[maxGateInput];
// ###
	LogicOut m_pOut;
	LogicSymbolShape m_logicSymbolShape;
	QString m_rectangularShapeText;
	const bool m_bInvertedOutput;  // will be initialized by the constructor from a given parameter
	bool m_bLikeOR;

private:
	/**
	 * Reads the logic symbol shape from KTLConfig.
	 */
	void updateLogicSymbolShape();

	bool m_bDoneInit;
};

/**
@short Boolean XNOR
@author David Saxton
*/
class ECXnor : public MultiInputGate {

public:
	ECXnor(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECXnor();

	static Item* construct(ItemDocument *itemDocument, bool newItem,
	                       const char *id);
	static LibraryItem *libraryItem();

protected:
	void inStateChanged(bool newState);
	virtual void drawShape(QPainter &p);
};

/**
@short Boolean XOR
@author David Saxton
*/
class ECXor : public MultiInputGate {

public:
	ECXor(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECXor();

	static Item* construct(ItemDocument *itemDocument, bool newItem,
	                       const char *id);
	static LibraryItem *libraryItem();

protected:
	void inStateChanged(bool newState);
	virtual void drawShape(QPainter &p);
};

/**
@short Boolean OR
@author David Saxton
*/
class ECOr : public MultiInputGate {

public:
	ECOr(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECOr();

	static Item* construct(ItemDocument *itemDocument, bool newItem,
	                       const char *id);
	static LibraryItem *libraryItem();

protected:
	void inStateChanged(bool newState);
	virtual void drawShape(QPainter &p);
};

/**
@short Boolean NOR
@author David Saxton
*/
class ECNor : public MultiInputGate {

public:
	ECNor(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECNor();

	static Item* construct(ItemDocument *itemDocument, bool newItem,
	                       const char *id);
	static LibraryItem *libraryItem();

protected:
	void inStateChanged(bool newState);
	virtual void drawShape(QPainter &p);
};

/**
@short Boolean NAND
@author David Saxton
*/
class ECNand : public MultiInputGate {

public:
	ECNand(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECNand();

	static Item* construct(ItemDocument *itemDocument, bool newItem,
	                       const char *id);
	static LibraryItem *libraryItem();

protected:
	void inStateChanged(bool newState);
	virtual void drawShape(QPainter &p);
};

/**
@short Boolean AND
@author David Saxton
*/
class ECAnd : public MultiInputGate {

public:
	ECAnd(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECAnd();

	static Item* construct(ItemDocument *itemDocument, bool newItem,
	                       const char *id);
	static LibraryItem *libraryItem();

protected:
	void inStateChanged(bool newState);
	virtual void drawShape(QPainter &p);
};
#endif
