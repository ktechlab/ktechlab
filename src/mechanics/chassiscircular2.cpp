/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "chassiscircular2.h"

#include "libraryitem.h"

#include <KLocalizedString>
#include <QPainter>
#include <QPainterPath>

#include <algorithm>
#include <cmath>

double normalizeAngle(double angle);

Item *ChassisCircular2::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ChassisCircular2((MechanicsDocument *)itemDocument, newItem, id);
}

LibraryItem *ChassisCircular2::libraryItem()
{
    return new LibraryItem(QStringList(QString("mech/chassis_circular_2")), i18n("Circular 2-Wheel Chassis"), i18n("Chassis'"), "chassis.png", LibraryItem::lit_mechanical, ChassisCircular2::construct);
}

ChassisCircular2::ChassisCircular2(MechanicsDocument *mechanicsDocument, bool newItem, const char *id)
    : MechanicsItem(mechanicsDocument, newItem, id ? id : "chassis_circular_2")
{
    m_name = i18n("Circular 2-Wheel Chassis");

    m_theta1 = 0.0;
    m_theta2 = 0.0;

    // Q3PointArray pa;    // 2018.08.14 - ported to PainterPath
    // pa.makeEllipse( -25, -25, 50, 50 );
    QPainterPath path;
    path.addEllipse(-25, -25, 50, 50);
    QPolygon pa = path.toFillPolygon().toPolygon();

    QTransform m(4, 0, 0, 4, 0, 0);
    // m.setTransformationMode( QMatrix::Areas ); // TODO find a replacement
    pa = m.map(pa);
    setItemPoints(pa);

    itemResized();
}

ChassisCircular2::~ChassisCircular2()
{
}

void ChassisCircular2::itemResized()
{
    const double w = sizeRect().width();
    const double h = sizeRect().height();

    m_wheel1Pos = QRect(int(w / 5), int(h / 6), int(w / 4), int(h / 8));
    m_wheel2Pos = QRect(int(w / 5), int(5 * h / 6 - h / 8), int(w / 4), int(h / 8));
}

void ChassisCircular2::advance(int phase)
{
    if (phase != 1)
        return;

    double speed1 = 60.;  // pixels per second
    double speed2 = 160.; // pixels per second

    m_theta1 = normalizeAngle(m_theta1 + (speed1 / 1000.) / m_wheel1Pos.width());
    m_theta2 = normalizeAngle(m_theta2 + (speed2 / 1000.) / m_wheel2Pos.width());

    const double d1 = speed1 / 1000.;
    const double d2 = speed2 / 1000.;
    const double sep = m_wheel2Pos.center().y() - m_wheel1Pos.center().y();

    double dtheta = std::atan((d2 - d1) / sep); // Change in orientation of chassis
    double moveAngle = absolutePosition().angle() + dtheta / 2;
    rotateBy(dtheta);
    moveBy(((d1 + d2) / 2.) * std::cos(moveAngle), ((d1 + d2) / 2.) * std::sin(moveAngle));
}

void ChassisCircular2::drawShape(QPainter &p)
{
    const double _x = int(sizeRect().x() + x());
    const double _y = int(sizeRect().y() + y());
    const double w = sizeRect().width();
    const double h = sizeRect().height();

    initPainter(p);
    p.setBrush(QColor(255, 246, 210));
    QRect circleRect = sizeRect();
    circleRect.moveLeft(int(circleRect.left() + x()));
    circleRect.moveTop(int(circleRect.top() + y()));
    p.drawEllipse(circleRect);

    // Draw wheels
    // TODO get this info from m_wheel1Pos and m_wheel2Pos
    const double X = _x + (w / 5);          // Wheel's left pos
    const double H = h / 8;                 // Wheel's height
    const double y1 = _y + (h / 6);         // Wheel 1 y-pos
    const double y2 = _y + (5 * h / 6) - H; // Wheel 2 y-pos

    p.setPen(Qt::NoPen);
    const double stripeWidth = 5;
    const double offset2 = 1 + int(m_theta1 * m_wheel1Pos.width()) % int(2 * stripeWidth);
    const double offset1 = 1 + int(m_theta2 * m_wheel2Pos.width()) % int(2 * stripeWidth);
    p.setBrush(QColor(255, 232, 182));
    for (double i = -1; i < std::ceil(m_wheel1Pos.width() / stripeWidth); ++i) {
        p.setClipRect(QRect(int(_x + m_wheel1Pos.x() + 2), int(_y + m_wheel1Pos.y() + 2), int(m_wheel1Pos.width() - 4), int(m_wheel1Pos.height() - 4)),
                      /* QPainter::CoordPainter */
                      Qt::ReplaceClip // TODO original Qt::UniteClip
        );
        p.drawRect(int(offset1 + X + i * stripeWidth * 2), int(y1 + 1), int(stripeWidth), int(H - 2));

        p.setClipRect(QRect(int(_x + m_wheel2Pos.x() + 2), int(_y + m_wheel2Pos.y() + 2), int(m_wheel2Pos.width() - 4), int(m_wheel2Pos.height() - 4)),
                      /* QPainter::CoordPainter */
                      Qt::ReplaceClip // TODO original Qt::UniteClip
        );
        p.drawRect(int(offset2 + X + i * stripeWidth * 2), int(y2 + 1), int(stripeWidth), int(H - 2));
    }
    p.setClipping(false);

    p.setPen(Qt::black);
    p.setBrush(Qt::NoBrush);
    p.drawRoundRect(int(X), int(y1), int(w / 4), int(H), 25, 50);
    p.drawRoundRect(int(X), int(y2), int(w / 4), int(H), 25, 50);

    deinitPainter(p);
}
