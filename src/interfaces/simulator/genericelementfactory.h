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

#include "ielementfactory.h"

#include <kdebug.h>

/**
 * Macro that declares an @ref IElementFactory class, with a given name and a list of
 * @ref IElement classes that the factory can create
 *
 * @param CLASSNAME the name of the declared, new class
 * @param ELEMENT_DECLARATIONS a list of @ref SUPPORT_ELEMENT macros, separated
 *      with whitespaces only. The SUPPORT_ELEMENT macros take as arguments
 *      the name of the supported classes
 *
 * Example:
 * @code
 *  DECLARE_ELEMENT_FACTORY(
 *      BasicElementFactory,
 *      SUPPORT_ELEMENT(Resistance)
 *      SUPPORT_ELEMENT( No_coma_between_supported_elements )
 *      SUPPORT_ELEMENT(Capacitance)
 *      );
 * @endcode
 *
 */
#define DECLARE_ELEMENT_FACTORY(CLASSNAME, ELEMENT_DECLARATIONS) \
    DECLARE_ELEMENT_FACTORY_IN_NAMESPACE( KTechLab, CLASSNAME, ELEMENT_DECLARATIONS)

/**
 * support macro, to be used as second parameter of @ref DECLARE_ELEMENT_FACTORY or
 * @ref DECLARE_ELEMENT_FACTORY_IN_NAMESPACE . More calls to this macro should be
 * separated with whitespace.
 *
 * @param CLASSNAME the name of the class, supported by the factory
 *
 */
#define SUPPORT_ELEMENT(CLASSNAME)                      \
    if( create ){                                       \
        if( type == #CLASSNAME )                        \
            return new CLASSNAME(parentInModel);        \
    } else {                                            \
        m_supportedComponents.append( #CLASSNAME );     \
    }


/**
 * Macro that declares an @ref IElementFactory class, within a given namespace
 * and with a given name and a list of
 * @ref IElement classes that the factory can create
 *
 * @param NAMESPACE the namespace where the class should reside
 * @param CLASSNAME the name of the declared, new class
 * @param ELEMENT_DECLARATIONS a list of @ref SUPPORT_ELEMENT macros, separated
 *      with whitespaces only. The SUPPORT_ELEMENT macros take as arguments
 *      the name of the supported classes
 *
 * Example:
 * @code
 *  DECLARE_ELEMENT_FACTORY_IN_NAMESPACE(
 *      KTechLab,
 *      BasicElementFactory,
 *      SUPPORT_ELEMENT(Resistance)
 *      SUPPORT_ELEMENT( No_coma_between_supported_elements )
 *      SUPPORT_ELEMENT(Capacitance)
 *      );
 * @endcode
 *
 */
#define DECLARE_ELEMENT_FACTORY_IN_NAMESPACE(NAMESPACE, CLASSNAME, ELEMENT_DECLARATIONS) \
    namespace NAMESPACE {                                                   \
    class CLASSNAME : public KTechLab::IElementFactory {                    \
        public:                                                             \
            CLASSNAME() :                                                   \
                m_simType("transient"),                                     \
                m_docMimeType("application/x-circuit")                      \
            {                                                               \
                m_supportedComponents.clear();                              \
                createOrRegister(false, "");                                \
            }                                                               \
            virtual const QString simulationType() const {                  \
                return m_simType;                                           \
            }                                                               \
            virtual const QString supportedDocumentMimeTypeName() const {   \
                return m_docMimeType;                                       \
            }                                                               \
            virtual const QList<QString> supportedComponentTypeIds() const{ \
                return m_supportedComponents;                               \
            }                                                               \
            virtual IElement * createElement(QString type,                  \
                                        QVariantMap parentInModel){         \
                return createOrRegister(true, type, parentInModel);         \
            }                                                               \
        private:                                                            \
            IElement * createOrRegister(bool create, QString type,          \
                            QVariantMap parentInModel = QVariantMap()){     \
                if(!create){  /* register */                                \
                    /* error check */                                       \
                    if( m_supportedComponents.size() != 0){                 \
                        kWarning() << "re-registering everything? why?\n";  \
                    }                                                       \
                    m_supportedComponents.clear();                          \
                }                                                           \
                ELEMENT_DECLARATIONS                                        \
                if(create){                                                 \
                    kError() << "requested unknown element type: "          \
                            << type << "\n";                                \
                    return NULL;                                            \
                }                                                           \
                return NULL;                                                \
            }                                                               \
            QList<QString> m_supportedComponents;                           \
            QString m_simType;                                              \
            QString m_docMimeType;                                          \
        };                                                                  \
    }


#endif // GENERIC_ELEMENT_FACTORY
