/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTLSWITCHESPLUGIN_H
#define KTLSWITCHESPLUGIN_H

#include <QVariantList>
#include "interfaces/component/icomponentplugin.h"

class KTLSwitchesFactory;

class KTLSwitchesPlugin : public KTechLab::IComponentPlugin
{
    Q_OBJECT
public:
    KTLSwitchesPlugin( QObject *parent, const QVariantList& args );
    virtual ~KTLSwitchesPlugin();
    virtual void unload();

private:
    void init();

    KTLSwitchesFactory * m_componentFactory;
};

#endif

