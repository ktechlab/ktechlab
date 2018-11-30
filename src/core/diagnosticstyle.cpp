/***************************************************************************
 *   Copyright (C) 2015 Zoltan Padrah                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "diagnosticstyle.h"

#include <qpainter.h>
#include <qwidget.h>

void DiagnosticStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    BaseStyle::drawControl(element, option, painter, widget);
    if (widget && painter) {
        // draw a border around the widget
        painter->setPen(QColor("red"));
        painter->drawRect(widget->rect());

        // show the classname of the widget
        QBrush translucentBrush(QColor(255,246,240, 100));
        painter->fillRect(widget->rect(), translucentBrush);
        painter->setPen(QColor("darkblue"));
//         QFont textFont = painter->font();
//         textFont.setPointSize( 8 );
//         painter->setFont(textFont);
        QString text(widget->metaObject()->className());
        text.append(":");
        text.append(widget->objectName());
        painter->drawText(widget->rect(),
                          Qt::AlignLeft | Qt::AlignTop,
                          //Qt::AlignRight | Qt::AlignBottom,
                          text);
    }
}

#include "diagnosticstyle.moc"
