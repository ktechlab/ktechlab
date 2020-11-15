/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *   Copyright (C) 2020 by Pino Toscano                                    *
 *   pino@kde.org                                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "colorutils.h"

#include <KColorCombo>

namespace ColorUtils
{

/**
Internal implementation based on KColorCombo, with the sole purpose
of making setColor() available as slot.
*/
class ColorCombo : public KColorCombo
{
    Q_OBJECT

public:
    ColorCombo(QWidget *parent = nullptr)
        : KColorCombo(parent)
    {
    }

public Q_SLOTS:
    void setColor(const QColor &col)
    {
        KColorCombo::setColor(col);
    }
};

QColor palette_qtstandard[] = {
    Qt::red,
    Qt::green,
    Qt::blue,
    Qt::cyan,
    Qt::magenta,
    Qt::yellow,
    Qt::darkRed,
    Qt::darkGreen,
    Qt::darkBlue,
    Qt::darkCyan,
    Qt::darkMagenta,
    Qt::darkYellow,
    Qt::white,
    Qt::lightGray,
    Qt::gray,
    Qt::darkGray,
    Qt::black
};
size_t palette_qtstandard_len = sizeof(palette_qtstandard) / sizeof(QColor);
QColor palette_led[] = {
    "#f62a2a",
    "#ff7733",
    "#ffbb33",
    "#eeee22",
    "#4cc308",
    "#22aaee"
};
size_t palette_led_len = sizeof(palette_led) / sizeof(QColor);

KColorCombo *createColorCombo(ColorScheme colorScheme, QWidget *parent)
{
    QColor *palette = nullptr;
    size_t palette_len;

    switch (colorScheme) {
    case QtStandard:
        palette = palette_qtstandard;
        palette_len = palette_qtstandard_len;
        break;
    case LED:
        palette = palette_led;
        palette_len = palette_led_len;
        break;
    }

    if (!palette)
        return nullptr;

    ColorCombo *combo = new ColorCombo(parent);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    combo->setColors(QList<QColor>(palette, palette + palette_len));
#else
    QList<QColor> colorList;
    for (size_t colorNr = 0; colorNr < palette_len; ++colorNr) {
        colorList.push_back(palette[colorNr]);
    }
    combo->setColors(colorList);
#endif
    return combo;
}

}

#include "colorutils.moc"
