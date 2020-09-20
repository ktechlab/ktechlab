/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "flowcontainer.h"
#include "canvasitemparts.h"
#include "cells.h"
#include "fpnode.h"
#include "icndocument.h"
#include "nodegroup.h"
#include "resizeoverlay.h"

#include <QIcon>
#include <QPainter>

#include <cmath>

const int topStrip = 24;
const int botStrip = 16;

FlowContainer::FlowContainer(ICNDocument *_icnDocument, bool newItem, const QString &id)
    : FlowPart(_icnDocument, newItem, id)
{
    m_ext_in = m_int_in = m_int_out = m_ext_out = nullptr;
    b_expanded = true;

    addButton("expandBtn", QRect(offsetX(), offsetY() + 24 - 11, 22, 22), QIcon::fromTheme("go-down"), true);
    m_rectangularOverlay = new RectangularOverlay(this, 8, 8);
    setSize(-160, -120, 320, 240);

    m_int_in = (FPNode *)createNode(width() / 2, 8 + topStrip, 90, "int_in", Node::fp_out);
    m_int_out = (FPNode *)createNode(width() / 2, height() - 8 - botStrip, 270, "int_out", Node::fp_in);

    button("expandBtn")->setState(true);

    updateAttachedPositioning();
    updateNodeLevels();
}

FlowContainer::~FlowContainer()
{
}

void FlowContainer::updateNodeLevels()
{
    FlowPart::updateNodeLevels();

    int l = level();

    if (m_ext_in)
        m_ext_in->setLevel(l);
    if (m_ext_out)
        m_ext_out->setLevel(l);

    if (m_int_in)
        m_int_in->setLevel(l + 1);
    if (m_int_out)
        m_int_out->setLevel(l + 1);
}

void FlowContainer::filterEndPartIDs(QStringList *ids)
{
    // Remove *all* nodes except for very bottom one
    if (m_int_out) {
        ids->removeAll(m_int_out->childId());
    }
    if (m_ext_in) {
        ids->removeAll(m_ext_in->childId());
    }
    if (m_int_in) {
        ids->removeAll(m_int_in->childId());
    }
}

void FlowContainer::createTopContainerNode()
{
    m_ext_in = (FPNode *)createNode(width() / 2, -8, 270, "ext_in", Node::fp_in);
    m_ext_in->setLevel(level());
    m_rectangularOverlay->removeTopMiddle();
    updateAttachedPositioning();
}

void FlowContainer::createBotContainerNode()
{
    m_ext_out = (FPNode *)createNode(width() / 2, height() + 8, 90, "ext_out", Node::fp_out);
    m_ext_out->setLevel(level());
    m_rectangularOverlay->removeBotMiddle();
    updateAttachedPositioning();
}

QSize FlowContainer::minimumSize() const
{
    return QSize(160, 64);
}

void FlowContainer::drawShape(QPainter &p)
{
    if (b_deleted)
        return;

    if (!m_sizeRect.isValid())
        return;

    const int _x = (int)x() + offsetX();
    const int _y = (int)y() + offsetY();

    int col = 0xef + level() * 0x6;
    if (col > 0xff)
        col = 0xff;
    p.setBrush(QColor(col, 0xff, col));
    if (b_expanded) {
        p.setPen(Qt::DotLine);
        p.drawRoundRect(_x, _y, width(), topStrip, 1500 / width(), 1500 / topStrip);
        p.drawRoundRect(_x, _y + height() - botStrip, width(), botStrip, 1500 / width(), 1500 / botStrip);
    } else {
        p.setPen(QPen((isSelected() ? m_selectedCol : Qt::black), 1, Qt::SolidLine));
        p.drawRoundRect(_x, _y, width(), topStrip, 1500 / width(), 1500 / topStrip);
    }

    p.setPen(Qt::black);
    p.setFont(font());
    p.drawText(QRect(22 + _x + 8, _y, width() - 8, topStrip), Qt::AlignLeft | Qt::AlignVCenter, m_caption);

    if (b_expanded) {
        p.setPen(Qt::SolidLine);
        p.setBrush(Qt::NoBrush);
        p.drawRoundRect(_x, _y, width(), height(), 1500 / width(), 1500 / height());
    }
}

void FlowContainer::childAdded(Item *child)
{
    if (!child)
        return;

    FlowPart::childAdded(child);

    connect(this, SIGNAL(movedBy(double, double)), child, SLOT(moveBy(double, double)));
    child->setZ(ICNDocument::Z::Item + child->level());

    updateContainedVisibility();
}

void FlowContainer::childRemoved(Item *child)
{
    FlowPart::childRemoved(child);

    if (!b_expanded)
        child->setVisible(true);

    disconnect(this, SIGNAL(movedBy(double, double)), child, SLOT(moveBy(double, double)));
}

void FlowContainer::updateConnectorPoints(bool add)
{
    if (b_deleted || !isVisible())
        add = false;

    if (b_pointsAdded == add)
        return;

    b_pointsAdded = add;

    Cells *cells = p_icnDocument->cells();
    if (!cells)
        return;

    int _x = (int)x() + offsetX();
    int _y = (int)y() + offsetY();
    int w = width();
    int h = b_expanded ? height() : topStrip;

    const int mult = add ? 1 : -1;

    // Top strip
    for (int y = _y; y < _y + 24; ++y) {
        for (int x = _x; x <= _x + w; x += 8) {
            if (cells->haveCellContaing(x, y)) {
                cells->cellContaining(x, y).CIpenalty += mult * ICNDocument::hs_item;
            }
        }
    }

    // Bottom strip
    for (int y = _y + h - 16; y <= _y + h; ++y) {
        for (int x = _x; x <= _x + width(); x += 8) {
            if (cells->haveCellContaing(x, y)) {
                cells->cellContaining(x, y).CIpenalty += mult * ICNDocument::hs_item;
            }
        }
    }

    // Left strip
    int x = _x;
    for (int y = _y + 24; y < _y + h - 16; y += 8) {
        if (cells->haveCellContaing(x, y)) {
            cells->cellContaining(x, y).CIpenalty += mult * ICNDocument::hs_item;
        }
    }

    // Right strip
    x = _x + width();
    for (int y = _y + 24; y < _y + h - 16; y += 8) {
        if (cells->haveCellContaing(x, y)) {
            cells->cellContaining(x, y).CIpenalty += mult * ICNDocument::hs_item;
        }
    }
}

void FlowContainer::setFullBounds(bool full)
{
    if (full || !b_expanded) {
        QRect bounds = b_expanded ? m_sizeRect : QRect(m_sizeRect.x(), m_sizeRect.y(), m_sizeRect.width(), topStrip);
        setPoints(QPolygon(bounds));
        return;
    }

    // 	qDebug() << Q_FUNC_INFO << "width="<<width()<<" height="<<height()<<endl;

    QPolygon pa(10);
    pa[0] = QPoint(0, 0);
    pa[1] = QPoint(width(), 0);
    pa[2] = QPoint(width(), height());
    pa[3] = QPoint(0, height());
    pa[4] = QPoint(0, 0);
    pa[5] = QPoint(8, topStrip);
    pa[6] = QPoint(8, height() - botStrip);
    pa[7] = QPoint(width() - 8, height() - botStrip);
    pa[8] = QPoint(width() - 8, topStrip);
    pa[9] = QPoint(8, topStrip);
    pa.translate(offsetX(), offsetY());
    setPoints(pa);
}

void FlowContainer::buttonStateChanged(const QString & /*id*/, bool state)
{
    setExpanded(state);
}

bool FlowContainer::parentIsCollapsed() const
{
    if (!p_parentItem)
        return false;

    FlowContainer *fc = dynamic_cast<FlowContainer *>((Item *)(p_parentItem));
    return !fc->isExpanded() || fc->parentIsCollapsed();
}

void FlowContainer::setSelected(bool yes)
{
    if (yes == isSelected())
        return;

    FlowPart::setSelected(yes);
    m_rectangularOverlay->showResizeHandles(yes && isVisible());
}

void FlowContainer::setExpanded(bool expanded)
{
    if (b_expanded == expanded)
        return;

    updateConnectorPoints(false);

    // Set this now, so that child items that we call know whether or not we actually are expanded
    b_expanded = expanded;

    updateContainedVisibility();
    updateAttachedPositioning();

    p_itemDocument->setModified(true);
    m_rectangularOverlay->setVisible(expanded);
    setFullBounds(false);

    bool nodesMoved = (m_ext_out != nullptr);
    if (nodesMoved)
        p_icnDocument->requestRerouteInvalidatedConnectors();

    p_icnDocument->requestStateSave();
}

void FlowContainer::postResize()
{
    // 	qDebug() << Q_FUNC_INFO << "width="<<width()<<endl;
    setFullBounds(false);
    FlowPart::postResize();
}

void FlowContainer::updateAttachedPositioning()
{
    if (b_deleted)
        return;

    int _x = int(x()) + offsetX();
    int _y = int(y()) + offsetY();
    int w = int((std::floor(float((width() + 8) / 16))) * 16);
    int h = height();

    if (m_ext_in)
        m_ext_in->move(_x + w / 2, _y - 8);

    if (m_int_in)
        m_int_in->move(_x + w / 2, _y + 8 + topStrip);

    if (b_expanded) {
        if (m_int_out)
            m_int_out->move(_x + w / 2, _y + h - 8 - botStrip);

        if (m_ext_out)
            m_ext_out->move(_x + w / 2, _y + h - 8 + botStrip);
    } else {
        // (Note: dont really care where internal nodes are if not expanded)

        if (m_ext_out)
            m_ext_out->move(_x + w / 2, _y + 8 + topStrip);
    }

    button("expandBtn")->setGuiPartSize(22, 22);
    button("expandBtn")->move(int(x()) + offsetX() + 7, int(y()) + offsetY() + 1);
}

void FlowContainer::updateContainedVisibility()
{
    if (b_deleted)
        return;

    if (m_ext_in)
        m_ext_in->setVisible(isVisible());
    if (m_int_in)
        m_int_in->setVisible(isVisible() && b_expanded);
    if (m_int_out)
        m_int_out->setVisible(isVisible() && b_expanded);
    if (m_ext_out)
        m_ext_out->setVisible(isVisible());

    const ItemList::iterator cEnd = m_children.end();
    for (ItemList::iterator it = m_children.begin(); it != cEnd; ++it) {
        if (*it)
            (*it)->setVisible(isVisible() && b_expanded);
    }

    m_rectangularOverlay->setVisible(isVisible() && b_expanded);

    NodeGroupList hidableNodeGroups;
    p_icnDocument->getTranslatable(children(true) += GuardedItem(this), nullptr, nullptr, &hidableNodeGroups);

    NodeGroupList::iterator hngEnd = hidableNodeGroups.end();
    for (NodeGroupList::iterator it = hidableNodeGroups.begin(); it != hngEnd; ++it)
        (*it)->setVisible(b_expanded);
}

void FlowContainer::setVisible(bool yes)
{
    if (b_deleted) {
        FlowPart::setVisible(false);
        return;
    }

    FlowPart::setVisible(yes);
    updateContainedVisibility();
}
