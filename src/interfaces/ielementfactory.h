/***************************************************************************
 *    IElementFactory interface.                                           *
 *       A common interface for all compoent model factories in KTechLab   *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef IELEMENTFACTORY_H
#define IELEMENTFACTORY_H

#include "ktlinterfacesexport.h"


#include <QObject>

namespace KTechLab {

    class IElement;

/**
 * \brief common interface for all the component model (element) factories
 * The main purpose of the \ref IElement factores is to register 
 * themselves on the \ref ISimulationManager and then create 
 * IElements as needed
 * 
 * One factory type can support only one simulation type and 
 * only one document type
 */
class KTLINTERFACES_EXPORT IElementFactory : public QObject {
    Q_OBJECT

public:
    // virtual desctructor
    virtual ~IElementFactory();

    /**
     * \return the type simulation where this factory can create IElements
     */
    virtual const QString &simulationType() const = 0;

    /**
     * \return the name of the document mimetype, for which the factory can 
     * create IElements
     */
    virtual const QString &supportedDocumentMimeTypeName() const = 0;

    /**
     * \return the list of all component type IDs for which this factory can
     * create components
     */
    virtual const QList<QString> supportedComponentTypeIds() const = 0;

    /**
     * create a component model, for a gein component type
     * \param type the type of the component
     * \return the model of the componen, of NULL, if the component is not 
     * supported
     */
    virtual IElement * createElement(QString type) = 0;
};

}

#endif // IELEMENTFACTORY_H

