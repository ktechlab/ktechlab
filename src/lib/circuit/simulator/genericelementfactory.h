/***************************************************************************
 *    Generic Element Factory                                              *
 *       used register any element in KTechLab                             *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                  2010 Julian Bäume                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef GENERIC_ELEMENT_FACTORY
#define GENERIC_ELEMENT_FACTORY

#include "interfaces/simulator/ielementfactory.h"
#include "../circuitexport.h"

//#if KDE_ENABKED
//#include <kdebug.h>
//#endif
//
//#include <QDebug>

/**
 * support macro, to be used in the implementation of the pure virtual function
 * in KTechLab::GenericElementFactory.
 *
 * @param CLASSNAME the name of the class, supported by the factory
 * @param TYPENAME a string containing the type of this element
 *
 */
#define SUPPORT_ELEMENT(CLASSNAME, TYPENAME)          \
    if( create ){                                     \
        if( type == TYPENAME )                        \
            return new CLASSNAME(parentInModel);      \
    } else {                                          \
        m_supportedComponents.append( TYPENAME );     \
    }

namespace KTechLab
{
/**
 * Default implementation for a KTechLab::IElementFactory that can be used to
 * provide elements used for simulation of circuits. You can just sub-class this
 * and override the pure virtual function. This header also provides a macro
 * helping you with implementing.
 */
class CIRCUIT_EXPORT GenericElementFactory : public KTechLab::IElementFactory
{
public:
    GenericElementFactory();
    virtual QString simulationType() const;
    virtual QString supportedDocumentMimeTypeName() const;
    virtual QList<QString> supportedComponentTypeIds() const;
    virtual IElement * createElement(const QByteArray& type, QVariantMap parentInModel);
protected:
    /**
     * Override this method using the SUPPORT_ELEMENT() macro. Make sore to
     * always return something or else the code will not compile. You can implement
     * this method like this:
     *
     * \code
        virtual IElement * createOrRegister(bool create, const QByteArray& type,
                                            QVariantMap parentInModel = QVariantMap())
        {
            SUPPORT_ELEMENT(Resistor,"Resistor")
            SUPPORT_ELEMENT(Capacitor,"Capacitor")
            return 0;
        }
     * \endcode
     */
    virtual IElement * createOrRegister(bool create, const QByteArray& type,
                                        QVariantMap parentInModel = QVariantMap()) =0;

    QList<QString> m_supportedComponents;
private:
    IElement * createOrRegisterImpl(bool create, const QByteArray& type,
                                    const QVariantMap& parentInModel = QVariantMap());
    QString m_simType;
    QString m_docMimeType;
};
}

#endif // GENERIC_ELEMENT_FACTORY
