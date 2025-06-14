/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mechanicsitem.h"
#include "itemdocumentdata.h"
#include "mechanicsdocument.h"

#include <KLocalizedString>

#include <QBitArray>
#include <QDebug>
#include <QPainter>
#include <cmath>

#define DPR (180.0 / M_PI)

/**
@returns an angle between 0 and 2 pi
*/
double normalizeAngle(double angle)
{
    if (angle < 0)
        angle += 2 * M_PI * (std::ceil(-angle));
    return angle - 2 * M_PI * std::floor(angle / (2 * M_PI));
}

MechanicsItem::MechanicsItem(MechanicsDocument *mechanicsDocument, bool newItem, const QString &id)
    : Item(mechanicsDocument, newItem, id)
{
    p_mechanicsDocument = mechanicsDocument;
    m_selectionMode = MechanicsItem::sm_move;

    createProperty("mass", Variant::Type::Double);
    property("mass")->setCaption(i18n("Mass"));
    property("mass")->setUnit("g");
    property("mass")->setValue(10.0);
    property("mass")->setMinValue(1e-3);
    property("mass")->setMaxValue(1e12);
    property("mass")->setAdvanced(true);

    createProperty("moi", Variant::Type::Double);
    property("moi")->setCaption(i18n("Moment of Inertia"));
    property("moi")->setUnit("gm");
    property("moi")->setValue(0.01);
    property("moi")->setMinValue(1e-3);
    property("moi")->setMaxValue(1e12);
    property("moi")->setAdvanced(true);

    setZ(ItemDocument::Z::Item);
    // 	setAnimated(true);
    p_mechanicsDocument->registerItem(this);
}

MechanicsItem::~MechanicsItem()
{
}

void MechanicsItem::setSelectionMode(SelectionMode sm)
{
    if (sm == m_selectionMode)
        return;

    m_selectionMode = sm;
}

void MechanicsItem::setSelected(bool yes)
{
    if (yes == isSelected())
        return;

    if (!yes)
        // Reset the selection mode
        m_selectionMode = MechanicsItem::sm_resize;

    Item::setSelected(yes);
}

void MechanicsItem::dataChanged()
{
    Item::dataChanged();
    m_mechanicsInfo.mass = dataDouble("mass");
    m_mechanicsInfo.momentOfInertia = dataDouble("moi");
    updateMechanicsInfoCombined();
}

PositionInfo MechanicsItem::absolutePosition() const
{
    MechanicsItem *parentMechItem = dynamic_cast<MechanicsItem *>(static_cast<Item *>(p_parentItem));
    if (parentMechItem)
        return parentMechItem->absolutePosition() + m_relativePosition;

    return m_relativePosition;
}

void MechanicsItem::reparented(Item *oldItem, Item *newItem)
{
    MechanicsItem *oldMechItem = dynamic_cast<MechanicsItem *>(oldItem);
    MechanicsItem *newMechItem = dynamic_cast<MechanicsItem *>(newItem);

    if (oldMechItem) {
        m_relativePosition = oldMechItem->absolutePosition() + m_relativePosition;
        disconnect(oldMechItem, SIGNAL(moved()), this, SLOT(parentMoved()));
    }

    if (newMechItem) {
        m_relativePosition = m_relativePosition - newMechItem->absolutePosition();
        connect(newMechItem, SIGNAL(moved()), this, SLOT(parentMoved()));
    }

    updateCanvasPoints();
}

void MechanicsItem::childAdded(Item *child)
{
    MechanicsItem *mechItem = dynamic_cast<MechanicsItem *>(child);
    if (!mechItem)
        return;

    connect(mechItem, SIGNAL(updateMechanicsInfoCombined()), this, SLOT(childMoved()));
    updateMechanicsInfoCombined();
}

void MechanicsItem::childRemoved(Item *child)
{
    MechanicsItem *mechItem = dynamic_cast<MechanicsItem *>(child);
    if (!mechItem)
        return;

    disconnect(mechItem, SIGNAL(updateMechanicsInfoCombined()), this, SLOT(childMoved()));
    updateMechanicsInfoCombined();
}

void MechanicsItem::parentMoved()
{
    PositionInfo absPos = absolutePosition();
    Item::moveBy(absPos.x() - x(), absPos.y() - y());
    updateCanvasPoints();
    Q_EMIT moved();
}

void MechanicsItem::updateCanvasPoints()
{
    const QRect ipbr = m_itemPoints.boundingRect();

    double scalex = double(m_sizeRect.width()) / double(ipbr.width());
    double scaley = double(m_sizeRect.height()) / double(ipbr.height());

    PositionInfo abs = absolutePosition();

    QTransform m;
    m.rotate(abs.angle() * DPR);
    m.translate(m_sizeRect.left(), m_sizeRect.top());
    m.scale(scalex, scaley);
    m.translate(-int(ipbr.left()), -int(ipbr.top()));
    setPoints(m.map(m_itemPoints));

    // QRect tempt = m.mapRect(ipbr); // 2017.10.01 - comment out unused variable
}

void MechanicsItem::rotateBy(double dtheta)
{
    m_relativePosition.rotate(dtheta);
    updateCanvasPoints();
    updateMechanicsInfoCombined();
    Q_EMIT moved();
}

void MechanicsItem::moveBy(double dx, double dy)
{
    m_relativePosition.translate(dx, dy);
    Item::moveBy(m_relativePosition.x() - x(), m_relativePosition.y() - y());
    Q_EMIT moved();
}

void MechanicsItem::updateMechanicsInfoCombined()
{
    m_mechanicsInfoCombined = m_mechanicsInfo;

    double mass_x = 0.;
    double mass_y = 0.;

    const ItemList::const_iterator end = m_children.end();
    for (ItemList::const_iterator it = m_children.begin(); it != end; ++it) {
        MechanicsItem *child = dynamic_cast<MechanicsItem *>(static_cast<Item *>(*it));
        if (child) {
            CombinedMechanicsInfo *childInfo = child->mechanicsInfoCombined();
            const PositionInfo relativeChildPosition = child->relativePosition();

            double mass = childInfo->mass;
            // 			double angle = relativeChildPosition.angle();
            double dx = relativeChildPosition.x() /*+ cos(angle)*childInfo->m_x - sin(angle)*childInfo->m_y*/;
            double dy = relativeChildPosition.y() /*+ sin(angle)*childInfo->m_x + cos(angle)*childInfo->m_y*/;

            m_mechanicsInfoCombined.mass += mass;
            mass_x += mass * dx;
            mass_y += mass * dy;

            double length_squared = dx * dx + dy * dy;
            m_mechanicsInfoCombined.momentOfInertia += length_squared * childInfo->momentOfInertia;
        }
    }

    m_mechanicsInfoCombined.x = mass_x / m_mechanicsInfoCombined.mass;
    m_mechanicsInfoCombined.y = mass_y / m_mechanicsInfoCombined.mass;
}

ItemData MechanicsItem::itemData() const
{
    ItemData itemData = Item::itemData();
    itemData.angleDegrees = m_relativePosition.angle() * DPR;
    return itemData;
}

bool MechanicsItem::mousePressEvent(const EventInfo &eventInfo)
{
    Q_UNUSED(eventInfo);
    return false;
}

bool MechanicsItem::mouseReleaseEvent(const EventInfo &eventInfo)
{
    Q_UNUSED(eventInfo);
    return false;
}

bool MechanicsItem::mouseDoubleClickEvent(const EventInfo &eventInfo)
{
    Q_UNUSED(eventInfo);
    return false;
}

bool MechanicsItem::mouseMoveEvent(const EventInfo &eventInfo)
{
    Q_UNUSED(eventInfo);
    return false;
}

bool MechanicsItem::wheelEvent(const EventInfo &eventInfo)
{
    Q_UNUSED(eventInfo);
    return false;
}

void MechanicsItem::enterEvent(QEvent *)
{
}

void MechanicsItem::leaveEvent(QEvent *)
{
}

QRect MechanicsItem::maxInnerRectangle(const QRect &outerRect) const
{
    QRect normalizedOuterRect = outerRect.normalized();
    const double LEFT = normalizedOuterRect.left();
    const double TOP = normalizedOuterRect.top();
    const double X = normalizedOuterRect.width();
    const double Y = normalizedOuterRect.height();
    const double a = normalizeAngle(absolutePosition().angle());

    double left;
    double top;
    double width;
    double height;

    // 	if ( can change width/height ratio )
    {
        double x1 = X * std::cos(a) - Y * std::sin(a);
        double y1 = X * std::sin(a) + Y * std::cos(a);
        double x2 = X * std::cos(a);
        double y2 = X * std::sin(a);
        double x3 = -Y * std::sin(a);
        double y3 = Y * std::cos(a);

        double xbig; /* = std::max( std::abs(x2-x3), std::abs(x1) );*/
        double ybig; /* = std::max( std::abs(y2-y3), std::abs(y1) );*/
        if ((a - floor(a / 6.2832) * 6.2832) < M_PI) {
            xbig = std::abs(x3 - x2);
            ybig = std::abs(y1);
        } else {
            xbig = std::abs(x1);
            ybig = std::abs(y3 - y2);
        }

        width = X * (X / xbig);
        height = Y * (Y / ybig);

        top = -std::sin(a) * (LEFT + width * std::sin(a)) + std::cos(a) * TOP;
        left = std::cos(a) * (LEFT + width * std::sin(a)) + std::sin(a) * TOP;
    }

    return QRect(int(left), int(top), int(width), int(height));
}

void MechanicsItem::initPainter(QPainter &p)
{
    PositionInfo absPos = absolutePosition();
    p.translate(absPos.x(), absPos.y());
    p.rotate(absPos.angle() * DPR);
    p.translate(-absPos.x(), -absPos.y());
}

void MechanicsItem::deinitPainter(QPainter &p)
{
    PositionInfo absPos = absolutePosition();
    p.translate(absPos.x(), absPos.y());
    p.rotate(-absPos.angle() * DPR);
    p.translate(-absPos.x(), -absPos.y());
}

PositionInfo::PositionInfo()
{
    reset();
}

const PositionInfo PositionInfo::operator+(const PositionInfo &info)
{
    // Copy the child to a new position
    PositionInfo newInfo = info;

    // Translate the newInfo by our translation amount
    newInfo.translate(x(), y());

    // Rotate the child about us
    newInfo.rotateAboutPoint(x(), y(), angle());

    return newInfo;
}

const PositionInfo PositionInfo::operator-(const PositionInfo &info)
{
    PositionInfo newInfo = *this;

    newInfo.translate(-info.x(), -info.y());
    newInfo.rotate(-info.angle());

    return newInfo;
}

void PositionInfo::rotateAboutPoint(double x, double y, double angle)
{
    m_angle += angle;

    double newx = x + (m_x - x) * std::cos(angle) - (m_y - y) * std::sin(angle);
    double newy = y + (m_x - x) * std::sin(angle) + (m_y - y) * std::cos(angle);

    m_x = newx;
    m_y = newy;
}

void PositionInfo::reset()
{
    m_x = 0.;
    m_y = 0.;
    m_angle = 0.;
}

MechanicsInfo::MechanicsInfo()
{
    mass = 0.;
    momentOfInertia = 0.;
}
CombinedMechanicsInfo::CombinedMechanicsInfo()
    : MechanicsInfo()
{
    x = 0.;
    y = 0.;
}
CombinedMechanicsInfo::CombinedMechanicsInfo(const MechanicsInfo &info)
    : MechanicsInfo(info)
{
    x = 0.;
    y = 0.;
}

#include "moc_mechanicsitem.cpp"
