/***************************************************************************
 *   Copyright (C) 2006 by William Hillerby                                *
 *   william.hillerby@ntlworld.com                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "variablecapacitor.h"
#include "canvasitemparts.h"
#include "capacitance.h"
#include "ecnode.h"
#include "libraryitem.h"

#include <KLocalizedString>

#include <QDebug>
#include <QPainter>
#include <QStyle>

Item *VariableCapacitor::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new VariableCapacitor(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *VariableCapacitor::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/variablecapacitor")), i18n("Variable Capacitor"), i18n("Passive"), "variable_capacitor.png", LibraryItem::lit_component, VariableCapacitor::construct);
}

VariableCapacitor::VariableCapacitor(ICNDocument *icnDocument, bool newItem, const QString &id)
    : Component(icnDocument, newItem, (!id.isEmpty()) ? id : "variable capacitor")
{
    m_name = i18n("Variable Capacitor");

    // Top Left(x,y) from centre point, width, height.
    setSize(-16, -8, 32, 16);

    // william - you might want to change this value. I added this line as it
    // was being used unitialized (in the sliderValueChanged function when
    // addSlider is called later on), and causing a crash - david.
    m_tickValue = 1;

    m_maxCapacitance = 0.0001;
    m_minCapacitance = 0.00005;

    m_currCapacitance = m_minCapacitance + ((m_maxCapacitance - m_minCapacitance) / 2);

    init1PinLeft();
    init1PinRight();

    m_pNNode[0]->setLength(15);
    m_pPNode[0]->setLength(15);

    m_pCapacitance = createCapacitance(m_pNNode[0], m_pPNode[0], m_currCapacitance);

    addDisplayText("capacitance", QRect(-8, -26, 16, 16), "", false);

    createProperty("currcapacitance", Variant::Type::Double);
    property("currcapacitance")->setCaption(i18n("Capacitance"));
    property("currcapacitance")->setUnit("F");
    property("currcapacitance")->setMinValue(1e-12);
    property("currcapacitance")->setMaxValue(1e12);
    property("currcapacitance")->setValue(m_currCapacitance);

    createProperty("maximum capacitance", Variant::Type::Double);
    property("maximum capacitance")->setCaption(i18n("Max"));
    property("maximum capacitance")->setUnit("F");
    property("maximum capacitance")->setMinValue(1e-12);
    property("maximum capacitance")->setMaxValue(1e12);
    property("maximum capacitance")->setValue(m_maxCapacitance);

    createProperty("minimum capacitance", Variant::Type::Double);
    property("minimum capacitance")->setCaption(i18n("Min"));
    property("minimum capacitance")->setUnit("F");
    property("minimum capacitance")->setMinValue(1e-12);
    property("minimum capacitance")->setMaxValue(1e12);
    property("minimum capacitance")->setValue(m_minCapacitance);

    Slider *s = addSlider("slider", 0, 100, 1, 50, Qt::Horizontal, QRect(-16, 10, 32, 16));
    m_pSlider = qobject_cast<QSlider *>(s->widget());
}

VariableCapacitor::~VariableCapacitor()
{
}

void VariableCapacitor::dataChanged()
{
    double new_minCapacitance = dataDouble("minimum capacitance");
    double new_maxCapacitance = dataDouble("maximum capacitance");

    if (new_minCapacitance != m_minCapacitance) {
        if (new_minCapacitance >= m_maxCapacitance) {
            m_minCapacitance = m_maxCapacitance;
            property("minimum capacitance")->setValue(m_minCapacitance);
        } else
            m_minCapacitance = new_minCapacitance;
    }

    if (new_maxCapacitance != m_maxCapacitance) {
        if (new_maxCapacitance <= m_minCapacitance) {
            m_maxCapacitance = m_minCapacitance;
            property("maximum capacitance")->setValue(m_maxCapacitance);
        } else
            m_maxCapacitance = new_maxCapacitance;
    }

    /*  Attempt at  fixme.
        m_currCapacitance = property( "currcapacitance" )->value().asDouble();

        if(m_currCapacitance > m_maxCapacitance) m_currCapacitance = m_maxCapacitance;
        else if(m_currCapacitance < m_minCapacitance) m_currCapacitance = m_minCapacitance;
    */

    m_tickValue = (m_maxCapacitance - m_minCapacitance) / m_pSlider->maximum();

    property("currcapacitance")->setValue(m_currCapacitance);

    // Calculate the capacitance jump per tick of a 100 tick slider.
    sliderValueChanged("slider", slider("slider")->value());
}

void VariableCapacitor::sliderValueChanged(const QString &id, int newValue)
{
    if (id != "slider")
        return;

    /** @todo fix slider so current cap can be set in toolbar and editor and slider updates */
    m_currCapacitance = m_minCapacitance + (newValue * m_tickValue);

    // Set the new capacitance value.
    m_pCapacitance->setCapacitance(m_currCapacitance);

    // Update property.
    property("currcapacitance")->setValue(m_currCapacitance);

    QString display = QString::number(m_currCapacitance / getMultiplier(m_currCapacitance), 'g', 3) + getNumberMag(m_currCapacitance) + "F";

    setDisplayText("capacitance", display);
}

void VariableCapacitor::drawShape(QPainter &p)
{
    initPainter(p);

    // Get centre point of component.
    int _y = int(y());
    int _x = int(x());

    p.drawRect(_x - 8, _y - 8, 5, 16);
    p.drawRect(_x + 3, _y - 8, 5, 16);

    // 	p.drawLine( _x-8, _y, _x-16, _y );
    // 	p.drawLine( _x+8, _y, _x+16, _y );

    // Diagonally pointing arrow
    QPolygon pa(3);
    pa[0] = QPoint(-4, 0);
    pa[1] = QPoint(-2, 4);
    pa[2] = QPoint(0, 0);

    pa.translate(_x + 16, _y - 8);
    p.setBrush(p.pen().color());
    p.drawPolygon(pa);

    p.drawLine(_x - 16, _y + 8, _x + 16, _y - 8);

    deinitPainter(p);
}
