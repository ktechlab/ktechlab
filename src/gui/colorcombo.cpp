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

#include <KLocalizedString>

#include <QColorDialog>
#include <QDebug>
#include <QPainter>

bool ColorCombo::createdPalettes = false;
QColor *ColorCombo::palette[NumberOfSchemes];
int ColorCombo::paletteSize[NumberOfSchemes];

ColorCombo::ColorCombo(ColorScheme colorScheme, QWidget *parent, const char *name)
    : QComboBox(parent /*, name */)
{
    setObjectName(name);
    m_colorScheme = colorScheme;

    customColor.setRgb(255, 255, 255);
    internalColor.setRgb(255, 255, 255);

    createPalettes();

    addColors();

    connect(this, SIGNAL(activated(int)), SLOT(slotActivated(int)));
    connect(this, SIGNAL(highlighted(int)), SLOT(slotHighlighted(int)));
}

ColorCombo::~ColorCombo()
{
}

void ColorCombo::createPalettes()
{
    if (createdPalettes)
        return;
    createdPalettes = true;

    paletteSize[QtStandard] = 17;
    palette[QtStandard] = new QColor[paletteSize[QtStandard]];

    int i = 0;

    palette[QtStandard][i++] = Qt::red;
    palette[QtStandard][i++] = Qt::green;
    palette[QtStandard][i++] = Qt::blue;
    palette[QtStandard][i++] = Qt::cyan;
    palette[QtStandard][i++] = Qt::magenta;
    palette[QtStandard][i++] = Qt::yellow;
    palette[QtStandard][i++] = Qt::darkRed;
    palette[QtStandard][i++] = Qt::darkGreen;
    palette[QtStandard][i++] = Qt::darkBlue;
    palette[QtStandard][i++] = Qt::darkCyan;
    palette[QtStandard][i++] = Qt::darkMagenta;
    palette[QtStandard][i++] = Qt::darkYellow;
    palette[QtStandard][i++] = Qt::white;
    palette[QtStandard][i++] = Qt::lightGray;
    palette[QtStandard][i++] = Qt::gray;
    palette[QtStandard][i++] = Qt::darkGray;
    palette[QtStandard][i++] = Qt::black;

    paletteSize[LED] = 6;
    palette[LED] = new QColor[paletteSize[LED]];

    i = 0;
    palette[LED][i++] = "#f62a2a";
    palette[LED][i++] = "#ff7733";
    palette[LED][i++] = "#ffbb33";
    palette[LED][i++] = "#eeee22";
    palette[LED][i++] = "#4cc308";
    palette[LED][i++] = "#22aaee";
}

void ColorCombo::setColor(const QColor &col)
{
    internalColor = col;
    addColors();
}

void ColorCombo::resizeEvent(QResizeEvent *re)
{
    QComboBox::resizeEvent(re);
    addColors();
}

void ColorCombo::slotActivated(int index)
{
    if (index == 0) {
        const QColor selectedColor = QColorDialog::getColor(customColor, this);
        if (selectedColor.isValid()) {
            customColor = selectedColor;
            QPainter painter;
            QPen pen;
            QRect rect(0, 0, width(), QFontMetrics(font()).height() + 4);
            QPixmap pixmap(rect.width(), rect.height());

            if (qGray(customColor.rgb()) < 128)
                pen.setColor(Qt::white);
            else
                pen.setColor(Qt::black);

            const bool isSuccess = painter.begin(&pixmap);
            if (!isSuccess) {
                qWarning() << Q_FUNC_INFO << " painter not active";
            }
            QBrush brush(customColor);
            painter.fillRect(rect, brush);
            painter.setPen(pen);
            painter.drawText(2, QFontMetrics(painter.font()).ascent() + 2, i18n("Custom..."));
            painter.end();

            setItemIcon(0, QIcon(pixmap));
            pixmap.detach();
        }

        internalColor = customColor;
    } else
        internalColor = palette[m_colorScheme][index - 1];

    emit activated(internalColor);
}

void ColorCombo::slotHighlighted(int index)
{
    if (index == 0)
        internalColor = customColor;
    else
        internalColor = palette[m_colorScheme][index - 1];

    emit highlighted(internalColor);
}

void ColorCombo::addColors()
{
    QPainter painter;
    QPen pen;
    QRect rect(0, 0, width(), QFontMetrics(font()).height() + 4);
    QPixmap pixmap(rect.width(), rect.height());
    int i;

    clear();

    createPalettes();

    for (i = 0; i < paletteSize[m_colorScheme]; i++)
        if (palette[m_colorScheme][i] == internalColor)
            break;

    if (i == paletteSize[m_colorScheme])
        customColor = internalColor;

    if (qGray(customColor.rgb()) < 128)
        pen.setColor(Qt::white);
    else
        pen.setColor(Qt::black);

    const bool isSuccess = painter.begin(&pixmap);
    if (!isSuccess) {
        qWarning() << Q_FUNC_INFO << " painter not active";
    }
    QBrush brush(customColor);
    painter.fillRect(rect, brush);
    painter.setPen(pen);
    painter.drawText(2, QFontMetrics(painter.font()).ascent() + 2, i18n("Custom..."));
    painter.end();

    insertItem(count(), QIcon(pixmap), QString());
    pixmap.detach();

    for (i = 0; i < paletteSize[m_colorScheme]; i++) {
        painter.begin(&pixmap);
        QBrush brush(palette[m_colorScheme][i]);
        painter.fillRect(rect, brush);
        painter.end();

        insertItem(count(), QIcon(pixmap), QString());
        pixmap.detach();

        if (palette[m_colorScheme][i] == internalColor) {
            setCurrentIndex(i + 1);
        }
    }
}
