/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitdocument.h"

#include "shell/core.h"

#include <KDebug>
#include <KLocale>
#include <Plasma/View>
#include <Plasma/Containment>

CircuitDocument::CircuitDocument( const KUrl &url, KTechLab::Core* core )
    :   KTechLab::PartDocument( url, core )
{

}

CircuitDocument::~CircuitDocument()
{
}

QString CircuitDocument::documentType() const
{
    return "Circuit";
}

QWidget* CircuitDocument::createViewWidget( QWidget* parent )
{
    Plasma::View *view = new Plasma::View( new Plasma::Containment() );
    kDebug() << "Created view: " << view << endl;
    return view;
}

#include "circuitdocument.moc"
