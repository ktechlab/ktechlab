/***************************************************************************
 *   Copyright (C) 2004-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "multiinputgate.h"
#include "ecnode.h"
#include "icndocument.h"
#include "libraryitem.h"
#include "logic.h"

#include <KLocalizedString>
#include <QPainter>
#include <cmath>

#include <algorithm>

#include <ktlconfig.h>

// BEGIN class MultiInputGate

void MultiInputGate_inStateChanged(void *objV, bool state) { // Enable
    MultiInputGate *objT = static_cast<MultiInputGate*>(objV);
    objT->inStateChanged(state);
}

MultiInputGate::MultiInputGate(ICNDocument *icnDocument, bool newItem, const char *id, const QString &rectangularShapeText, bool invertedOutput, int baseWidth, bool likeOR)
    : Component(icnDocument, newItem, id)
{
    m_bLikeOR = likeOR;
    m_bDoneInit = false;
    m_numInputs = 0;
    m_distinctiveWidth = baseWidth;
    m_bInvertedOutput = invertedOutput;
    m_rectangularShapeText = rectangularShapeText;

    for (int i = 0; i < maxGateInput; ++i) {
        inLogic[i] = nullptr;
        inNode[i] = nullptr;
    }

    updateLogicSymbolShape();

    updateInputs(2);

    init1PinRight(16);
    m_pOut = createLogicOut(m_pPNode[0], false);

    createProperty("numInput", Variant::Type::Int);
    property("numInput")->setCaption(i18n("Number Inputs"));
    property("numInput")->setMinValue(2);
    property("numInput")->setMaxValue(maxGateInput);
    property("numInput")->setValue(2);

    m_bDoneInit = true;
}

MultiInputGate::~MultiInputGate()
{
}

void MultiInputGate::slotUpdateConfiguration()
{
    updateLogicSymbolShape();
    Component::slotUpdateConfiguration();
}

void MultiInputGate::updateLogicSymbolShape()
{
    // Set the canvas changed for the old shape
    setChanged();

    if (KTLConfig::logicSymbolShapes() == KTLConfig::EnumLogicSymbolShapes::Distinctive) {
        m_logicSymbolShape = Distinctive;
        setSize(-m_distinctiveWidth / 2, offsetY(), m_distinctiveWidth, height(), true);
    } else {
        m_logicSymbolShape = Rectangular;
        setSize(-16, offsetY(), 32, height(), true);
    }

    updateSymbolText();
    updateAttachedPositioning();
    if (p_itemDocument)
        p_itemDocument->requestEvent(ItemDocument::ItemDocumentEvent::RerouteInvalidatedConnectors);

    // Set the canvas changed for the new shape
    setChanged();
}

void MultiInputGate::updateSymbolText()
{
    if (m_logicSymbolShape == Distinctive)
        removeDisplayText("rect-shape-text");
    else {
        int w = 32 - (m_bInvertedOutput ? 6 : 0);
        QRect r(-16, 4 - height() / 2, w, height() - 4);
        addDisplayText("rect-shape-text", r, m_rectangularShapeText, true, Qt::AlignTop | Qt::AlignHCenter);
    }
}

int MultiInputGate::logicSymbolShapeToWidth() const
{
    return (m_logicSymbolShape == Distinctive) ? m_distinctiveWidth : 32;
}

void MultiInputGate::dataChanged()
{
    updateInputs(std::min(maxGateInput, dataInt("numInput")));
}

void MultiInputGate::updateInputs(int newNum)
{
    if (newNum == m_numInputs)
        return;

    if (newNum < 2)
        newNum = 2;
    else if (newNum > maxGateInput)
        newNum = maxGateInput;

    int newWidth = logicSymbolShapeToWidth();

    QRect r(-newWidth / 2, -8 * newNum, newWidth, 16 * newNum);
    setSize(r, true);
    updateSymbolText();

    const bool added = (newNum > m_numInputs);
    if (added) {
        for (int i = m_numInputs; i < newNum; ++i) {
            ECNode *node = createPin(0, 0, 0, "in" + QString::number(i));
            inNode[i] = node;
            inLogic[i] = createLogicIn(node);
            //inLogic[i]->setCallback(this, (CallbackPtr)(&MultiInputGate::inStateChanged));
            inLogic[i]->setCallback2(MultiInputGate_inStateChanged, this);
        }
    } else {
        for (int i = newNum; i < m_numInputs; ++i) {
            removeNode("in" + QString::number(i));
            removeElement(inLogic[i], false);
            inNode[i] = nullptr;
            inLogic[i] = nullptr;
        }
    }

    m_numInputs = newNum;

    // We can't call a pure-virtual function if we haven't finished our constructor yet...
    if (m_bDoneInit)
        inStateChanged(!added);

    updateAttachedPositioning();
}

void MultiInputGate::updateAttachedPositioning()
{
    // Check that our ndoes have been created before we attempt to use them
    if (!m_nodeMap.contains("p1") || !m_nodeMap.contains("in" + QString::number(m_numInputs - 1)))
        return;

    int _x = offsetX() + 8;
    int _y = offsetY() + 8;

    m_nodeMap["p1"].x = logicSymbolShapeToWidth() / 2 + 8;
    m_nodeMap["p1"].y = 0;

    int n = m_numInputs;

    for (int i = 0; i < n; ++i) {
        m_nodeMap["in" + QString::number(i)].x = _x - 16;
        m_nodeMap["in" + QString::number(i)].y = _y + 16 * i;

        // The curvy part at the base of OR-like logic gates means that the
        // input needs need to be increased in length
        if (m_bLikeOR) {
            int length = 8;
            if (m_logicSymbolShape == Distinctive) {
                length += int(std::sqrt(double(64 * n * n - (8 * n - 8 - 16 * i) * (8 * n - 8 - 16 * i)))) / n;
            }
            inNode[i]->setLength(length);
        }
    }

    if (m_bDoneInit)
        Component::updateAttachedPositioning();
}

void MultiInputGate::drawShape(QPainter &p)
{
    initPainter(p);

    int _x = int(x() + offsetX());
    int _y = int(y() + offsetY());

    if (m_bInvertedOutput) {
        p.drawRect(_x, _y, 32 - 6, height());
        p.drawEllipse(_x + 32 - 6, int(y()) - 3, 6, 6);
    } else {
        p.drawRect(_x, _y, 32, height());
    }

    deinitPainter(p);
}
// END class MultiInputGate

// BEGIN class ECXNor
Item *ECXnor::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECXnor(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ECXnor::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/xnor")), i18n("XNOR gate"), i18n("Logic"), "xnor.png", LibraryItem::lit_component, ECXnor::construct);
}

ECXnor::ECXnor(ICNDocument *icnDocument, bool newItem, const char *id)
    : MultiInputGate(icnDocument, newItem, id ? id : "xnor", "=1", true, 48, true)
{
    m_name = i18n("XNOR gate");

    inStateChanged(false);
}

ECXnor::~ECXnor()
{
}

void ECXnor::inStateChanged(bool)
{
    int highCount = 0;
    for (int i = 0; i < m_numInputs; ++i) {
        if (inLogic[i]->isHigh())
            highCount++;
    }

    m_pOut->setHigh(highCount != 1);
}

void ECXnor::drawShape(QPainter &p)
{
    if (m_logicSymbolShape == Rectangular) {
        MultiInputGate::drawShape(p);
        return;
    }

    initPainter(p);
    int _x = int(x()) + offsetX();
    int _y = int(y()) + offsetY();

    p.save();
    p.setPen(Qt::NoPen);
    p.drawChord(_x - width() + 22, _y, 2 * width() - 28, height(), -16 * 81, 16 * 162);
    p.restore();

    p.drawArc(_x - width() + 22, _y, 2 * width() - 28, height(), -16 * 90, 16 * 180);
    p.drawArc(_x - 8, _y, 16, height(), -16 * 90, 16 * 180);
    p.drawArc(_x, _y, 16, height(), -16 * 90, 16 * 180);

    p.drawEllipse(_x + width() - 6, _y + (height() / 2) - 3, 6, 6);

    deinitPainter(p);
}
// END class ECXnor

// BEGIN class ECXor
Item *ECXor::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECXor(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ECXor::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/xor")), i18n("XOR gate"), i18n("Logic"), "xor.png", LibraryItem::lit_component, ECXor::construct);
}

ECXor::ECXor(ICNDocument *icnDocument, bool newItem, const char *id)
    : MultiInputGate(icnDocument, newItem, id ? id : "xor", "=1", false, 48, true)
{
    m_name = i18n("XOR gate");

    inStateChanged(false);
}

ECXor::~ECXor()
{
}

void ECXor::inStateChanged(bool)
{
    int highCount = 0;
    for (int i = 0; i < m_numInputs; ++i) {
        if (inLogic[i]->isHigh())
            highCount++;
    }

    m_pOut->setHigh(highCount == 1);
}

void ECXor::drawShape(QPainter &p)
{
    if (m_logicSymbolShape == Rectangular) {
        MultiInputGate::drawShape(p);
        return;
    }

    initPainter(p);
    int _x = int(x()) + offsetX();
    int _y = int(y()) + offsetY();

    p.save();
    p.setPen(Qt::NoPen);
    p.drawChord(_x - width() + 16, _y, 2 * width() - 16, height(), -16 * 81, 16 * 162);
    p.restore();

    p.drawArc(_x - width() + 16, _y, 2 * width() - 16, height(), -16 * 90, 16 * 180);
    p.drawArc(_x - 8, _y, 16, height(), -16 * 90, 16 * 180);
    p.drawArc(_x, _y, 16, height(), -16 * 90, 16 * 180);

    deinitPainter(p);
}
// END class ECXor

// BEGIN class ECOr
Item *ECOr::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECOr(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ECOr::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/or")), i18n("OR gate"), i18n("Logic"), "or.png", LibraryItem::lit_component, ECOr::construct);
}

ECOr::ECOr(ICNDocument *icnDocument, bool newItem, const char *id)
    : MultiInputGate(icnDocument, newItem, id ? id : "or", QChar(0x2265) + QString("1"), false, 48, true)
{
    m_name = i18n("OR gate");

    inStateChanged(false);
}

ECOr::~ECOr()
{
}

void ECOr::inStateChanged(bool)
{
    bool allLow = true;
    for (int i = 0; i < m_numInputs && allLow; ++i) {
        if (inLogic[i]->isHigh())
            allLow = false;
    }

    m_pOut->setHigh(!allLow);
}

void ECOr::drawShape(QPainter &p)
{
    if (m_logicSymbolShape == Rectangular) {
        MultiInputGate::drawShape(p);
        return;
    }

    initPainter(p);
    int _x = int(x()) + offsetX();
    int _y = int(y()) + offsetY();

    p.save();
    p.setPen(Qt::NoPen);
    // 	p.setBrush( Qt::red );
    p.drawChord(_x - width(), _y, 2 * width(), height(), -16 * 81, 16 * 162);
    // 	p.drawPie( _x-width()+16, _y, 2*width()-16, height(), -16*100, 16*200 );
    p.restore();

    p.drawArc(_x - width(), _y, 2 * width(), height(), -16 * 90, 16 * 180);
    p.drawArc(_x - 8, _y, 16, height(), -16 * 90, 16 * 180);

    deinitPainter(p);
}
// END class ECOr

// BEGIN class ECNor
Item *ECNor::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECNor(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ECNor::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/nor")), i18n("NOR gate"), i18n("Logic"), "nor.png", LibraryItem::lit_component, ECNor::construct);
}

ECNor::ECNor(ICNDocument *icnDocument, bool newItem, const char *id)
    : MultiInputGate(icnDocument, newItem, id ? id : "nor", QChar(0x2265) + QString("1"), true, 48, true)
{
    m_name = i18n("NOR Gate");

    inStateChanged(false);
}

ECNor::~ECNor()
{
}

void ECNor::inStateChanged(bool)
{
    bool allLow = true;
    for (int i = 0; i < m_numInputs && allLow; ++i) {
        if (inLogic[i]->isHigh())
            allLow = false;
    }

    m_pOut->setHigh(allLow);
}

void ECNor::drawShape(QPainter &p)
{
    if (m_logicSymbolShape == Rectangular) {
        MultiInputGate::drawShape(p);
        return;
    }

    initPainter(p);
    int _x = int(x()) + offsetX();
    int _y = int(y()) + offsetY();

    p.save();
    p.setPen(Qt::NoPen);
    p.drawChord(_x - width() + 6, _y, 2 * width() - 12, height(), -16 * 81, 16 * 162);
    p.restore();

    p.drawArc(_x - width() + 6, _y, 2 * width() - 12, height(), -16 * 90, 16 * 180);
    p.drawArc(_x - 8, _y, 16, height(), -16 * 90, 16 * 180);

    p.drawEllipse(_x + width() - 6, _y + (height() / 2) - 3, 6, 6);

    deinitPainter(p);
}
// END class ECNor

// BEGIN class ECNand
Item *ECNand::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECNand(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ECNand::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/nand")), i18n("NAND gate"), i18n("Logic"), "nand.png", LibraryItem::lit_component, ECNand::construct);
}

ECNand::ECNand(ICNDocument *icnDocument, bool newItem, const char *id)
    : MultiInputGate(icnDocument, newItem, id ? id : "nand", "&", true, 32, false)
{
    m_name = i18n("NAND Gate");

    inStateChanged(false);
}

ECNand::~ECNand()
{
}

void ECNand::inStateChanged(bool)
{
    for (int i = 0; i < m_numInputs; ++i) {
        if (!inLogic[i]->isHigh()) {
            m_pOut->setHigh(true);
            return;
        }
    }

    m_pOut->setHigh(false);
}

void ECNand::drawShape(QPainter &p)
{
    if (m_logicSymbolShape == Rectangular) {
        MultiInputGate::drawShape(p);
        return;
    }

    initPainter(p);
    int _x = int(x()) + offsetX();
    int _y = int(y()) + offsetY();
    p.drawChord(_x - width() + 6, _y, 2 * width() - 12, height(), -16 * 90, 16 * 180);
    p.drawEllipse(_x + width() - 6, _y + (height() / 2) - 3, 6, 6);
    deinitPainter(p);
}
// END class ECNand

// BEGIN class ECAnd
Item *ECAnd::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECAnd(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ECAnd::libraryItem()
{
    QStringList idList;
    idList << "ec/and"
           << "ec/and_2";
    return new LibraryItem(idList, i18n("AND gate"), i18n("Logic"), "and.png", LibraryItem::lit_component, ECAnd::construct);
}

ECAnd::ECAnd(ICNDocument *icnDocument, bool newItem, const char *id)
    : MultiInputGate(icnDocument, newItem, id ? id : "and", "&", false, 32, false)
{
    m_name = i18n("AND Gate");

    inStateChanged(false);
}

ECAnd::~ECAnd()
{
}

void ECAnd::inStateChanged(bool)
{
    for (int i = 0; i < m_numInputs; ++i) {
        if (!inLogic[i]->isHigh()) {
            m_pOut->setHigh(false);
            return;
        }
    }

    m_pOut->setHigh(true);
}

void ECAnd::drawShape(QPainter &p)
{
    if (m_logicSymbolShape == Rectangular) {
        MultiInputGate::drawShape(p);
        return;
    }

    initPainter(p);

    int _x = int(x()) + offsetX();
    int _y = int(y()) + offsetY();

    p.drawChord(_x - width(), _y, 2 * width(), height(), -16 * 90, 16 * 180);

    deinitPainter(p);
}
// END class ECAnd
