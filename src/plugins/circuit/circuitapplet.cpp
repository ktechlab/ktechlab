/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitapplet.h"

#include <Plasma/DataEngine>

CircuitApplet::CircuitApplet( QObject *parent, const QVariantList &args )
    :   Plasma::Applet( parent, args )
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setBackgroundHints(DefaultBackground);
}

CircuitApplet::~CircuitApplet()
{}

void CircuitApplet::init()
{}

void CircuitApplet::dataUpdated( const QString &name, const Plasma::DataEngine::Data &data )
{

}

// vim: sw=4 sts=4 et tw=100
