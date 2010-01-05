/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "colorcombo.h"
#include "diode.h"
#include "led.h"
#include "ecnode.h"
#include "libraryitem.h"
#include "matrixdisplay.h"
#include "simulator.h"

#include <kdebug.h>
#include <klocale.h>
#include <qpainter.h>
#include <qstring.h>

Item* MatrixDisplay::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
    return new MatrixDisplay((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem* MatrixDisplay::libraryItem() {
    return new LibraryItem(
               "ec/matrix_display",
               i18n("Matrix Display"),
               i18n("Outputs"),
               "matrixdisplay.png",
               LibraryItem::lit_component,
               MatrixDisplay::construct);
}

MatrixDisplay::MatrixDisplay(ICNDocument *icnDocument, bool newItem, const char *id)
        : Component(icnDocument, newItem, id ? id : "matrix_display") {
    m_name = i18n("Matrix Display");
    m_bDynamicContent = true;

    //BEGIN Reset members
    for (unsigned i = 0; i < max_md_height; i++)
        m_pRowNodes[i] = 0;

    for (unsigned i = 0; i < max_md_width; i++)
        m_pColNodes[i] = 0;

    m_lastUpdatePeriod = 0.0;
    m_r = m_g = m_b = 0.0;
    m_bRowCathode = true;
    m_numRows = m_numCols = 0;
    //END Reset members

    createProperty("0-rows", Variant::Type::Int);
    property("0-rows")->setCaption(i18n("Rows"));
    property("0-rows")->setMinValue(1);
    property("0-rows")->setMaxValue(max_md_height);
    property("0-rows")->setValue(7);

    createProperty("1-cols", Variant::Type::Int);
    property("1-cols")->setCaption(i18n("Columns"));
    property("1-cols")->setMinValue(1);
    property("1-cols")->setMaxValue(max_md_width);
    property("1-cols")->setValue(5);

    createProperty("color", Variant::Type::Color);
    property("color")->setCaption(i18n("Color"));
    property("color")->setColorScheme(ColorCombo::LED);

    createProperty("diode-configuration", Variant::Type::Select);
    property("diode-configuration")->setCaption(i18n("Configuration"));
    QStringMap allowed;
    allowed["Row Cathode"] = i18n("Row Cathode");
    allowed["Column Cathode"] = i18n("Column Cathode");
    property("diode-configuration")->setAllowed(allowed);
    property("diode-configuration")->setValue("Row Cathode");
    property("diode-configuration")->setAdvanced(true);
}

MatrixDisplay::~MatrixDisplay() {

    for (unsigned i = 0; i < m_numCols; i++) {
        for (unsigned j = 0; j < m_numRows; j++) {
//            removeElement(m_LEDs[i][j].m_pDiode, false);
		delete m_LEDs[i][j].m_pDiode;
        }
    }
}

void MatrixDisplay::dataChanged() {
    QColor color = dataColor("color");
    m_r = double(color.red())   / 0x100;
    m_g = double(color.green()) / 0x100;
    m_b = double(color.blue())  / 0x100;

    int numRows = dataInt("0-rows");
    int numCols = dataInt("1-cols");

    bool ledsChanged = (numRows != int(m_numRows)) || (numCols != int(m_numCols));

    if (ledsChanged) initPins(numRows, numCols);

    bool rowCathode = dataString("diode-configuration") == "Row Cathode";

    if ((rowCathode != m_bRowCathode) || ledsChanged) {
        m_bRowCathode = rowCathode;

        for (unsigned i = 0; i < m_numCols; i++) {
            for (unsigned j = 0; j < m_numRows; j++) {

// do we really need to refresh the diode here?
                removeElement(m_LEDs[i][j].m_pDiode, false);
                m_LEDs[i][j].m_pDiode = new Diode();

                if (rowCathode) {
                    setup2pinElement(m_LEDs[i][j].m_pDiode, m_pColNodes[i]->pin(), m_pRowNodes[j]->pin());
                } else setup2pinElement(m_LEDs[i][j].m_pDiode, m_pRowNodes[j]->pin(), m_pColNodes[i]->pin());
            }
        }
    }
}

void MatrixDisplay::initPins(unsigned numRows, unsigned numCols) {
    if ((numRows == m_numRows) && (numCols == m_numCols))
        return;

    if (numRows > max_md_height)
        numRows = max_md_height;

    if (numCols > max_md_width)
        numCols = max_md_width;

    m_lastUpdatePeriod = 0.0;

    //BEGIN Remove diodes
    // All the diodes are going to be readded from dataChanged (where this
    // function is called from), so easiest just to delete the diodes now and
    // resize.

    for (unsigned i = 0; i < m_numCols; i++) {
        for (unsigned j = 0; j < m_numRows; j++)
            removeElement(m_LEDs[i][j].m_pDiode, false);
    }

    m_LEDs.resize(numCols);

    for (unsigned i = 0; i < numCols; i++) {
        m_LEDs[i].resize(numRows);

        for (unsigned j = 0; j < numRows; j++) {
            MatrixDisplayCell *tmp = &m_LEDs[i][j];

            tmp->m_avgBrightness = 0.0;
            tmp->m_lastBrightness = 255;
            tmp->m_pDiode = 0;
        }
    }
    //END Remove diodes

    //BEGIN Create or destroy pins
    if (numCols >= m_numCols) {
        for (unsigned i = m_numCols; i < numCols; i++)
            m_pColNodes[i] = createPin(0, 0, 270, colPinID(i));
    } else {
        for (unsigned i = numCols; i < m_numCols; i++) {
            removeNode(colPinID(i));
            m_pColNodes[i] = 0;
        }
    }

    m_numCols = numCols;

    if (numRows >= m_numRows) {
        for (unsigned i = m_numRows; i < numRows; i++)
            m_pRowNodes[i] = createPin(0, 0, 0, rowPinID(i));
    } else {
        for (unsigned i = numRows; i < m_numRows; i++) {
            removeNode(rowPinID(i));
            m_pRowNodes[i] = 0;
        }
    }

    m_numRows = numRows;
    //END Create or destroy pins

    //BEGIN Position pins et al
    setSize(-int(numCols + 1)*8, -int(numRows + 1)*8, int(numCols + 1)*16, int(numRows + 1)*16, true);

    for (int i = 0; i < int(m_numCols); i++) {
        m_nodeMap[colPinID(i)].x = offsetX() + 16 + 16 * i;
        m_nodeMap[colPinID(i)].y = offsetY() + height() + 8;
    }

    for (int i = 0; i < int(m_numRows); i++) {
        m_nodeMap[rowPinID(i)].x = offsetX() - 8;
        m_nodeMap[rowPinID(i)].y = offsetY() + 16 + 16 * i;
    }

    updateAttachedPositioning();
    //END Position pins et al
}

QString MatrixDisplay::colPinID(int col) const {
    return QString("col_%1").arg(QString::number(col));
}

QString MatrixDisplay::rowPinID(int row) const {
    return QString("row_%1").arg(QString::number(row));
}

void MatrixDisplay::stepNonLogic() {
    for (unsigned i = 0; i < m_numCols; i++) {
        for (unsigned j = 0; j < m_numRows; j++)
            m_LEDs[i][j].m_avgBrightness += LED::brightness(m_LEDs[i][j].m_pDiode->current()) * LINEAR_UPDATE_PERIOD;
    }

    m_lastUpdatePeriod += LINEAR_UPDATE_PERIOD;
}

void MatrixDisplay::drawShape(QPainter &p) {
    if (isSelected())
        p.setPen(m_selectedCol);

    p.drawRect(boundingRect());
    initPainter(p);

    const int _x = int(x() + offsetX());
    const int _y = int(y() + offsetY());

    // To avoid flicker, require at least a 10 ms sample before changing
    // the brightness
    double minUpdatePeriod = 0.0099;

    for (int i = 0; i < int(m_numCols); i++) {
        for (int j = 0; j < int(m_numRows); j++) {
            if (m_lastUpdatePeriod > minUpdatePeriod)
                m_LEDs[i][j].m_lastBrightness = unsigned(m_LEDs[i][j].m_avgBrightness / m_lastUpdatePeriod);

            double _b = m_LEDs[i][j].m_lastBrightness;

            QColor brush = QColor(uint(255 - (255 - _b) * (1 - m_r)), uint(255 - (255 - _b) * (1 - m_g)), uint(255 - (255 - _b) * (1 - m_b)));

            p.setBrush(brush);
            p.setPen(Qt::NoPen);
            p.drawEllipse(_x + 10 + i*16, _y + 10 + j*16, 12, 12);
        }
    }

    if (m_lastUpdatePeriod > minUpdatePeriod) {
        m_lastUpdatePeriod = 0.0;

        for (unsigned i = 0; i < m_numCols; i++) {
            for (unsigned j = 0; j < m_numRows; j++)
                m_LEDs[i][j].m_avgBrightness = 0.0;
        }
    }

    deinitPainter(p);
}
