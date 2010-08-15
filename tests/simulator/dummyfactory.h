/*
    dummy factory and component model. used to testthe simulationmanager

*/
#ifndef DUMMYFACTORY_H_
#define DUMMYFACTORY_H_

#include "interfaces/simulator/ielement.h"
#include "interfaces/simulator/ielementfactory.h"

#include <iostream>

namespace KTechLab{

class DummyElement : public IElement {
  public:
    void dummyMethod(){
        std::cout << "dummyMethod called\n";
    }
};

class DummyElementFactory : public IElementFactory {

  QList<QString> m_componentList;
  QString m_docMimetype;
  QString m_simType;
  
  public:
    DummyElementFactory(){
      m_componentList.append("dummy-element");
      m_docMimetype = "dummy-mimetype";
      m_simType = "dummy-simulation";
    }

    /**
     * \return the type simulation where this factory can create IElements
     */
    virtual const QString simulationType() const {
      return m_simType;
    }

    /**
     * \return the name of the document mimetype, for which the factory can
     * create IElements
     */
    virtual const QString supportedDocumentMimeTypeName() const {
      return m_docMimetype;
    }

    /**
     * \return the list of all component type IDs for which this factory can
     * create components
     */
    // FIXME element vs component
    virtual const QList<QString> supportedComponentTypeIds() const {

        return m_componentList;
    }

    /**
     * create a component model, for a gein component type
     * \param type the type of the component
     * \return the model of the componen, of NULL, if the component is not
     * supported
     */
    virtual IElement * createElement(QString type){
      if( type == "dummy-element" )
        return new DummyElement();
      return NULL;
    }
};

} // namespace
#endif // DUMMYFACTORY_H_
