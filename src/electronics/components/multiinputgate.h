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

#include "component.h"
#include "logic.h"

const int maxGateInput = 256;

/**
@author David Saxton
*/
class MultiInputGate : public CallbackClass, public Component
{
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
    MultiInputGate(ICNDocument *icnDocument, bool newItem, const char *id, const QString &rectangularShapeText, bool invertedOutput, int baseWidth, bool likeOR);
    ~MultiInputGate() override;

protected:
    enum LogicSymbolShape { Distinctive, Rectangular };

    void updateAttachedPositioning() override;
    void slotUpdateConfiguration() override;
public: // internal interfaces
    virtual void inStateChanged(bool newState) = 0;
protected:
    /**
     * This will draw the shape if the logic symbol is currently
     * rectangular. Distinctive shapes should be drawn in the respective
     * subclasses.
     */
    void drawShape(QPainter &p) override;
    void dataChanged() override;
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

    int m_numInputs;
    int m_distinctiveWidth;
    LogicIn *inLogic[maxGateInput];
    ECNode *inNode[maxGateInput];
    LogicOut *m_pOut;
    LogicSymbolShape m_logicSymbolShape;
    QString m_rectangularShapeText;
    bool m_bInvertedOutput;
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
class ECXnor : public MultiInputGate
{
public:
    ECXnor(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECXnor() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

protected:
    void inStateChanged(bool newState) override;
    void drawShape(QPainter &p) override;
};

/**
@short Boolean XOR
@author David Saxton
*/
class ECXor : public MultiInputGate
{
public:
    ECXor(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECXor() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

protected:
    void inStateChanged(bool newState) override;
    void drawShape(QPainter &p) override;
};

/**
@short Boolean OR
@author David Saxton
*/
class ECOr : public MultiInputGate
{
public:
    ECOr(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECOr() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

protected:
    void inStateChanged(bool newState) override;
    void drawShape(QPainter &p) override;
};

/**
@short Boolean NOR
@author David Saxton
*/
class ECNor : public MultiInputGate
{
public:
    ECNor(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECNor() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

protected:
    void inStateChanged(bool newState) override;
    void drawShape(QPainter &p) override;
};

/**
@short Boolean NAND
@author David Saxton
*/
class ECNand : public MultiInputGate
{
public:
    ECNand(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECNand() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

protected:
    void inStateChanged(bool newState) override;
    void drawShape(QPainter &p) override;
};

/**
@short Boolean AND
@author David Saxton
*/
class ECAnd : public MultiInputGate
{
public:
    ECAnd(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECAnd() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

protected:
    void inStateChanged(bool newState) override;
    void drawShape(QPainter &p) override;
};

#endif
