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


#include "voltageandcurrentitem.h"
#include <interfaces/idocumentscene.h>

#include <cmath>

using namespace KTechLab;

const qreal VoltageAndCurrentItem::m_centerX = 0;
const qreal VoltageAndCurrentItem::m_centerY = -10;

VoltageAndCurrentItem::VoltageAndCurrentItem(const QRectF& rect, QGraphicsItem* parent, IDocumentScene* scene) : 
    QGraphicsRectItem(parent, scene),
    m_voltageInV(0), m_currentInA(0)
{
    Q_UNUSED(rect);
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, false);
    setFlag(ItemIgnoresParentOpacity);
    setOpacity(0.95);
    updateAppearance();
}

VoltageAndCurrentItem::~VoltageAndCurrentItem()
{

}

void VoltageAndCurrentItem::setVoltage(qreal voltageInV)
{
    m_voltageInV = voltageInV;
    updateAppearance();
}

void VoltageAndCurrentItem::setCurrent(qreal currentInA)
{
    m_currentInA = currentInA;
    updateAppearance();
}




/// The maximum length of the voltage indiactor
const int vLength = 8;

/// The current at the middle of the current indicator
const qreal iMidPoint = 0.03;

/// The maximum thicnkess of the current indicator
const int iLength = 6;

inline qreal calcIProp(const qreal i) {
	return 1 - iMidPoint / (iMidPoint + std::abs(i));
}

inline int calcThickness(const qreal prop) {
	return (int)((iLength -2)*prop + 2);
}

qreal voltageLength(qreal v) {
	qreal v_max = 1e1;
	qreal v_min = 1e-1;

	v = std::abs(v);

	if(v >= v_max) return 1.0;
	else if(v <= v_min) return 0.0;
	else return std::log(v / v_min) / std::log(v_max / v_min);
}

inline int calcLength(qreal v) {
	qreal prop = voltageLength(v);

	if (v > 0)
		prop *= -1.0;

	return int(vLength * prop);
}



QColor voltageColor(qreal v) {
	qreal prop = voltageLength(v);

	if(v >= 0)
		return QColor(int(255 * prop), int(166 * prop), 0);
	else return QColor(0, int(136 * prop), int(255 * prop));
}

void VoltageAndCurrentItem::updateAppearance()
{
  QColor color = voltageColor(m_voltageInV);
  qreal iProp = calcIProp(m_currentInA);
  int thickness = calcThickness(iProp);
  int length = calcLength(m_voltageInV);
  
  setPen(QPen(color, thickness));
  
  qreal startX = m_centerX - length / 2;
  qreal y = m_centerY - thickness / 2;
  qreal width = length;
  qreal height = thickness;
  setRect(startX, y, width, height);
  
  setToolTip(QString("V = %1V; I = %2A").arg(m_voltageInV).arg(m_currentInA));
}
