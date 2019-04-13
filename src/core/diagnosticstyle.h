/***************************************************************************
 *   Copyright (C) 2015 Zoltan Padrah                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTECHLAB_CORE_MAIN_H__
#define KTECHLAB_CORE_MAIN_H__

// #include <QMotifStyle>
#include <qproxystyle.h>

/**
 * see approach from here:
 * http://stackoverflow.com/questions/5909907/drawing-an-overlay-on-top-of-an-applications-window
 */
class DiagnosticStyle : public QProxyStyle
{
Q_OBJECT

public:
    typedef QProxyStyle BaseStyle;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
    virtual ~DiagnosticStyle() { }
};

#endif // KTECHLAB_CORE_MAIN_H__
