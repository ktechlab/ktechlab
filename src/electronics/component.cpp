/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "component.h"
#include "canvasitemparts.h"
#include "circuitdocument.h"
#include "ecnode.h"
#include "itemdocumentdata.h"
#include "node.h"
#include "pin.h"
#include "simulator.h"

#include "bjt.h"
#include "capacitance.h"
#include "cccs.h"
#include "ccvs.h"
#include "currentsignal.h"
#include "currentsource.h"
#include "diode.h"
#include "inductance.h"
#include "jfet.h"
#include "logic.h"
#include "mosfet.h"
#include "opamp.h"
#include "resistance.h"
#include "switch.h"
#include "vccs.h"
#include "vcvs.h"
#include "voltagepoint.h"
#include "voltagesignal.h"
#include "voltagesource.h"

#include <QBitArray>
#include <QMatrix>
#include <QPainter>
#include <QWidget>
#include <cmath>

#include <ktlconfig.h>
#include <ktechlab_debug.h>

// const int dipWidth = 112;    // 2017.10.01 - comment out unused constants
// const int pairSep = 32;

// Degrees per radian

Component::Component(ICNDocument *icnDocument, bool newItem, const QString &id)
    : CNItem(icnDocument, newItem, id)
    , m_angleDegrees(0)
    , b_flipped(false)
{
    m_pCircuitDocument = dynamic_cast<CircuitDocument *>(icnDocument);

    for (int i = 0; i < 4; ++i) {
        m_pPNode[i] = nullptr;
        m_pNNode[i] = nullptr;
    }

    // Get configuration options
    slotUpdateConfiguration();

    // And finally register this :-)
    if (icnDocument)
        icnDocument->registerItem(this);
}

Component::~Component()
{
    removeElements();
    if (!Simulator::isDestroyedSim()) {
        Simulator::self()->detachComponent(this);
    }
}

void Component::removeItem()
{
    if (b_deleted)
        return;
    if (!Simulator::isDestroyedSim()) {
        Simulator::self()->detachComponent(this);
    }
    CNItem::removeItem();
}

void Component::removeElements(bool setPinsInterIndependent)
{
    const ElementMapList::iterator end = m_elementMapList.end();
    for (ElementMapList::iterator it = m_elementMapList.begin(); it != end; ++it) {
        Element *e = (*it).e;
        if (e) {
            emit elementDestroyed(e);
            e->componentDeleted();
        }
    }
    m_elementMapList.clear();

    const SwitchList::iterator swEnd = m_switchList.end();
    for (SwitchList::iterator it = m_switchList.begin(); it != swEnd; ++it) {
        Switch *sw = *it;
        if (!sw)
            continue;

        emit switchDestroyed(sw);
        delete sw;
    }
    m_switchList.clear();

    if (setPinsInterIndependent)
        setAllPinsInterIndependent();
}

void Component::removeElement(Element *element, bool setPinsInterIndependent)
{
    if (!element)
        return;

    emit elementDestroyed(element);
    element->componentDeleted();

    const ElementMapList::iterator end = m_elementMapList.end();
    for (ElementMapList::iterator it = m_elementMapList.begin(); it != end;) {
        ElementMapList::iterator next = it;
        ++next;

        if ((*it).e == element)
            m_elementMapList.erase(it);

        it = next;
    }

    if (setPinsInterIndependent)
        rebuildPinInterDepedence();
}

void Component::removeSwitch(Switch *sw)
{
    if (!sw)
        return;

    emit switchDestroyed(sw);
    delete sw;
    m_switchList.removeAll(sw);
    m_pCircuitDocument->requestAssignCircuits();
}

void Component::setNodalCurrents()
{
    const ElementMapList::iterator end = m_elementMapList.end();
    for (ElementMapList::iterator it = m_elementMapList.begin(); it != end; ++it) {
        ElementMap m = (*it);
        for (int i = 0; i < 4; i++) {
            if (m.n[i]) {
                m.n[i]->mergeCurrent(m.e->m_cnodeI[i]);
            }
        }
    }
}

void Component::initPainter(QPainter &p)
{
    CNItem::initPainter(p);

    if (!b_flipped && (m_angleDegrees % 360 == 0))
        return;

    p.save();

    p.translate(int(x()), int(y()));
    if (b_flipped)
        p.scale(-1, 1);

    p.rotate(m_angleDegrees);
    p.translate(-int(x()), -int(y()));
}

void Component::deinitPainter(QPainter &p)
{
    if (!b_flipped && (m_angleDegrees % 360 == 0))
        return;

    p.restore();
}

void Component::setAngleDegrees(int degrees)
{
    if (!p_icnDocument)
        return;

    degrees = ((degrees % 360) + 360) % 360;
    if (m_angleDegrees == degrees)
        return;

    updateConnectorPoints(false);
    m_angleDegrees = degrees;
    itemPointsChanged();
    updateAttachedPositioning();
    p_icnDocument->requestRerouteInvalidatedConnectors();

    emit orientationChanged();
}

void Component::setFlipped(bool flipped)
{
    if (!p_icnDocument)
        return;

    if (flipped == b_flipped)
        return;

    updateConnectorPoints(false);
    b_flipped = flipped;
    itemPointsChanged();
    updateAttachedPositioning();
    p_icnDocument->requestRerouteInvalidatedConnectors();

    emit orientationChanged();
}

void Component::itemPointsChanged()
{
    QPolygon transformedPoints = transMatrix(m_angleDegrees, b_flipped, 0, 0, false).map(m_itemPoints);
    // 	transformedPoints.translate( int(x()), int(y()) );
    setPoints(transformedPoints);
}

void Component::restoreFromItemData(const ItemData &itemData)
{
    CNItem::restoreFromItemData(itemData);

    setAngleDegrees(int(itemData.angleDegrees));
    setFlipped(itemData.flipped);
}

ItemData Component::itemData() const
{
    ItemData itemData = CNItem::itemData();
    itemData.angleDegrees = m_angleDegrees;
    itemData.flipped = b_flipped;
    return itemData;
}

QMatrix Component::transMatrix(int angleDegrees, bool flipped, int x, int y, bool inverse)
{
    QMatrix m;
    m.translate(x, y);
    if (inverse) {
        m.rotate(-angleDegrees);
        if (flipped)
            m.scale(-1, 1);
    } else {
        if (flipped)
            m.scale(-1, 1);
        m.rotate(angleDegrees);
    }
    m.translate(-x, -y);
    // m.setTransformationMode( QMatrix::Areas ); // TODO find a replacement
    return m;
}

void Component::finishedCreation()
{
    CNItem::finishedCreation();
    updateAttachedPositioning();
}

void Component::updateAttachedPositioning()
{
    const double RPD = M_PI / 180.0;

    if (b_deleted || !m_bDoneCreation)
        return;

    // BEGIN Transform the nodes
    const NodeInfoMap::iterator end = m_nodeMap.end();
    for (NodeInfoMap::iterator it = m_nodeMap.begin(); it != end; ++it) {
        if (!it.value().node)
            qCCritical(KTL_LOG) << "Node in nodemap is null";
        else {
            int nx = int((std::cos(m_angleDegrees * RPD) * it.value().x) - (std::sin(m_angleDegrees * RPD) * it.value().y));
            int ny = int((std::sin(m_angleDegrees * RPD) * it.value().x) + (std::cos(m_angleDegrees * RPD) * it.value().y));

            if (b_flipped)
                nx = -nx;

#define round_8(x) (((x) > 0) ? int(((x) + 4) / 8) * 8 : int(((x)-4) / 8) * 8)
            nx = round_8(nx);
            ny = round_8(ny);
#undef round_8

            int newDir = (((m_angleDegrees + it.value().orientation) % 360) + 360) % 360;
            if (b_flipped)
                newDir = (((180 - newDir) % 360) + 360) % 360;

            it.value().node->move(nx + x(), ny + y());
            it.value().node->setOrientation(newDir);
        }
    }
    // END Transform the nodes

    // BEGIN Transform the GuiParts
    QMatrix m;

    if (b_flipped)
        m.scale(-1, 1);
    m.rotate(m_angleDegrees);
    // m.setTransformationMode( QMatrix::Areas ); // TODO find a replacement

    const TextMap::iterator textMapEnd = m_textMap.end();
    for (TextMap::iterator it = m_textMap.begin(); it != textMapEnd; ++it) {
        QRect newPos = m.mapRect(it.value()->recommendedRect());
        it.value()->move(newPos.x() + x(), newPos.y() + y());
        it.value()->setGuiPartSize(newPos.width(), newPos.height());
        it.value()->setAngleDegrees(m_angleDegrees);
    }
    const WidgetMap::iterator widgetMapEnd = m_widgetMap.end();
    for (WidgetMap::iterator it = m_widgetMap.begin(); it != widgetMapEnd; ++it) {
        QRect newPos = m.mapRect(it.value()->recommendedRect());
        it.value()->move(newPos.x() + x(), newPos.y() + y());
        it.value()->setGuiPartSize(newPos.width(), newPos.height());
        it.value()->setAngleDegrees(m_angleDegrees);
    }
    // END Transform the GuiParts
}

void Component::drawPortShape(QPainter &p)
{
    int h = height();
    int w = width() - 1;
    int _x = int(x() + offsetX());
    int _y = int(y() + offsetY());

    double roundSize = 8;
    double slantIndent = 8;

    const double DPR = 180.0 / M_PI;
    double inner = std::atan(h / slantIndent); // Angle for slight corner
    double outer = M_PI - inner;               // Angle for sharp corner

    int inner16 = int(16 * inner * DPR);
    int outer16 = int(16 * outer * DPR);

    p.save();
    p.setPen(Qt::NoPen);
    p.drawPolygon(areaPoints());
    p.restore();

    initPainter(p);

    // Left line
    p.drawLine(int(_x), int(_y + roundSize / 2), int(_x), int(_y + h - roundSize / 2));

    // Right line
    p.drawLine(int(_x + w), int(_y - slantIndent + h - roundSize / 2), int(_x + w), int(_y + slantIndent + roundSize / 2));

    // Bottom line
    p.drawLine(
        int(_x + (1 - std::cos(outer)) * (roundSize / 2)), int(_y + h + (std::sin(outer) - 1) * (roundSize / 2)), int(_x + w + (std::cos(inner) - 1) * (roundSize / 2)), int(_y + h - slantIndent + (std::sin(inner) - 1) * (roundSize / 2)));

    // Top line
    p.drawLine(int(_x + w + (std::cos(outer) - 1) * (roundSize / 2)), int(_y + slantIndent + (1 - std::sin(inner)) * (roundSize / 2)), int(_x + (1 - std::cos(inner)) * (roundSize / 2)), int(_y + (1 - std::sin(outer)) * (roundSize / 2)));

    // Top left
    p.drawArc(int(_x), int(_y), int(roundSize), int(roundSize), 90 * 16, outer16);

    // Bottom left
    p.drawArc(int(_x), int(_y + h - roundSize), int(roundSize), int(roundSize), 180 * 16, outer16);

    // Top right
    p.drawArc(int(_x + w - roundSize), int(_y + slantIndent), int(roundSize), int(roundSize), 0, inner16);

    // Bottom right
    p.drawArc(int(_x + w - roundSize), int(_y - slantIndent + h - roundSize), int(roundSize), int(roundSize), 270 * 16, inner16);

    deinitPainter(p);
}

void Component::initDIP(const QStringList &pins)
{
    const int numPins = pins.size();
    const int numSide = numPins / 2 + numPins % 2;

    // Pins along left
    for (int i = 0; i < numSide; i++) {
        if (!pins[i].isEmpty()) {
            const int nodeX = -8 + offsetX();
            const int nodeY = (i + 1) * 16 + offsetY();
            ECNode *node = ecNodeWithID(pins[i]);
            if (node) {
                m_nodeMap[pins[i]].x = nodeX;
                m_nodeMap[pins[i]].y = nodeY;
                m_nodeMap[pins[i]].orientation = 0;
            } else
                createPin(nodeX, nodeY, 0, pins[i]);
        }
    }
    // Pins along right
    for (int i = numSide; i < numPins; i++) {
        if (!pins[i].isEmpty()) {
            const int nodeX = width() + 8 + offsetX();
            const int nodeY = (2 * numSide - i) * 16 + offsetY();
            ECNode *node = ecNodeWithID(pins[i]);
            if (node) {
                m_nodeMap[pins[i]].x = nodeX;
                m_nodeMap[pins[i]].y = nodeY;
                m_nodeMap[pins[i]].orientation = 180;
            } else
                createPin(nodeX, nodeY, 180, pins[i]);
        }
    }

    updateAttachedPositioning();
}

void Component::initDIPSymbol(const QStringList &pins, int _width)
{
    const int numPins = pins.size();
    const int numSide = numPins / 2 + numPins % 2;

    setSize(-(_width - (_width % 16)) / 2, -(numSide + 1) * 8, _width, (numSide + 1) * 16, true);

    // 2015.01.11 - do not use painter
    //     QWidget tmpWidget;
    //     //tmpWidget.setAttribute(Qt::WA_PaintOutsidePaintEvent, true); // note: add this if needed
    //     //QPainter p(&tmpWidget);
    //     QPainter p;
    //     const bool isSuccess = p.begin(&tmpWidget);
    //     if (!isSuccess) {
    //         qCWarning(KTL_LOG) << " painter not active";
    //     }

    // p.setFont( font() ); // 2015.01.11 - do not use painter
    QFontMetrics fontMetrics(font());

    // Pins along left
    for (int i = 0; i < numSide; i++) {
        if (!pins[i].isEmpty()) {
            const QString text = pins.at(i);

            const int _top = (i + 1) * 16 - 8 + offsetY();
            const int _width = width() / 2 - 6;
            const int _left = 6 + offsetX();
            const int _height = 16;

            // QRect br = p.boundingRect( QRect( _left, _top, _width, _height ), Qt::AlignLeft, text ); // 2015.01.11 - do not use painter
            QRect br = fontMetrics.boundingRect(QRect(_left, _top, _width, _height), Qt::AlignLeft, text);

            addDisplayText(text, br, text);
        }
    }
    // Pins along right
    for (int i = numSide; i < numPins; i++) {
        if (!pins[i].isEmpty()) {
            const QString text = pins.at(i);

            const int _top = (2 * numSide - i) * 16 - 8 + offsetY();
            const int _width = width() / 2 - 6;
            const int _left = (width() / 2) + offsetX();
            const int _height = 16;

            // QRect br = p.boundingRect( QRect( _left, _top, _width, _height ), Qt::AlignRight, text ); // 2015.01.11 - do not use painter
            QRect br = fontMetrics.boundingRect(QRect(_left, _top, _width, _height), Qt::AlignLeft, text);
            addDisplayText(text, br, text);
        }
    }

    updateAttachedPositioning();
}

void Component::init1PinLeft(int h1)
{
    if (h1 == -1)
        h1 = offsetY() + height() / 2;

    m_pNNode[0] = createPin(offsetX() - 8, h1, 0, "n1");
}

void Component::init2PinLeft(int h1, int h2)
{
    if (h1 == -1)
        h1 = offsetY() + 8;
    if (h2 == -1)
        h2 = offsetY() + height() - 8;

    m_pNNode[0] = createPin(offsetX() - 8, h1, 0, "n1");
    m_pNNode[1] = createPin(offsetX() - 8, h2, 0, "n2");
}

void Component::init3PinLeft(int h1, int h2, int h3)
{
    if (h1 == -1)
        h1 = offsetY() + 8;
    if (h2 == -1)
        h2 = offsetY() + height() / 2;
    if (h3 == -1)
        h3 = offsetY() + height() - 8;

    m_pNNode[0] = createPin(offsetX() - 8, h1, 0, "n1");
    m_pNNode[1] = createPin(offsetX() - 8, h2, 0, "n2");
    m_pNNode[2] = createPin(offsetX() - 8, h3, 0, "n3");
}

void Component::init4PinLeft(int h1, int h2, int h3, int h4)
{
    if (h1 == -1)
        h1 = offsetY() + 8;
    if (h2 == -1)
        h2 = offsetY() + 24;
    if (h3 == -1)
        h3 = offsetY() + height() - 24;
    if (h4 == -1)
        h4 = offsetY() + height() - 8;

    m_pNNode[0] = createPin(offsetX() - 8, h1, 0, "n1");
    m_pNNode[1] = createPin(offsetX() - 8, h2, 0, "n2");
    m_pNNode[2] = createPin(offsetX() - 8, h3, 0, "n3");
    m_pNNode[3] = createPin(offsetX() - 8, h4, 0, "n4");
}

void Component::init1PinRight(int h1)
{
    if (h1 == -1)
        h1 = offsetY() + height() / 2;

    m_pPNode[0] = createPin(offsetX() + width() + 8, h1, 180, "p1");
}

void Component::init2PinRight(int h1, int h2)
{
    if (h1 == -1)
        h1 = offsetY() + 8;
    if (h2 == -1)
        h2 = offsetY() + height() - 8;

    m_pPNode[0] = createPin(offsetX() + width() + 8, h1, 180, "p1");
    m_pPNode[1] = createPin(offsetX() + width() + 8, h2, 180, "p2");
}

void Component::init3PinRight(int h1, int h2, int h3)
{
    if (h1 == -1)
        h1 = offsetY() + 8;
    if (h2 == -1)
        h2 = offsetY() + height() / 2;
    if (h3 == -1)
        h3 = offsetY() + height() - 8;

    m_pPNode[0] = createPin(offsetX() + width() + 8, h1, 180, "p1");
    m_pPNode[1] = createPin(offsetX() + width() + 8, h2, 180, "p2");
    m_pPNode[2] = createPin(offsetX() + width() + 8, h3, 180, "p3");
}

void Component::init4PinRight(int h1, int h2, int h3, int h4)
{
    if (h1 == -1)
        h1 = offsetY() + 8;
    if (h2 == -1)
        h2 = offsetY() + 24;
    if (h3 == -1)
        h3 = offsetY() + height() - 24;
    if (h4 == -1)
        h4 = offsetY() + height() - 8;

    m_pPNode[0] = createPin(offsetX() + width() + 8, h1, 180, "p1");
    m_pPNode[1] = createPin(offsetX() + width() + 8, h2, 180, "p2");
    m_pPNode[2] = createPin(offsetX() + width() + 8, h3, 180, "p3");
    m_pPNode[3] = createPin(offsetX() + width() + 8, h4, 180, "p4");
}

ECNode *Component::ecNodeWithID(const QString &ecNodeId)
{
    if (!p_icnDocument) {
        // 		qCDebug(KTL_LOG) << "Warning: ecNodeWithID("<<ecNodeId<<") does not exist";
        return createPin(0, 0, 0, ecNodeId);
    }

    return dynamic_cast<ECNode *>(p_icnDocument->nodeWithID(nodeId(ecNodeId)));
}

void Component::slotUpdateConfiguration()
{
    const LogicConfig logicConfig = LogicIn::getConfig();

    const ElementMapList::iterator end = m_elementMapList.end();
    for (ElementMapList::iterator it = m_elementMapList.begin(); it != end; ++it) {
        if (LogicIn *logicIn = dynamic_cast<LogicIn *>((*it).e))
            logicIn->setLogic(logicConfig);
    }
}

BJT *Component::createBJT(ECNode *c, ECNode *b, ECNode *e, bool isNPN)
{
    return createBJT(c->pin(), b->pin(), e->pin(), isNPN);
}

Capacitance *Component::createCapacitance(ECNode *n0, ECNode *n1, double capacitance)
{
    return createCapacitance(n0->pin(), n1->pin(), capacitance);
}

CCCS *Component::createCCCS(ECNode *n0, ECNode *n1, ECNode *n2, ECNode *n3, double gain)
{
    return createCCCS(n0->pin(), n1->pin(), n2->pin(), n3->pin(), gain);
}

CCVS *Component::createCCVS(ECNode *n0, ECNode *n1, ECNode *n2, ECNode *n3, double gain)
{
    return createCCVS(n0->pin(), n1->pin(), n2->pin(), n3->pin(), gain);
}

CurrentSignal *Component::createCurrentSignal(ECNode *n0, ECNode *n1, double current)
{
    return createCurrentSignal(n0->pin(), n1->pin(), current);
}

CurrentSource *Component::createCurrentSource(ECNode *n0, ECNode *n1, double current)
{
    return createCurrentSource(n0->pin(), n1->pin(), current);
}

Diode *Component::createDiode(ECNode *n0, ECNode *n1)
{
    return createDiode(n0->pin(), n1->pin());
}

JFET *Component::createJFET(ECNode *D, ECNode *G, ECNode *S, int JFET_type)
{
    return createJFET(D->pin(), G->pin(), S->pin(), JFET_type);
}

Inductance *Component::createInductance(ECNode *n0, ECNode *n1, double inductance)
{
    return createInductance(n0->pin(), n1->pin(), inductance);
}

LogicIn *Component::createLogicIn(ECNode *node)
{
    return createLogicIn(node->pin());
}

LogicOut *Component::createLogicOut(ECNode *node, bool isHigh)
{
    return createLogicOut(node->pin(), isHigh);
}

MOSFET *Component::createMOSFET(ECNode *D, ECNode *G, ECNode *S, ECNode *B, int MOSFET_type)
{
    return createMOSFET(D->pin(), G->pin(), S->pin(), B ? B->pin() : nullptr, MOSFET_type);
}

OpAmp *Component::createOpAmp(ECNode *nonInverting, ECNode *out, ECNode *inverting)
{
    return createOpAmp(nonInverting->pin(), out->pin(), inverting->pin());
}

Resistance *Component::createResistance(ECNode *n0, ECNode *n1, double resistance)
{
    return createResistance(n0->pin(), n1->pin(), resistance);
}

Switch *Component::createSwitch(ECNode *n0, ECNode *n1, bool open)
{
    return createSwitch(n0->pin(), n1->pin(), open);
}

VCCS *Component::createVCCS(ECNode *n0, ECNode *n1, ECNode *n2, ECNode *n3, double gain)
{
    return createVCCS(n0->pin(), n1->pin(), n2->pin(), n3->pin(), gain);
}

VCVS *Component::createVCVS(ECNode *n0, ECNode *n1, ECNode *n2, ECNode *n3, double gain)
{
    return createVCVS(n0->pin(), n1->pin(), n2->pin(), n3->pin(), gain);
}

VoltagePoint *Component::createVoltagePoint(ECNode *n0, double voltage)
{
    return createVoltagePoint(n0->pin(), voltage);
}

VoltageSignal *Component::createVoltageSignal(ECNode *n0, ECNode *n1, double voltage)
{
    return createVoltageSignal(n0->pin(), n1->pin(), voltage);
}

VoltageSource *Component::createVoltageSource(ECNode *n0, ECNode *n1, double voltage)
{
    return createVoltageSource(n0->pin(), n1->pin(), voltage);
}

BJT *Component::createBJT(Pin *cN, Pin *bN, Pin *eN, bool isNPN)
{
    BJT *e = new BJT(isNPN);

    QList<Pin *> pins;
    pins << bN << cN << eN;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

Capacitance *Component::createCapacitance(Pin *n0, Pin *n1, double capacitance)
{
    Capacitance *e = new Capacitance(capacitance, LINEAR_UPDATE_PERIOD);

    QList<Pin *> pins;
    pins << n0 << n1;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

CCCS *Component::createCCCS(Pin *n0, Pin *n1, Pin *n2, Pin *n3, double gain)
{
    CCCS *e = new CCCS(gain);

    QList<Pin *> pins;
    pins << n0 << n1 << n2 << n3;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

CCVS *Component::createCCVS(Pin *n0, Pin *n1, Pin *n2, Pin *n3, double gain)
{
    CCVS *e = new CCVS(gain);

    QList<Pin *> pins;
    pins << n0 << n1 << n2 << n3;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterCircuitDependent(it, pins);

    pins.clear();
    pins << n0 << n1;
    setInterGroundDependent(it, pins);

    pins.clear();
    pins << n2 << n3;
    setInterGroundDependent(it, pins);

    return e;
}

CurrentSignal *Component::createCurrentSignal(Pin *n0, Pin *n1, double current)
{
    CurrentSignal *e = new CurrentSignal(LINEAR_UPDATE_PERIOD, current);

    QList<Pin *> pins;
    pins << n0 << n1;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

CurrentSource *Component::createCurrentSource(Pin *n0, Pin *n1, double current)
{
    CurrentSource *e = new CurrentSource(current);

    QList<Pin *> pins;
    pins << n0 << n1;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

Diode *Component::createDiode(Pin *n0, Pin *n1)
{
    Diode *e = new Diode();

    QList<Pin *> pins;
    pins << n0 << n1;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

JFET *Component::createJFET(Pin *D, Pin *G, Pin *S, int JFET_type)
{
    JFET *e = new JFET((JFET::JFET_type)JFET_type);

    QList<Pin *> pins;
    pins << D << G << S;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

Inductance *Component::createInductance(Pin *n0, Pin *n1, double inductance)
{
    Inductance *e = new Inductance(inductance, LINEAR_UPDATE_PERIOD);

    QList<Pin *> pins;
    pins << n0 << n1;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

LogicIn *Component::createLogicIn(Pin *node)
{
    LogicIn *e = new LogicIn(LogicIn::getConfig());

    QList<Pin *> pins;
    pins << node;

    ElementMapList::iterator it = handleElement(e, pins);
    return e;
}

LogicOut *Component::createLogicOut(Pin *node, bool isHigh)
{
    LogicOut *e = new LogicOut(LogicIn::getConfig(), isHigh);

    QList<Pin *> pins;
    pins << node;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

MOSFET *Component::createMOSFET(Pin *D, Pin *G, Pin *S, Pin *B, int MOSFET_type)
{
    MOSFET *e = new MOSFET((MOSFET::MOSFET_type)MOSFET_type);

    QList<Pin *> pins;
    pins << D << G << S << B;

    /// \todo remove the following line removing body if null
    pins.removeAll(nullptr);

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

OpAmp *Component::createOpAmp(Pin *nonInverting, Pin *inverting, Pin *out)
{
    OpAmp *e = new OpAmp();

    QList<Pin *> pins;
    pins << nonInverting << inverting << out;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

Resistance *Component::createResistance(Pin *n0, Pin *n1, double resistance)
{
    Resistance *e = new Resistance(resistance);

    QList<Pin *> pins;
    pins << n0 << n1;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

Switch *Component::createSwitch(Pin *n0, Pin *n1, bool open)
{
    // Note that a Switch is not really an element (although in many cases it
    // behaves very much like one).

    Switch *e = new Switch(this, n0, n1, open ? Switch::Open : Switch::Closed);
    m_switchList.append(e);
    n0->addSwitch(e);
    n1->addSwitch(e);
    emit switchCreated(e);
    return e;
}

VCCS *Component::createVCCS(Pin *n0, Pin *n1, Pin *n2, Pin *n3, double gain)
{
    VCCS *e = new VCCS(gain);

    QList<Pin *> pins;
    pins << n0 << n1 << n2 << n3;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

VCVS *Component::createVCVS(Pin *n0, Pin *n1, Pin *n2, Pin *n3, double gain)
{
    VCVS *e = new VCVS(gain);

    QList<Pin *> pins;
    pins << n0 << n1 << n2 << n3;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterCircuitDependent(it, pins);

    pins.clear();
    pins << n0 << n1;
    setInterGroundDependent(it, pins);

    pins.clear();
    pins << n2 << n3;
    setInterGroundDependent(it, pins);
    return e;
}

VoltagePoint *Component::createVoltagePoint(Pin *n0, double voltage)
{
    VoltagePoint *e = new VoltagePoint(voltage);

    QList<Pin *> pins;
    pins << n0;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

VoltageSignal *Component::createVoltageSignal(Pin *n0, Pin *n1, double voltage)
{
    VoltageSignal *e = new VoltageSignal(LINEAR_UPDATE_PERIOD, voltage);

    QList<Pin *> pins;
    pins << n0 << n1;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

VoltageSource *Component::createVoltageSource(Pin *n0, Pin *n1, double voltage)
{
    VoltageSource *e = new VoltageSource(voltage);

    QList<Pin *> pins;
    pins << n0 << n1;

    ElementMapList::iterator it = handleElement(e, pins);
    setInterDependent(it, pins);
    return e;
}

ElementMapList::iterator Component::handleElement(Element *e, const QList<Pin *> &pins)
{
    if (!e)
        return m_elementMapList.end();

    ElementMap em;
    em.e = e;
    int at = 0;
    QList<Pin *>::ConstIterator end = pins.end();
    for (QList<Pin *>::ConstIterator it = pins.begin(); it != end; ++it) {
        (*it)->addElement(e);
        em.n[at++] = *it;
    }

    // ElementMapList::iterator it = m_elementMapList.append(em);
    ElementMapList::iterator it = m_elementMapList.insert(m_elementMapList.end(), em);

    emit elementCreated(e);
    return it;
}

void Component::setInterDependent(ElementMapList::iterator it, const QList<Pin *> &pins)
{
    setInterCircuitDependent(it, pins);
    setInterGroundDependent(it, pins);
}

void Component::setInterCircuitDependent(ElementMapList::iterator it, const QList<Pin *> &pins)
{
    QList<Pin *>::ConstIterator end = pins.end();
    for (QList<Pin *>::ConstIterator it1 = pins.begin(); it1 != end; ++it1) {
        for (QList<Pin *>::ConstIterator it2 = pins.begin(); it2 != end; ++it2) {
            (*it1)->addCircuitDependentPin(*it2);
        }
    }

    (*it).interCircuitDependent.append(pins);
}

void Component::setInterGroundDependent(ElementMapList::iterator it, const QList<Pin *> &pins)
{
    QList<Pin *>::ConstIterator end = pins.end();
    for (QList<Pin *>::ConstIterator it1 = pins.begin(); it1 != end; ++it1) {
        for (QList<Pin *>::ConstIterator it2 = pins.begin(); it2 != end; ++it2) {
            (*it1)->addGroundDependentPin(*it2);
        }
    }

    (*it).interGroundDependent.append(pins);
}

void Component::rebuildPinInterDepedence()
{
    setAllPinsInterIndependent();

    // Rebuild dependencies
    ElementMapList::iterator emlEnd = m_elementMapList.end();
    for (ElementMapList::iterator it = m_elementMapList.begin(); it != emlEnd; ++it) {
        // Many copies of the pin lists as these will be affected when we call setInter*Dependent
        PinListList list = (*it).interCircuitDependent;

        PinListList::iterator depEnd = list.end();
        for (PinListList::iterator depIt = list.begin(); depIt != depEnd; ++depIt)
            setInterCircuitDependent(it, *depIt);

        list = (*it).interGroundDependent;

        depEnd = list.end();
        for (PinListList::iterator depIt = list.begin(); depIt != depEnd; ++depIt)
            setInterGroundDependent(it, *depIt);
    }
}

void Component::setAllPinsInterIndependent()
{
    NodeInfoMap::iterator nmEnd = m_nodeMap.end();
    for (NodeInfoMap::iterator it = m_nodeMap.begin(); it != nmEnd; ++it) {
        // PinVector pins = (static_cast<ECNode*>(it.value().node))->pins();
        ECNode *node = dynamic_cast<ECNode *>(it.value().node);
        if (!node) {
            qCWarning(KTL_LOG) << "skipping not-ECNode node: " << it.value().node;
            continue;
        }
        PinVector pins = node->pins();
        PinVector::iterator pinsEnd = pins.end();
        for (PinVector::iterator pinsIt = pins.begin(); pinsIt != pinsEnd; ++pinsIt) {
            if (*pinsIt)
                (*pinsIt)->removeDependentPins();
        }
    }
}

void Component::initElements(const uint stage)
{
    /// @todo this function is ugly and messy and needs tidying up

    const ElementMapList::iterator end = m_elementMapList.end();

    if (stage == 1) {
        for (ElementMapList::iterator it = m_elementMapList.begin(); it != end; ++it) {
            (*it).e->add_initial_dc();
        }
        return;
    }

    for (ElementMapList::iterator it = m_elementMapList.begin(); it != end; ++it) {
        ElementMap m = (*it);

        if (m.n[3]) {
            m.e->setCNodes(m.n[0]->eqId(), m.n[1]->eqId(), m.n[2]->eqId(), m.n[3]->eqId());
        } else if (m.n[2]) {
            m.e->setCNodes(m.n[0]->eqId(), m.n[1]->eqId(), m.n[2]->eqId());
        } else if (m.n[1]) {
            m.e->setCNodes(m.n[0]->eqId(), m.n[1]->eqId());
        } else if (m.n[0]) {
            m.e->setCNodes(m.n[0]->eqId());
        }
    }
}

ECNode *Component::createPin(double x, double y, int orientation, const QString &name)
{
    return dynamic_cast<ECNode *>(createNode(x, y, orientation, name, Node::ec_pin));
}

// static
double Component::voltageLength(double v)
{
    double v_max = 1e+1;
    double v_min = 1e-1;

    v = std::abs(v);

    if (v >= v_max)
        return 1.0;
    else if (v <= v_min)
        return 0.0;
    else
        return std::log(v / v_min) / std::log(v_max / v_min);
}

// static
QColor Component::voltageColor(double v)
{
    double prop = voltageLength(v);

    if (v >= 0)
        return QColor(int(255 * prop), int(166 * prop), 0);
    else
        return QColor(0, int(136 * prop), int(255 * prop));
}

// BEGIN class ElementMap
ElementMap::ElementMap()
{
    e = nullptr;
    for (int i = 0; i < 4; ++i)
        n[i] = nullptr;
}
// END class ElementMap
