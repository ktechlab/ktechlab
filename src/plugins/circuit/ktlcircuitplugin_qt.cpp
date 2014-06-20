/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktlcircuitplugin_qt.h"

#include <QDebug>

namespace KTechLab {

KTLCircuitPluginQt::KTLCircuitPluginQt( const KComponentData & componentData,
                                        QObject* parent, const QVariantList& /* args */ )
    : KTechLab::IDocumentPlugin( componentData, parent ) /* ,
    m_componentModel( new ComponentModel() ) */
{

    // init();
}

KTLCircuitPluginQt::~KTLCircuitPluginQt()
{
    qDebug() << "KTLCircuitPluginQt::~KTLCircuitPluginQt";
}

void KTLCircuitPluginQt::unload()
{
    qWarning() << "KTLCircuitPluginQt::unload: not implemented";
}

}


