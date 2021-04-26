/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "flowpart.h"
#include "canvasitemparts.h"
#include "connector.h"
#include "flowcode.h"
#include "flowcodedocument.h"
#include "fpnode.h"
#include "inputflownode.h"
#include "itemdocument.h"
#include "itemdocumentdata.h"
#include "micropackage.h"
#include "microsettings.h"
#include "picinfo.h"
#include "pinmapping.h"
#include "variant.h"

#include <QBitArray>
#include <QBitmap>
#include <QPainter>
#include <QPixmap>
#include <QRegExp>

#include <algorithm>
#include <cassert>
#include <cmath>

#include <ktechlab_debug.h>

// Degrees per radian
const double DPR = 57.29577951308232087665461840231273527024;

// The following arrays of numbers represent the positions of nodes in different configurations,
// with the numbers as NodeInfo::Position.

int diamondNodePositioning[8][3] = {{270, 90, 0}, {270, 90, 180}, {270, 0, 90}, {270, 0, 180}, {180, 0, 90}, {180, 0, 270}, {180, 90, 0}, {180, 90, 270}};

int inOutNodePositioning[8][2] = {{270, 90},
                                  {270, 0},
                                  {270, 180},
                                  {0, 0}, // (invalid)
                                  {180, 0},
                                  {180, 90},
                                  {180, 270},
                                  {0, 0}}; // (invalid)

int inNodePositioning[4] = {270, 0, 90, 180};

int outNodePositioning[4] = {90, 180, 270, 0};

FlowPart::FlowPart(ICNDocument *icnDocument, bool newItem, const QString &id)
    : CNItem(icnDocument, newItem, id)
{
    m_flowSymbol = FlowPart::ps_other;
    m_orientation = 0;
    m_stdInput = nullptr;
    m_stdOutput = nullptr;
    m_altOutput = nullptr;

    if (icnDocument) {
        icnDocument->registerItem(this);
        m_pFlowCodeDocument = dynamic_cast<FlowCodeDocument *>(icnDocument);
        assert(m_pFlowCodeDocument);

        connect(m_pFlowCodeDocument, &FlowCodeDocument::picTypeChanged, this, &FlowPart::slotUpdateFlowPartVariables);
        connect(m_pFlowCodeDocument, &FlowCodeDocument::pinMappingsChangedFlowCode, this, &FlowPart::slotUpdateFlowPartVariables);
    }
}

FlowPart::~FlowPart()
{
    // We have to check view, as if the item is deleted before the CNItem constructor
    // is called, then there will be no view
    if (m_pFlowCodeDocument) {
        const VariantDataMap::iterator end = m_variantData.end();
        for (VariantDataMap::iterator it = m_variantData.begin(); it != end; ++it) {
            Variant *v = it.value();
            if (v)
                m_pFlowCodeDocument->varNameChanged("", v->value().toString());
        }
    }
}

void FlowPart::setCaption(const QString &caption)
{
    if (m_flowSymbol == FlowPart::ps_other) {
        m_caption = caption;
        return;
    }

    // 2016.05.03 - do not use temporary widget for getting font metrics
    // 	QWidget *w = new QWidget();
    // 	//QPainter p(w);
    //     QPainter p;
    //     const bool isSuccess = p.begin(w);
    //     if (!isSuccess) {
    //         qCWarning(KTL_LOG) << " painter not active";
    //     }
    // 	p.setFont( font() );
    // 	const int text_width = p.boundingRect( boundingRect(), (Qt::SingleLine | Qt::AlignHCenter | Qt::AlignVCenter), caption ).width();
    // 	p.end();
    // 	delete w;

    QFontMetrics fontMetrics(font());
    const int text_width = fontMetrics.boundingRect(boundingRect(), (Qt::TextSingleLine | Qt::AlignHCenter | Qt::AlignVCenter), caption).width();

    int width = std::max(((int)(text_width / 16)) * 16, 48);

    switch (m_flowSymbol) {
    case FlowPart::ps_call:
        width += 48;
        break;
    case FlowPart::ps_io:
    case FlowPart::ps_round:
        width += 32;
        break;
    case FlowPart::ps_decision:
        width += 64;
        break;
    case FlowPart::ps_process:
    default:
        width += 32;
        break;
    }

    bool hasSideConnectors = m_flowSymbol == FlowPart::ps_decision;
    if (hasSideConnectors && (width != this->width()))
        p_icnDocument->requestRerouteInvalidatedConnectors();

    initSymbol(m_flowSymbol, width);
    m_caption = caption;
}
void FlowPart::postResize()
{
    updateNodePositions();
    CNItem::postResize();
}

void FlowPart::createStdInput()
{
    m_stdInput = (FPNode *)createNode(0, 0, 270, "stdinput", Node::fp_in);
    updateNodePositions();
}
void FlowPart::createStdOutput()
{
    m_stdOutput = (FPNode *)createNode(0, 0, 90, "stdoutput", Node::fp_out);
    updateNodePositions();
}
void FlowPart::createAltOutput()
{
    m_altOutput = (FPNode *)createNode(0, 0, 0, "altoutput", Node::fp_out);
    updateNodePositions();
}

void FlowPart::initSymbol(FlowPart::FlowSymbol symbol, int width)
{
    m_flowSymbol = symbol;

    switch (symbol) {
    case FlowPart::ps_other:
        return;
    case FlowPart::ps_call:
    case FlowPart::ps_process:
        setItemPoints(QRect(-width / 2, -16, width, 24));
        break;
    case FlowPart::ps_io: {
        // define parallelogram shape
        QPolygon pa(4);
        pa[0] = QPoint(-(width - 10) / 2, -16);
        pa[1] = QPoint(width / 2, -16);
        pa[2] = QPoint((width - 10) / 2, 8);
        pa[3] = QPoint(-width / 2, 8);
        setItemPoints(pa);
        break;
    }
    case FlowPart::ps_round: {
        // define rounded rectangles as two semicricles with RP_NUM/2 points with gap inbetween
        // These points are not used for drawing; merely for passing to qcanvaspolygonitem for collision detection
        // If there is a better way for a rounder rectangle + collision detection, please let me know...

        int halfHeight = 12;

        // Draw semicircle
        double x;
        const int RP_NUM = 48;
        QPolygon pa(RP_NUM);
        int point = 0;
        for (double y = -1.0; y <= 1.0; y += 4.0 / (RP_NUM - 2)) {
            x = sqrt(1 - y * y) * halfHeight;
            pa[point] = QPoint((int)(width + x) - halfHeight, (int)(halfHeight * y));
            pa[RP_NUM - 1 - point] = QPoint((int)(halfHeight - x), (int)(halfHeight * y));
            point++;
        }

        pa.translate(-width / 2, 4);
        setItemPoints(pa);
        break;
    }

    case FlowPart::ps_decision: {
        // define rhombus
        QPolygon pa(6);
        pa[0] = QPoint(0, -24);
        pa[1] = QPoint(width / 2, -6);
        pa[2] = QPoint(width / 2, 6);
        pa[3] = QPoint(0, 24);
        pa[4] = QPoint(-width / 2, 6);
        pa[5] = QPoint(-width / 2, -6);
        setItemPoints(pa);
        break;
    }
    default:
        qCCritical(KTL_LOG) << "Unknown flowSymbol: " << symbol;
    }
}

void FlowPart::drawShape(QPainter &p)
{
    initPainter(p);

    const double _x = int(x() + offsetX());
    const double _y = int(y() + offsetY());
    const double w = width();
    double h = height();

    switch (m_flowSymbol) {
    case FlowPart::ps_other:
        CNItem::drawShape(p);
        break;
    case FlowPart::ps_io: {
        h--;
        double roundSize = 8;
        double slantIndent = 5;

        // 		CNItem::drawShape(p);
        double inner = std::atan(h / slantIndent);
        double outer = M_PI - inner;

        int inner16 = int(16 * inner * DPR);
        int outer16 = int(16 * outer * DPR);

        p.save();
        p.setPen(Qt::NoPen);
        p.drawPolygon(areaPoints());
        p.restore();

        p.drawLine(int(_x + slantIndent + roundSize / 2), int(_y), int(_x + w - roundSize / 2), int(_y));
        p.drawLine(int(_x + w - slantIndent - roundSize / 2), int(_y + h), int(_x + roundSize / 2), int(_y + h));
        p.drawLine(
            int(_x + w + (std::sin(outer) - 1) * roundSize / 2), int(_y + (1 - std::cos(outer)) * roundSize / 2), int(_x + w - slantIndent + (std::sin(inner) - 1) * roundSize / 2), int(_y + h + (std::cos(inner) - 1) * roundSize / 2));
        p.drawLine(int(_x + (1 - std::sin(outer)) * roundSize / 2), int(_y + h + (std::cos(outer) - 1) * roundSize / 2), int(_x + slantIndent + (1 - std::sin(inner)) * roundSize / 2), int(_y + (1 - std::cos(inner)) * roundSize / 2));
        p.drawArc(int(_x + slantIndent), int(_y), int(roundSize), int(roundSize), 90 * 16, inner16);
        p.drawArc(int(_x + w - roundSize), int(_y), int(roundSize), int(roundSize), 270 * 16 + inner16, outer16);
        p.drawArc(int(_x - slantIndent + w - roundSize), int(_y + h - roundSize), int(roundSize), int(roundSize), 270 * 16, inner16);
        p.drawArc(int(_x), int(_y + h - roundSize), int(roundSize), int(roundSize), 90 * 16 + inner16, outer16);
        break;
    }
    case FlowPart::ps_decision:
        // TODO Make the shape nice and pretty with rounded corners
        CNItem::drawShape(p);
        break;
    case FlowPart::ps_call:
        p.drawRoundRect(int(_x), int(_y), int(w), int(h + 1), int(1000. / w), int(1000. / h));
        p.drawLine(int(_x + 8), int(_y), int(_x + 8), int(_y + h));
        p.drawLine(int(_x + w - 8), int(_y), int(_x + w - 8), int(_y + h));
        break;
    case FlowPart::ps_process:
        p.drawRoundRect(int(_x), int(_y), int(w), int(h + 1), int(1000. / w), int(1000. / h));
        break;
    case FlowPart::ps_round:
        p.drawRoundRect(int(_x), int(_y), int(w), int(h + 1), 30, 100);
        break;
    }

    p.setPen(Qt::black);
    p.setFont(font());
    p.drawText(boundingRect(), (Qt::TextWordWrap | Qt::AlignHCenter | Qt::AlignVCenter), m_caption);
}

QString FlowPart::gotoCode(const QString &internalNodeId)
{
    FlowPart *end = outputPart(internalNodeId);
    if (!end)
        return "";
    return "goto " + end->id();
}

FlowPart *FlowPart::outputPart(const QString &internalNodeId)
{
    Node *node = p_icnDocument->nodeWithID(nodeId(internalNodeId));

    FPNode *fpnode = dynamic_cast<FPNode *>(node);
    // FIXME dynamic_cast used to replace fpnode::type() call
    if (!fpnode || (dynamic_cast<InputFlowNode *>(fpnode) != nullptr))
        // if ( !fpnode || fpnode->type() == Node::fp_in )
        return nullptr;

    return fpnode->outputFlowPart();
}

FlowPartList FlowPart::inputParts(const QString &id)
{
    Node *node = p_icnDocument->nodeWithID(id);

    if (FPNode *fpNode = dynamic_cast<FPNode *>(node))
        return fpNode->inputFlowParts();

    return FlowPartList();
}

FlowPartList FlowPart::inputParts()
{
    FlowPartList list;

    const NodeInfoMap::iterator nEnd = m_nodeMap.end();
    for (NodeInfoMap::iterator it = m_nodeMap.begin(); it != nEnd; ++it) {
        Node *node = p_icnDocument->nodeWithID(it.value().id);
        FlowPartList newList;

        if (FPNode *fpNode = dynamic_cast<FPNode *>(node))
            newList = fpNode->inputFlowParts();

        const FlowPartList::iterator nlEnd = newList.end();
        for (FlowPartList::iterator it = newList.begin(); it != nlEnd; ++it) {
            if (*it)
                list.append(*it);
        }
    }

    return list;
}

FlowPartList FlowPart::outputParts()
{
    FlowPartList list;

    const NodeInfoMap::iterator end = m_nodeMap.end();
    for (NodeInfoMap::iterator it = m_nodeMap.begin(); it != end; ++it) {
        FlowPart *part = outputPart(it.key());
        if (part)
            list.append(part);
    }

    return list;
}

FlowPart *FlowPart::endPart(QStringList ids, FlowPartList *previousParts)
{
    if (ids.empty()) {
        const NodeInfoMap::iterator end = m_nodeMap.end();
        for (NodeInfoMap::iterator it = m_nodeMap.begin(); it != end; ++it) {
            ids.append(it.key());
        }
        filterEndPartIDs(&ids);
    }

    const bool createdList = (!previousParts);
    if (createdList) {
        previousParts = new FlowPartList;
    } else if (previousParts->contains(this)) {
        return nullptr;
    }

    previousParts->append(this);

    if (ids.empty()) {
        return nullptr;
    }

    if (ids.size() == 1) {
        return outputPart(*(ids.begin()));
    }

    typedef QList<FlowPartList> ValidPartsList;
    ValidPartsList validPartsList;

    const QStringList::iterator idsEnd = ids.end();
    for (QStringList::iterator it = ids.begin(); it != idsEnd; ++it) {
        //int prevLevel = level();
        FlowPartList validParts;
        FlowPart *part = outputPart(*it);
        while (part) {
            if (!validParts.contains(part)) {
                validParts.append(part);
                // 				if ( part->level() >= level() ) {
                //const int _l = part->level();
                part = part->endPart(QStringList(), previousParts);
                //prevLevel = _l;
                // 				} else {
                // 					part = nullptr;
                // 				}
            } else {
                part = nullptr;
            }
        }
        if (!validParts.empty()) {
            validPartsList.append(validParts);
        }
    }

    if (createdList) {
        delete previousParts;
        previousParts = nullptr;
    }

    if (validPartsList.empty())
        return nullptr;

    FlowPartList firstList = *(validPartsList.begin());
    const FlowPartList::iterator flEnd = firstList.end();
    const ValidPartsList::iterator vplEnd = validPartsList.end();
    for (FlowPartList::iterator it = firstList.begin(); it != flEnd; ++it) {
        bool ok = true;
        for (ValidPartsList::iterator vplit = validPartsList.begin(); vplit != vplEnd; ++vplit) {
            if (!(*vplit).contains(*it))
                ok = false;
        }
        if (ok)
            return *it;
    }

    return nullptr;
}

void FlowPart::handleIfElse(FlowCode *code, const QString &case1Statement, const QString &case2Statement, const QString &case1, const QString &case2)
{
    if (!code)
        return;

    FlowPart *stop = nullptr;
    FlowPart *part1 = outputPart(case1);
    FlowPart *part2 = outputPart(case2);

    if (part1 && part2)
        stop = endPart((QStringList(case1) << case2));

    if ((!part1 && !part2) || (part1 == stop && part2 == stop))
        return;

    code->addStopPart(stop);

    if (part1 && part1 != stop && code->isValidBranch(part1)) {
        // Use the case1 statement
        code->addCode("if " + case1Statement + " then " + "\n{");
        code->addCodeBranch(part1);
        code->addCode("}");

        if (part2 && part2 != stop && code->isValidBranch(part2)) {
            code->addCode("else\n{");
            code->addCodeBranch(part2);
            code->addCode("}");
        }
    } else if (code->isValidBranch(part2)) {
        // Use the case2 statement
        code->addCode("if " + case2Statement + " then " + "\n{");
        code->addCodeBranch(part2);
        code->addCode("}");
    }

    code->removeStopPart(stop);
    code->addCodeBranch(stop);
}

Variant *FlowPart::createProperty(const QString &id, Variant::Type::Value type)
{
    if (type != Variant::Type::Port && type != Variant::Type::Pin && type != Variant::Type::VarName && type != Variant::Type::SevenSegment && type != Variant::Type::KeyPad)
        return CNItem::createProperty(id, type);

    Variant *v = createProperty(id, Variant::Type::String);
    v->setType(type);

    if (type == Variant::Type::VarName) {
        if (m_pFlowCodeDocument) {
            if (MicroSettings *settings = m_pFlowCodeDocument->microSettings())
                v->setAllowed(settings->variableNames());
        }
        connect(property(id), qOverload<QVariant, QVariant>(&Property::valueChanged), this, &FlowPart::varNameChanged);
    } else
        slotUpdateFlowPartVariables();

    return v;
}

void FlowPart::slotUpdateFlowPartVariables()
{
    if (!m_pFlowCodeDocument)
        return;

    MicroSettings *s = m_pFlowCodeDocument->microSettings();
    if (!s)
        return;

    const PinMappingMap pinMappings = s->pinMappings();
    QStringList sevenSegMaps;
    QStringList keyPadMaps;

    PinMappingMap::const_iterator pEnd = pinMappings.end();
    for (PinMappingMap::const_iterator it = pinMappings.begin(); it != pEnd; ++it) {
        switch (it.value().type()) {
        case PinMapping::SevenSegment:
            sevenSegMaps << it.key();
            break;

        case PinMapping::Keypad_4x3:
        case PinMapping::Keypad_4x4:
            keyPadMaps << it.key();
            break;

        case PinMapping::Invalid:
            break;
        }
    }

    QStringList ports = s->microInfo()->package()->portNames();
    ports.sort();

    QStringList pins = s->microInfo()->package()->pinIDs(PicPin::type_bidir | PicPin::type_input | PicPin::type_open);
    pins.sort();

    const VariantDataMap::iterator vEnd = m_variantData.end();
    for (VariantDataMap::iterator it = m_variantData.begin(); it != vEnd; ++it) {
        Variant *v = it.value();
        if (!v)
            continue;

        if (v->type() == Variant::Type::Port)
            v->setAllowed(ports);
        else if (v->type() == Variant::Type::Pin)
            v->setAllowed(pins);
        else if (v->type() == Variant::Type::SevenSegment) {
            v->setAllowed(sevenSegMaps);
            if (!sevenSegMaps.isEmpty() && !sevenSegMaps.contains(v->value().toString()))
                v->setValue(sevenSegMaps.first());
        } else if (v->type() == Variant::Type::KeyPad) {
            v->setAllowed(keyPadMaps);
            if (!keyPadMaps.isEmpty() && !keyPadMaps.contains(v->value().toString()))
                v->setValue(keyPadMaps.first());
        }
    }
}

void FlowPart::updateVarNames()
{
    if (!m_pFlowCodeDocument)
        return;

    MicroSettings *s = m_pFlowCodeDocument->microSettings();
    if (!s)
        return;

    const QStringList names = s->variableNames();
    const VariantDataMap::iterator end = m_variantData.end();
    for (VariantDataMap::iterator it = m_variantData.begin(); it != end; ++it) {
        Variant *v = it.value();
        if (v && v->type() == Variant::Type::VarName)
            v->setAllowed(names);
    }
}

void FlowPart::varNameChanged(QVariant newValue, QVariant oldValue)
{
    if (!m_pFlowCodeDocument)
        return;
    m_pFlowCodeDocument->varNameChanged(newValue.toString(), oldValue.toString());
}

inline int nodeDirToPos(int dir)
{
    switch (dir) {
    case 0:
        return 0;
    case 270:
        return 1;
    case 180:
        return 2;
    case 90:
        return 3;
    }
    return 0;
}

void FlowPart::updateAttachedPositioning()
{
    if (b_deleted)
        return;

    // BEGIN Rearrange text if appropriate
    const QRect textPos[4] = {QRect(offsetX() + width(), 6, 40, 16), QRect(0, offsetY() - 16, 40, 16), QRect(offsetX() - 40, 6, 40, 16), QRect(0, offsetY() + height(), 40, 16)};

    NodeInfo *stdOutputInfo = m_stdOutput ? &m_nodeMap["stdoutput"] : nullptr;
    NodeInfo *altOutputInfo = m_altOutput ? &m_nodeMap["altoutput"] : nullptr;

    Text *outputTrueText = m_textMap.contains("output_true") ? m_textMap["output_true"] : nullptr;
    Text *outputFalseText = m_textMap.contains("output_false") ? m_textMap["output_false"] : nullptr;

    if (stdOutputInfo && outputTrueText)
        outputTrueText->setOriginalRect(textPos[nodeDirToPos(stdOutputInfo->orientation)]);

    if (altOutputInfo && outputFalseText)
        outputFalseText->setOriginalRect(textPos[nodeDirToPos(altOutputInfo->orientation)]);

    const TextMap::iterator textMapEnd = m_textMap.end();
    for (TextMap::iterator it = m_textMap.begin(); it != textMapEnd; ++it) {
        QRect pos = it.value()->recommendedRect();
        it.value()->move(pos.x() + x(), pos.y() + y());
        it.value()->setGuiPartSize(pos.width(), pos.height());
    }
    // END Rearrange text if appropriate

    const NodeInfoMap::iterator end = m_nodeMap.end();
    for (NodeInfoMap::iterator it = m_nodeMap.begin(); it != end; ++it) {
        if (!it.value().node) {
            qCCritical(KTL_LOG) << "Node in nodemap is null";
            continue;
        }

        double nx = it.value().x;
        double ny = it.value().y;

#define round_8(x) (((x) > 0) ? int(((x) + 4) / 8) * 8 : int(((x)-4) / 8) * 8)
        nx = round_8(nx);
        ny = round_8(ny);
#undef round_8

        it.value().node->move(int(nx + x()), int(ny + y()));
        it.value().node->setOrientation(it.value().orientation);
    }
}

ItemData FlowPart::itemData() const
{
    ItemData itemData = CNItem::itemData();
    itemData.orientation = m_orientation;
    return itemData;
}

void FlowPart::restoreFromItemData(const ItemData &itemData)
{
    CNItem::restoreFromItemData(itemData);
    if (itemData.orientation >= 0)
        setOrientation(uint(itemData.orientation));
}

void FlowPart::updateNodePositions()
{
    if (m_orientation > 7) {
        qCWarning(KTL_LOG) << "Invalid orientation: " << m_orientation;
        return;
    }

    NodeInfo *stdInputInfo = m_stdInput ? &m_nodeMap["stdinput"] : nullptr;
    NodeInfo *stdOutputInfo = m_stdOutput ? &m_nodeMap["stdoutput"] : nullptr;
    NodeInfo *altOutputInfo = m_altOutput ? &m_nodeMap["altoutput"] : nullptr;

    if (m_stdInput && m_stdOutput && m_altOutput) {
        stdInputInfo->orientation = diamondNodePositioning[m_orientation][0];
        stdOutputInfo->orientation = diamondNodePositioning[m_orientation][1];
        altOutputInfo->orientation = diamondNodePositioning[m_orientation][2];
    } else if (m_stdInput && m_stdOutput) {
        stdInputInfo->orientation = inOutNodePositioning[m_orientation][0];
        stdOutputInfo->orientation = inOutNodePositioning[m_orientation][1];
    } else if (m_orientation < 4) {
        if (stdInputInfo)
            stdInputInfo->orientation = inNodePositioning[m_orientation];
        else if (stdOutputInfo)
            stdOutputInfo->orientation = outNodePositioning[m_orientation];
    } else {
        qCWarning(KTL_LOG) << "Invalid orientation: " << m_orientation;
        return;
    }

    const NodeInfoMap::iterator end = m_nodeMap.end();
    for (NodeInfoMap::iterator it = m_nodeMap.begin(); it != end; ++it) {
        if (!it.value().node)
            qCCritical(KTL_LOG) << "Node in nodemap is null";
        else {
            switch (it.value().orientation) {
            case 0:
                it.value().x = offsetX() + width() + 8;
                it.value().y = 0;
                break;
            case 270:
                it.value().x = 0;
                it.value().y = offsetY() - 8;
                break;
            case 180:
                it.value().x = offsetX() - 8;
                it.value().y = 0;
                break;
            case 90:
                it.value().x = 0;
                it.value().y = offsetY() + height() + 8;
                ;
                break;
            }
        }
    }

    updateAttachedPositioning();
}

void FlowPart::setOrientation(uint orientation)
{
    if (orientation == m_orientation)
        return;

    m_orientation = orientation;
    updateNodePositions();
    p_icnDocument->requestRerouteInvalidatedConnectors();
}

uint FlowPart::allowedOrientations() const
{
    // The bit positions shown here represent whether or not that orientation is allowed, the orientation being
    // what is displayed in the i'th position (0 to 3 on top, 4 to 7 on bottom) of orientation widget

    if (m_stdInput && m_stdOutput && m_altOutput)
        return 255;

    if (m_stdInput && m_stdOutput)
        return 119;

    if (m_stdInput || m_stdOutput)
        return 15;

    return 0;
}

void FlowPart::orientationPixmap(uint orientation, QPixmap &pm) const
{
    const QSize size = pm.size();

    if (!(allowedOrientations() & (1 << orientation))) {
        qCWarning(KTL_LOG) << "Requesting invalid orientation of " << orientation;
        return;
    }

    QBitmap mask(50, 50);
    // QPainter maskPainter(&mask); // 2016.05.03 - initialize painter explicitly
    QPainter maskPainter;
    {
        const bool isSuccess = maskPainter.begin(&mask);
        if (!isSuccess) {
            qCWarning(KTL_LOG) << " painter not active";
        }
    }

    mask.fill(Qt::color0);
    maskPainter.setBrush(Qt::color1);
    maskPainter.setPen(Qt::color1);

    // BEGIN painter on pm
    {
        // QPainter p(&pm); // 2016.05.03 - explicitly initialize painter
        QPainter p;
        const bool isBeginSuccess = p.begin(&pm);
        if (!isBeginSuccess) {
            qCWarning(KTL_LOG) << " painter not active";
        }
        p.setBrush(m_brushCol);
        p.setPen(Qt::black);

        // In order: right corner, top corner, left corner, bottom corner

        QPoint c[4] = {QPoint(int(0.7 * size.width()), int(0.5 * size.height())),
                       QPoint(int(0.5 * size.width()), int(0.4 * size.height())),
                       QPoint(int(0.3 * size.width()), int(0.5 * size.height())),
                       QPoint(int(0.5 * size.width()), int(0.6 * size.height()))};

        QPoint d[4];
        d[0] = c[0] + QPoint(7, 0);
        d[1] = c[1] + QPoint(0, -7);
        d[2] = c[2] + QPoint(-7, 0);
        d[3] = c[3] + QPoint(0, 7);

        if (m_stdInput && m_stdOutput && m_altOutput) {
            // BEGIN Draw diamond outline
            QPolygon diamond(4);
            for (uint i = 0; i < 4; ++i)
                diamond[i] = c[i];

            p.drawPolygon(diamond);
            maskPainter.drawPolygon(diamond);
            // END Draw diamond outline

            // BEGIN Draw input
            int pos0 = nodeDirToPos(diamondNodePositioning[orientation][0]);
            p.drawLine(c[pos0], d[pos0]);
            maskPainter.drawLine(c[pos0], d[pos0]);
            // END Draw input

            // BEGIN Draw "true" output as a tick
            QPolygon tick(4);
            tick[0] = QPoint(-3, 0);
            tick[1] = QPoint(0, 2);
            tick[2] = QPoint(0, 2);
            tick[3] = QPoint(4, -2);

            int pos1 = nodeDirToPos(diamondNodePositioning[orientation][1]);
            tick.translate(d[pos1].x(), d[pos1].y());
            p.drawLines(tick);
            maskPainter.drawLines(tick);
            // END Draw "true" output as a tick

            // BEGIN Draw "false" output as a cross
            QPolygon cross(4);
            cross[0] = QPoint(-2, -2);
            cross[1] = QPoint(2, 2);
            cross[2] = QPoint(-2, 2);
            cross[3] = QPoint(2, -2);

            int pos2 = nodeDirToPos(diamondNodePositioning[orientation][2]);
            cross.translate(d[pos2].x(), d[pos2].y());
            p.drawLines(cross);
            maskPainter.drawLines(cross);
            // END Draw "false" output as a cross
        } else if (m_stdInput || m_stdOutput) {
            p.drawRoundRect(int(0.3 * size.width()), int(0.4 * size.height()), int(0.4 * size.width()), int(0.2 * size.height()));
            maskPainter.drawRoundRect(int(0.3 * size.width()), int(0.4 * size.height()), int(0.4 * size.width()), int(0.2 * size.height()));

            int hal = 5; // half arrow length
            int haw = 3; // half arrow width

            QPoint arrows[4][6] = {{QPoint(hal, 0), QPoint(0, -haw), QPoint(hal, 0), QPoint(-hal, 0), QPoint(hal, 0), QPoint(0, haw)},

                                   {QPoint(0, -hal), QPoint(-haw, 0), QPoint(0, -hal), QPoint(0, hal), QPoint(0, -hal), QPoint(haw, 0)},

                                   {QPoint(-hal, 0), QPoint(0, -haw), QPoint(-hal, 0), QPoint(hal, 0), QPoint(-hal, 0), QPoint(0, haw)},

                                   {QPoint(0, hal), QPoint(-haw, 0), QPoint(0, hal), QPoint(0, -hal), QPoint(0, hal), QPoint(haw, 0)}};

            int inPos = -1;
            int outPos = -1;

            if (m_stdInput && m_stdOutput) {
                inPos = nodeDirToPos(inOutNodePositioning[orientation][0]);
                outPos = nodeDirToPos(inOutNodePositioning[orientation][1]);
            } else if (m_stdInput) {
                inPos = nodeDirToPos(inNodePositioning[orientation]);
            } else if (m_stdOutput) {
                outPos = nodeDirToPos(outNodePositioning[orientation]);
            }

            if (inPos != -1) {
                QPolygon inArrow(6);
                for (int i = 0; i < 6; ++i) {
                    inArrow[i] = arrows[(inPos + 2) % 4][i];
                }
                inArrow.translate(d[inPos].x(), d[inPos].y());
                p.drawPolygon(inArrow);
                maskPainter.drawPolygon(inArrow);
            }

            if (outPos != -1) {
                QPolygon outArrow(6);
                for (int i = 0; i < 6; ++i) {
                    outArrow[i] = arrows[outPos][i];
                }
                outArrow.translate(d[outPos].x(), d[outPos].y());
                p.drawPolygon(outArrow);
                maskPainter.drawPolygon(outArrow);
            }
        }
    }
    // END painter on pm
    pm.setMask(mask); // pm needs not to have active painters on it
}
