/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dependentsource.h"
#include "ecnode.h"
#include "libraryitem.h"
#include "pin.h"

#include "cccs.h"
#include "ccvs.h"
#include "vccs.h"
#include "vcvs.h"

#include <KLocalizedString>
#include <QPainter>

// BEGIN class DependentSource
DependentSource::DependentSource(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, id)
{
    setSize(-16, -16, 32, 32);

    init2PinLeft();
    init2PinRight();

    m_pNNode[1]->setLength(13);
    m_pPNode[1]->setLength(13);

    createProperty("gain", Variant::Type::Double);
    property("gain")->setCaption(i18n("Gain"));
    property("gain")->setValue(1.0);

    addDisplayText("gain", QRect(-16, -32, 32, 16), "");
}

DependentSource::~DependentSource()
{
}

void DependentSource::drawOutline(QPainter &p)
{
    const int _x = (int)x() - 16;
    const int _y = (int)y() - 32;

    // Top rectangle
    p.drawRect(_x, _y + 19, width(), 11);

#if 0
	p.save();
	bool canSetCol = (p.pen().color() != Qt::color0) && (p.pen().color() != Qt::color1);
	
	// Bottom lines
	if (canSetCol)
		p.setPen( m_pNNode[1]->isSelected() ? m_selectedCol : Qt::black );
	p.drawLine( _x, _y+40, _x+8, _y+40 ); // Left inny
	
	if (canSetCol)
		p.setPen( m_pPNode[1]->isSelected() ? m_selectedCol : Qt::black );
	p.drawLine( _x+width(), _y+40, _x+24, _y+40 ); // Right inny
	
	p.restore();
#endif

    // Bottom diamond
    QPolygon pa4(4);
    pa4[0] = QPoint(_x + 6, _y + 40);
    pa4[1] = QPoint(_x + 16, _y + 32);
    pa4[2] = QPoint(_x + 26, _y + 40);
    pa4[3] = QPoint(_x + 16, _y + 48);
    p.drawPolygon(pa4);
}

void DependentSource::drawTopArrow(QPainter &p)
{
    const int _x = (int)x() - 16;
    const int _y = (int)y() - 32;

    if (p.pen().color() == m_selectedCol)
        p.setPen(Qt::black);

    if (p.brush().color() == m_brushCol)
        p.setBrush(Qt::black);

    p.drawLine(_x + 8, _y + 24, _x + 24, _y + 24);

    QPolygon pa3(3);
    pa3[0] = QPoint(_x + 24, _y + 24);
    pa3[1] = QPoint(_x + 19, _y + 21);
    pa3[2] = QPoint(_x + 19, _y + 27);
    p.drawPolygon(pa3);
}

void DependentSource::drawBottomArrow(QPainter &p)
{
    const int _x = (int)x() - 16;
    const int _y = (int)y() - 32;

    if (p.pen().color() == m_selectedCol)
        p.setPen(Qt::black);

    if (p.brush().color() == m_brushCol)
        p.setBrush(Qt::black);

    p.drawLine(_x + 11, _y + 40, _x + 21, _y + 40);

    QPolygon pa3(3);
    pa3[0] = QPoint(_x + 21, _y + 40);
    pa3[1] = QPoint(_x + 16, _y + 37);
    pa3[2] = QPoint(_x + 16, _y + 43);
    p.drawPolygon(pa3);
}
// END class DependentSource

// BEGIN class ECCCCS
Item *ECCCCS::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECCCCS((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *ECCCCS::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/cccs")), i18n("CCCS"), i18n("Sources"), "cccs.png", LibraryItem::lit_component, ECCCCS::construct);
}

ECCCCS::ECCCCS(ICNDocument *icnDocument, bool newItem, const char *id)
    : DependentSource(icnDocument, newItem, id ? id : "cccs")
{
    m_name = i18n("Current Controlled Current Source");
    m_cccs = createCCCS(m_pNNode[0], m_pPNode[0], m_pNNode[1], m_pPNode[1], 1.);
    m_pNNode[1]->pin()->setGroundType(Pin::gt_medium);
}

ECCCCS::~ECCCCS()
{
}

void ECCCCS::dataChanged()
{
    double gain = dataDouble("gain");

    QString display = QString::number(gain / getMultiplier(gain), 'g', 3) + getNumberMag(gain) + QChar(' ');
    setDisplayText("gain", display);

    m_cccs->setGain(gain);
}

void ECCCCS::drawShape(QPainter &p)
{
    initPainter(p);
    drawOutline(p);
    drawTopArrow(p);
    drawBottomArrow(p);
    deinitPainter(p);
}
// END class ECCCCS

// BEGIN class ECCCVS
Item *ECCCVS::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECCCVS((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *ECCCVS::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/ccvs")), i18n("CCVS"), i18n("Sources"), "ccvs.png", LibraryItem::lit_component, ECCCVS::construct);
}

ECCCVS::ECCCVS(ICNDocument *icnDocument, bool newItem, const char *id)
    : DependentSource(icnDocument, newItem, id ? id : "ccvs")
{
    m_name = i18n("Current Controlled Voltage Source");
    m_ccvs = createCCVS(m_pNNode[0], m_pPNode[0], m_pNNode[1], m_pPNode[1], 1.);
    m_pNNode[1]->pin()->setGroundType(Pin::gt_medium);
}

ECCCVS::~ECCCVS()
{
}

void ECCCVS::dataChanged()
{
    double gain = dataDouble("gain");

    QString display = QString::number(gain / getMultiplier(gain), 'g', 3) + getNumberMag(gain) + QChar(' ');
    setDisplayText("gain", display);

    m_ccvs->setGain(gain);
}

void ECCCVS::drawShape(QPainter &p)
{
    initPainter(p);
    drawOutline(p);
    drawTopArrow(p);
    deinitPainter(p);
}
// END class ECCCVS

// BEGIN class ECVCCS
Item *ECVCCS::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECVCCS((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *ECVCCS::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/vccs")), i18n("VCCS"), i18n("Sources"), "vccs.png", LibraryItem::lit_component, ECVCCS::construct);
}

ECVCCS::ECVCCS(ICNDocument *icnDocument, bool newItem, const char *id)
    : DependentSource(icnDocument, newItem, id ? id : "vccs")
{
    m_name = i18n("Voltage Controlled Current Source");
    m_vccs = createVCCS(m_pNNode[0], m_pPNode[0], m_pNNode[1], m_pPNode[1], 1.);
    m_pNNode[1]->pin()->setGroundType(Pin::gt_medium);
}

ECVCCS::~ECVCCS()
{
}

void ECVCCS::dataChanged()
{
    double gain = dataDouble("gain");

    QString display = QString::number(gain / getMultiplier(gain), 'g', 3) + getNumberMag(gain) + QChar(' ');
    setDisplayText("gain", display);

    m_vccs->setGain(gain);
}

void ECVCCS::drawShape(QPainter &p)
{
    initPainter(p);
    drawOutline(p);
    drawBottomArrow(p);
    deinitPainter(p);
}
// END class ECVCCS

// BEGIN class ECVCVS
Item *ECVCVS::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECVCVS((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *ECVCVS::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/vcvs")), i18n("VCVS"), i18n("Sources"), "vcvs.png", LibraryItem::lit_component, ECVCVS::construct);
}

ECVCVS::ECVCVS(ICNDocument *icnDocument, bool newItem, const char *id)
    : DependentSource(icnDocument, newItem, id ? id : "vcvs")
{
    m_name = i18n("Voltage Controlled Voltage Source");
    m_vcvs = createVCVS(m_pNNode[0], m_pPNode[0], m_pNNode[1], m_pPNode[1], 1.);
    m_pNNode[1]->pin()->setGroundType(Pin::gt_medium);
}

ECVCVS::~ECVCVS()
{
}

void ECVCVS::dataChanged()
{
    double gain = dataDouble("gain");

    QString display = QString::number(gain / getMultiplier(gain), 'g', 3) + getNumberMag(gain) + QChar(' ');
    setDisplayText("gain", display);

    m_vcvs->setGain(gain);
}

void ECVCVS::drawShape(QPainter &p)
{
    initPainter(p);
    drawOutline(p);
    deinitPainter(p);
}
// END class ECVCVS
