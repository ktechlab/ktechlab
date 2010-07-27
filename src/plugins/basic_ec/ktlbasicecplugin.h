/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTLBASICECPLUGIN_H
#define KTLBASICECPLUGIN_H

#include "interfaces/component/icomponentplugin.h"

#include <QVariantList>

namespace KTechLab
{

class KTLBasicECFactory;
class BasicElementFactory;

class KTLBasicECPlugin : public IComponentPlugin
{
    Q_OBJECT
public:
    KTLBasicECPlugin( QObject *parent, const QVariantList& args );
    virtual ~KTLBasicECPlugin();
    virtual void unload();

private:
    void init();

    KTLBasicECFactory * m_componentFactory;
    
    BasicElementFactory * m_basicElementFactory;
};

}

#endif

