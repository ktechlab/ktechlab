/***************************************************************************
 *   Copyright (C) 2003-2006 David Saxton <david@bluehaze.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "orientationwidget.h"
#include "cnitem.h"
#include "cnitemgroup.h"
#include "component.h"
#include "flowpart.h"
#include "iteminterface.h"
#include "itemlibrary.h"
#include "node.h"

#include <QBitmap>
#include <QDebug>
#include <QGridLayout>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>

#include <cassert>

const int _size = 44;

// BEGIN class OrientationWidget
OrientationWidget::OrientationWidget(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this /*, 2, 4, 0, 4 */);
    layout->setMargin(0);
    layout->setSpacing(4);

    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 4; ++col) {
            QPushButton *btn = new QPushButton(this);
            m_toolBtn[row][col] = btn;
            layout->addWidget(btn, row, col);
            btn->setFixedSize(_size + 6, _size + 6);
            // 			btn->setFlat(true);
            btn->setIconSize(QSize(50, 50));
            btn->setCheckable(true);

            connect(btn, &QPushButton::clicked, this, &OrientationWidget::slotButtonClicked);
        }
    }
    setVisible(false);
}

OrientationWidget::~OrientationWidget()
{
}

void OrientationWidget::slotUpdate(CNItemGroup *itemGroup)
{
    if (m_pCNItem)
        m_pCNItem->disconnect(this);

    m_pCNItem = dynamic_cast<CNItem *>(itemGroup->activeItem());
    if (m_pCNItem)
        connect(m_pCNItem, &CNItem::orientationChanged, this, &OrientationWidget::updateShownOrientation);

    bool haveSameOrientation = itemGroup->haveSameOrientation();

    if (FlowPart *flowPart = dynamic_cast<FlowPart *>((CNItem *)m_pCNItem)) {
        // Do we actually need to udpate the interface?
        if (m_pFlowPart && (m_bHaveSameOrientation == haveSameOrientation))
            return;

        m_pComponent = nullptr;
        m_pFlowPart = flowPart;
        m_bHaveSameOrientation = haveSameOrientation;

        initFromFlowPart(m_pFlowPart);
    }

    else if (Component *component = dynamic_cast<Component *>((CNItem *)m_pCNItem)) {
        // Do we actually need to udpate the interface?
        if (m_pComponent && (m_bHaveSameOrientation == haveSameOrientation))
            return;

        m_pFlowPart = nullptr;
        m_pComponent = component;
        m_bHaveSameOrientation = haveSameOrientation;

        initFromComponent(m_pComponent);
    }

    else
        slotClear();
}

void OrientationWidget::initFromFlowPart(FlowPart *flowPart)
{
    if (!flowPart)
        return;

    uint valid = flowPart->allowedOrientations();

    for (uint i = 0; i < 2; ++i) {
        for (uint j = 0; j < 4; ++j) {
            uint o = j + 4 * i;
            if (valid & (1 << o)) {
                m_toolBtn[i][j]->setEnabled(true);
                QPixmap pm(50, 50);
                flowPart->orientationPixmap(o, pm);
                m_toolBtn[i][j]->setIcon(QIcon(pm));
            }
        }
    }

    updateShownOrientation();
}

void OrientationWidget::initFromComponent(Component *component)
{
    const QImage im = itemLibrary()->componentImage(component);

    QRect bound = component->boundingRect();

    // We want a nice square bounding rect
    const int dy = bound.width() - bound.height();
    if (dy > 0) {
        bound.setTop(bound.top() - (dy / 2));
        bound.setBottom(bound.bottom() + (dy / 2));
    } else if (dy < 0) {
        bound.setLeft(bound.left() + (dy / 2));
        bound.setRight(bound.right() - (dy / 2));
    }

    QPixmap tbPm;

    for (unsigned col = 0; col < 4; ++col) {
        for (unsigned row = 0; row < 2; ++row) {
            bool flipped = bool(row);
            int angle = 90 * col;

            if (col || row) {
                tbPm.convertFromImage(im.transformed(Component::transMatrix(angle, flipped, bound.width() / 2, bound.height() / 2)));
            } else {
                tbPm.convertFromImage(im);
            }

            m_toolBtn[row][col]->setIcon(QIcon(tbPm));
        }
    }

    updateShownOrientation();
    setVisible(true);
}

void OrientationWidget::slotClear()
{
    if (m_pCNItem)
        m_pCNItem->disconnect(this);

    m_pComponent = nullptr;
    m_pFlowPart = nullptr;
    m_pCNItem = nullptr;

    setVisible(false);
}

void OrientationWidget::slotButtonClicked()
{
    QPushButton *btn = const_cast<QPushButton *>(dynamic_cast<const QPushButton *>(sender()));
    assert(btn);

    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 4; ++col) {
            bool isButton = (m_toolBtn[row][col] == btn);
            m_toolBtn[row][col]->setChecked(isButton);
            if (!isButton)
                continue;

            if (m_pFlowPart)
                ItemInterface::self()->setFlowPartOrientation((4 * row) + col);
            else
                ItemInterface::self()->setComponentOrientation(90 * col, bool(row));
        }
    }
}

void OrientationWidget::updateShownOrientation()
{
    if (!m_pFlowPart && !m_pComponent)
        return;

    CNItemGroup *ig = dynamic_cast<CNItemGroup *>(ItemInterface::self()->itemGroup());
    if (!ig)
        return;

    bool haveSameOrientation = ig->haveSameOrientation();

    for (unsigned col = 0; col < 4; ++col) {
        for (unsigned row = 0; row < 2; ++row) {
            if (m_pFlowPart) {
                bool setOn = haveSameOrientation && (m_pFlowPart->orientation() == (4 * row + col));
                m_toolBtn[row][col]->setChecked(setOn);
            } else {
                bool flipped = bool(row);
                int angle = 90 * col;

                bool setOn = haveSameOrientation && (m_pComponent->angleDegrees() == angle) && (m_pComponent->flipped() == flipped);
                m_toolBtn[row][col]->setChecked(setOn);
            }
        }
    }
}
// END class OrientationWidget

#include "moc_orientationwidget.cpp"
