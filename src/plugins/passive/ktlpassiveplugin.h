/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTLPASSIVEPLUGIN_H
#define KTLPASSIVEPLUGIN_H

#include <QVariantList>
#include "interfaces/component/icomponentplugin.h"

namespace KTechLab
{

class KTLPassiveFactory;

class KTLPassivePlugin : public IComponentPlugin
{
    Q_OBJECT
public:
    KTLPassivePlugin( QObject *parent, const QVariantList& args );
    virtual ~KTLPassivePlugin();
    virtual void unload();

private:
    void init();

    KTLPassiveFactory * m_componentFactory;
};

}

#endif

