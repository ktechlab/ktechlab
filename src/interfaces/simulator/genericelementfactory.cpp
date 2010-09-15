/***************************************************************************
 *    Generic Element Factory                                              *
 *       used register any element in KTechLab                             *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "genericelementfactory.h"

using namespace KTechLab;

GenericElementFactory::GenericElementFactory()
    : m_simType("transient"),
      m_docMimeType("application/x-circuit")
{
    m_supportedComponents.clear();
}

IElement* GenericElementFactory::createElement(const QByteArray& type, QVariantMap parentInModel)
{
    return createOrRegisterImpl(true, type, parentInModel);
}

QList< QString > GenericElementFactory::supportedComponentTypeIds() const
{
    return m_supportedComponents;
}

QString GenericElementFactory::supportedDocumentMimeTypeName() const
{
    return m_docMimeType;
}

QString GenericElementFactory::simulationType() const
{
    return m_simType;
}

IElement* GenericElementFactory::createOrRegisterImpl(bool create, const QByteArray& type,
                                                      const QVariantMap& parentInModel)
{
    if (!create) {  /* register */
        /* error check */
        if ( m_supportedComponents.size() != 0) {
            kWarning() << "re-registering everything? why?\n";
        }
        m_supportedComponents.clear();
    }
    IElement* result = createOrRegister(create,type,parentInModel);
    if (create && !result) {
        kError() << "requested unknown element type: "
        << type << "\n";
        return NULL;
    }
    return NULL;
}
