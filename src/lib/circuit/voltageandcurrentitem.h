/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  Zoltan Padrah <zoltan.padrah@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef VOLTAGEANDCURRENTITEM_H
#define VOLTAGEANDCURRENTITEM_H

#include <QGraphicsRectItem>

class QGraphicsSceneHoverEvent;
class QGraphicsSimpleTextItem;

namespace KTechLab {

class IDocumentScene;


/**
 * Class to display a rectangle with the height being the voltage and the width being the current.
 * The color intensity depends on the voltage.
 */
class VoltageAndCurrentItem : public QGraphicsRectItem
{
    qreal m_voltageInV;
    qreal m_currentInA;
    QSharedPointer<QGraphicsSimpleTextItem> m_tooltip;

public:
    VoltageAndCurrentItem(const QRectF & rect, QGraphicsItem* parent = 0, IDocumentScene* scene = 0);
    virtual ~VoltageAndCurrentItem();
    
    void setVoltage(qreal voltageInV);
    void setCurrent(qreal currentInA);
    
protected:
  virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
  virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );

protected:
    void updateAppearance();
    
    static const qreal m_centerX;
    static const qreal m_centerY;
};

}

#endif // VOLTAGEANDCURRENTITEM_H
