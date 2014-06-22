/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef KTLCIRCUITPLUGIN_QT_H_
#define KTLCIRCUITPLUGIN_QT_H_

#include "interfaces/idocumentplugin.h"

namespace KTechLab {

class KTLCircuitPluginQt : public KTechLab::IDocumentPlugin
{
public:
    KTLCircuitPluginQt(const KComponentData &componentData, QObject *parent, const QVariantList& args );
    virtual ~KTLCircuitPluginQt();
    virtual void unload();

};

}

#endif // KTLCIRCUITPLUGIN_QT_H_
