/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "idocumentplugin.h"
#include "component/componentcontainer.h"

#include <KComponentData>

using namespace KTechLab;

IDocumentPlugin::IDocumentPlugin( KComponentData data, QObject *parent)
    :   KDevelop::IPlugin( data, parent )
{}

DataContainer * IDocumentPlugin::createComponentContainer( const QString &component )
{
    return new ComponentContainer( component );
}


QString IDocumentPlugin::fileNameForComponent ( const QString& component )
{
    return QString(component).replace('/','_').append(".svg");
}

#include "idocumentplugin.moc"
// vim: sw=4 sts=4 et tw=100
