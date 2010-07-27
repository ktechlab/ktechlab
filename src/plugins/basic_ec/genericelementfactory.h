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

#ifndef GENERIC_ELEMENT_FACTORY
#define GENERIC_ELEMENT_FACTORY

#include "interfaces/ielementfactory.h"

#include <kdebug.h>

/**
 * Macro for declaring an element factory
 * It declares a variable having the name "m_factory" and the ELEMENT_TYPE
 * concatenated.
 * Use it inside the plugin class declaration, outside of any methods
 */
#define DECLARE_ELEMENT_FACTORY_MEMBER(ELEMENT_TYPE)  \
    GenericElementFactory<ELEMENT_TYPE> *m_factory##ELEMENT_TYPE;

/**
 * Macro for registering an element factory
 * Use it inside the constructor of the plugin
 */
#define REGISTER_ELEMENT_FACTORY(ELEMENT_TYPE ) \
    m_factory##ELEMENT_TYPE = new GenericElementFactory<ELEMENT_TYPE>( #ELEMENT_TYPE ); \
    ISimulationManager::self()->registerElementFactory(m_factory##ELEMENT_TYPE)

/**
 * Macro for undergistering an element factory
 * Use it inside the unload() method of the plugin
 */
#define UNREGISTER_ELEMENT_FACTORY(ELEMENT_TYPE ) \
    ISimulationManager::self()->unregisterElementFactory(m_factory##ELEMENT_TYPE)

    
namespace KTechLab {

    class IElement;
    
/**
 * Generic class for defining factories for any element
 * it takes as template parameter the type of the element and in its
 * constructor the ID of that element
 * This class is a convenience class, and it can be reused multiple times,
 * in order to a plugin to register more element types
 */
template <class ElementType>
class GenericElementFactory : public IElementFactory {
    public:
        GenericElementFactory(QString id) :
            m_simType("transient"),
            m_docMimeType("application/x-circuit")
        {
            m_supportedComponents.append(id);
        }

        virtual const QString &simulationType() const {
            return m_simType;
        }
        virtual const QString &supportedDocumentMimeTypeName() const {
            return m_docMimeType;
        }
        virtual const QList<QString> supportedComponentTypeIds() const {
            return m_supportedComponents;
        }
        virtual IElement * createElement(QString type){
            if( type != m_supportedComponents.first()){
                kError() << "requested type " << type << " from " <<
                    m_supportedComponents.first() << " factory!\n";
                return 0;
            }
            return new ElementType();
        }
    private:
        QList<QString> m_supportedComponents;
        QString m_simType;
        QString m_docMimeType;

};

}
#endif // GENERIC_ELEMENT_FACTORY

