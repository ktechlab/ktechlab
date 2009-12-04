/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DOCUMENTPLUGIN_H
#define DOCUMENTPLUGIN_H

#include "ktlinterfacesexport.h"
#include "datacontainer.h"

#include <interfaces/iplugin.h>
#include <QObject>
#include <QString>

namespace KDevelop
{
class IDocument;
} // namespace KDevelop

namespace KTechLab
{

class IComponentFactory;

class KTLINTERFACES_EXPORT IDocumentPlugin: public KDevelop::IPlugin
{
    Q_OBJECT
public:
    IDocumentPlugin( KComponentData data, QObject *parent = 0 );
    virtual ~IDocumentPlugin() {};

    /**
     * create a DataContainer for a given document
     */
    virtual DataContainer * createDataContainer( KDevelop::IDocument *document, const QString &component = QString() )=0;

    /**
     * create a DataContainer for a given component
     */
    DataContainer * createComponentContainer( const QString &component );

    virtual void registerComponentFactory( KTechLab::IComponentFactory * factory )=0;
};

} // namespace KTechLab

#endif

