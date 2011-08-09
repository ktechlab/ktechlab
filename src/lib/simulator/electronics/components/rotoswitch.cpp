/***************************************************************************
*   Copyright (C) 2005 by John Myers                                      *
*   electronerd@electronerdia.net                                         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "rotoswitch.h"

#include "canvasitemparts.h"
#include "ecnode.h"
#include "libraryitem.h"
#include "switch.h"

#include <klocale.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>
#include <cmath>
#include <cassert>

#include <kdebug.h>

//BEGIN class ECRotoSwitch
Item* ECRotoSwitch::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
    return new ECRotoSwitch((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem* ECRotoSwitch::libraryItem() {
    return new LibraryItem(
               "ec/roto_switch",
               i18n("Rotary"),
               i18n("Switches"),
               "rotary.png",
               LibraryItem::lit_component,
               ECRotoSwitch::construct);
}

ECRotoSwitch::ECRotoSwitch(ICNDocument *icnDocument, bool newItem, const char *id)
        : Component(icnDocument, newItem, id ? id : "roto_switch"),
        m_numPositions(0) {
    m_name = i18n("Rotary Switch");
    Q3PointArray pa;
    pa.makeArc(-_pinInnerRadius, -_pinInnerRadius, 2*_pinInnerRadius, 2*_pinInnerRadius , 0, 16*360);
    setItemPoints(pa);
    //setSize( -64, -64, 128, 128 );

    //half the side length of the buttons
    int buttonRadius = 10;
    addButton("go_left",
              QRect(-_pinOuterRadius / 3 - buttonRadius,
                    _pinOuterRadius - 3 * buttonRadius,
                    2 * buttonRadius,
                    2 * buttonRadius),
              "<",
              false);

    addButton("go_right",
              QRect(_pinOuterRadius / 3 - buttonRadius,
                    _pinOuterRadius - 3 * buttonRadius,
                    2 * buttonRadius,
                    2 * buttonRadius),
              ">",
              false);

    /*Variant * v = createProperty( "button_map", Variant::Type::String );
    v->setCaption( i18n("Button Map") );
    v->setAdvanced(true);
    const QString defButtonMap("SSSSSSSSSSSM");
    v->setValue(defButtonMap);
    */
    Variant * v = createProperty("num_positions", Variant::Type::Int);
    v->setCaption(i18n("Number of Positions"));
    v->setAdvanced(false);
    v->setValue(6);
    v->setMinValue(3);
    m_inNode = createPin(0, height() / 2, 270, "in");

    v = createProperty("bounce", Variant::Type::Bool);
    v->setCaption("Bounce");
    v->setAdvanced(true);
    v->setValue(false);

    v = createProperty("bounce_period", Variant::Type::Double);
    v->setCaption("Bounce Period");
    v->setAdvanced(true);
    v->setUnit("s");
    v->setValue(5e-3);

    v = createProperty("cur_position", Variant::Type::Int);
    v->setHidden(true);
    v->setValue(0);

    //v = createProperty( "left_momentary", Variant::Type::Bool );
    //v->setCaption(i18n("Left Momentary" ) );
    //v->setValue(false);
}

ECRotoSwitch::~ECRotoSwitch() {
}

void ECRotoSwitch::dataChanged() {
    bool bounce = dataBool("bounce");
    int bouncePeriod_ms = int(dataDouble("bounce_period") * 1e3);
    m_curPosition = dataInt("cur_position");
    setUpSwitches();

    if (m_positions[m_curPosition].posSwitch->state() != Switch::Closed) {
        m_positions[m_curPosition].posSwitch->setState(Switch::Closed);
    }

    for (int i = 0; i < m_numPositions; i++) {
        m_positions[i].posSwitch->setBounce(bounce, bouncePeriod_ms);
    }
}

inline int roundTo10(int a) {
    return ((a / 10) + (a % 10 < 5 ? 0 : 1))*10;
}

void ECRotoSwitch::drawShape(QPainter &p) {
    initPainter(p);

    int cx = static_cast<int>(x());
    int cy =  static_cast<int>(y());

    const int rotorRadius = 5;

    //draw the rotor
    p.drawEllipse(cx - rotorRadius,
                  cy - rotorRadius,
                  2 * rotorRadius,
                  2 * rotorRadius);

    //and its connection
    p.drawLine(cx, cy + rotorRadius, cx, cy + _pinInnerRadius);

    //draw the output positions
    double angleBetweenPositions = (4 * M_PI / 3) / (m_numPositions - 1);

    /// \internal \brief Round to the nearest multiple of 8
#define round_8(a) (((a) > 0) ? int(((a)+4)/8)*8 : int(((a)-4)/8)*8)

    for (int i = 0; i < m_numPositions ; i++) {
        double angle = (7 * M_PI / 6) - (i * angleBetweenPositions);
        int contactX = static_cast<int>(_contactRingRadius * cos(angle));
        int contactY = static_cast<int>(_contactRingRadius * sin(angle));

        p.drawEllipse(cx + contactX - _contactRadius,
                      cy - contactY - _contactRadius,
                      2 * _contactRadius,
                      2 * _contactRadius);

        int pinX, pinY;

        switch (m_positions[i].pinAngle) {
        case 180:
            pinX = _pinInnerRadius;
            pinY = round_8(contactY);
            break;

        case 90:
            pinX = round_8(contactX);
            pinY = _pinInnerRadius;
            break;

        case 0:
            pinX = -_pinInnerRadius;
            pinY = round_8(contactY);
            break;

        default:
            assert(!"Bad pin angle");
        }

        p.drawLine(cx + contactX, cy - contactY, cx + pinX, cy - pinY);
    }

#undef round_8

    //draw the connection to the selected position
    double angle = (7 * M_PI / 6) - (m_curPosition * angleBetweenPositions);

    int contactX = static_cast<int>(_contactRingRadius * cos(angle));
    int contactY = static_cast<int>(_contactRingRadius * sin(angle));

    int rotorX = static_cast<int>(rotorRadius * cos(angle));
    int rotorY = static_cast<int>(rotorRadius * sin(angle));

    p.drawLine(cx + rotorX, cy - rotorY, cx + contactX, cy - contactY);

    deinitPainter(p);
}

void ECRotoSwitch::buttonStateChanged(const QString & id, bool state) {
    SwitchPosition& curSP = m_positions[m_curPosition];
    int nextPos = m_curPosition;

    if (m_numPositions < 2) {
        return;
    }

    if (!state) { //release
        if (!curSP.isMomentary) return;

        if (m_curPosition == 0) {
            nextPos = m_curPosition + 1;
        } else if (m_curPosition == m_numPositions - 1) {
            nextPos = m_curPosition - 1;
        }

    } else { //press
        if (id == "go_left" && m_curPosition > 0) {
            nextPos = m_curPosition - 1;
        } else if (id == "go_right" && m_curPosition < m_numPositions - 1) {
            nextPos = m_curPosition + 1;
        }

    }

    if (nextPos != m_curPosition) {
        SwitchPosition& nextSP = m_positions[nextPos];

        curSP.posSwitch->setState(Switch::Open);
        nextSP.posSwitch->setState(Switch::Closed);

        m_curPosition = nextPos;

        property("cur_position")->setValue(m_curPosition);
    }
}


/*!
Set up the switches according to the button_map
*
*/
void ECRotoSwitch::setUpSwitches() {
    if(dataInt("num_positions") == m_numPositions) {
        // number of positions didn't change, so we don't have to do anything.
        return;
    }

    //this uses the _old_ value of m_numPositions!
    for(int i = 0; i < m_numPositions; i++) {
        SwitchPosition &sp = m_positions[i];
        QString pinName = QString("pin_%1").arg(i);
        removeNode(pinName);
        delete sp.posSwitch;
    }

    m_numPositions = dataInt("num_positions");

    m_positions.clear();

    m_positions.reserve(m_numPositions);
    double angleBetweenPositions = (4 * M_PI / 3) / (m_numPositions - 1);

    for (int i = 0; i < m_numPositions; i++) {
        double angle = (7 * M_PI / 6) - (i * angleBetweenPositions);
        int contactX = static_cast<int>(_contactRingRadius * cos(angle));
        int contactY = static_cast<int>(_contactRingRadius * sin(angle));

        SwitchPosition sp;

        if (angle > 3 * M_PI / 4) {
            sp.pinAngle = 0;
            contactX = -_pinOuterRadius;
        } else if (angle > M_PI / 4) {
            sp.pinAngle = 90;
            contactY = _pinOuterRadius;
        } else {
            sp.pinAngle = 180;
            contactX = _pinOuterRadius;
        }

        // kdDebug() << contactX <<", "<< contactY <<endl;
        sp.node = createPin(contactX, -contactY, sp.pinAngle, QString("pin_%1").arg(i));
        sp.posSwitch = new Switch(this, m_inNode->pin(), sp.node->pin(), Switch::Open);
        sp.isMomentary = false; //(map[i] == 'M');
        m_positions.push_back(sp);
    }

    if(m_curPosition >= m_numPositions) {
	m_curPosition = m_numPositions - 1;
	m_positions[m_curPosition].posSwitch->setState(Switch::Closed);
	property("cur_position")->setValue(m_curPosition);
    }

    updateAttachedPositioning();

    // redraw ourself
    setChanged();
}

/*!
* Set the current position to \c newPosition updating the state of the switch.
* \c m_curPosition must reference a valid position to switch away from
*
* \param newPosition the position to switch to
*/
void ECRotoSwitch::setActivePosition(int newPosition) {
    SwitchPosition& curSP = m_positions[m_curPosition];
    SwitchPosition& nextSP = m_positions[newPosition];

    curSP.posSwitch->setState(Switch::Open);
    nextSP.posSwitch->setState(Switch::Closed);

    m_curPosition = newPosition;

    property("cur_position")->setValue(m_curPosition);
}

//END class ECRotoSwitch

