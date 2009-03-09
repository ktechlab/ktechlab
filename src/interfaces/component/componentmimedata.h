/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef COMPONENTMIMEDATA_H
#define COMPONENTMIMEDATA_H

#include "../ktlinterfacesexport.h"

#include <QMimeData>

namespace KTechLab
{
class IComponentFactory;
class IComponent;

/**
 * This class carries an instance of IComponentFactory during Drag & Drop actions to provide new
 * components.
 */
class KTLINTERFACES_EXPORT ComponentMimeData: public QMimeData
{
Q_OBJECT
public:
    ComponentMimeData( const QString &name, KTechLab::IComponentFactory *factory );

    /**
     * Let the ComponentFactory create a new component and return it.
     */
    KTechLab::IComponent * createComponent();

private:
    KTechLab::IComponentFactory *m_componentFactory;
    QString m_componentName;
};

} // namespace KTechLab

#endif

