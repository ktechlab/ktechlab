/***************************************************************************
 *    SimulationManager implmentation                                      *
 *       A controller for simulations in KTechLab                          *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "simulationmanager.h"

#include "interfaces/icomponentdocument.h"
#include "interfaces/simulator/ielementfactory.h"
#include "interfaces/simulator/isimulatorfactory.h"

#include "kdebug.h"

#include "QMap"

using namespace KTechLab;

// ----- private data for simulation manager ------

  /*
    there are:
    - simulation type
    - document type
    - component type
    - simulator factory
    - element factory

    mappings:
        (simulation type, document type) -> simulator factory
        (simulation type, document type, element type) -> element factory
      for document instances:
        (document, simulation type) -> simulator
  */
// TODO make all these classes inner classes, preverably hidden

    /// key for simulator factory map
    class SimulatorFactoryMapKey {
      public:
        QString simulationType;
        QString documentType;

        // needed by QMap implementation
        bool operator < (const SimulatorFactoryMapKey &other) const {
          return (simulationType + documentType) <
              (other.simulationType + other.documentType);
        }
    };

    /// key for element factory map
    class ElementMapKey {
      public:
        QString simulationType;
        QString documentType;
        QString elementType;

        // for convenient logging
        QString toString() {
          return "( simulatioType = " + simulationType +
              "; documentType = " + documentType +
              "; elementType = " + elementType +
              ")";
        }

        bool operator < (const ElementMapKey &other) const {
          return (simulationType + documentType + elementType) <
              (other.simulationType + other.documentType + other.elementType);
        }
    };

   /// key for created simulator map
    class SimulatorMapKey {
      public:
        // constructor, needed for the const reference to string
        SimulatorMapKey(IComponentDocument *doc,
                            const QString& simType) :
            document( doc ),
            simulationType( simType )
        {
        }

        IComponentDocument *document;
        const QString &simulationType;

        // for QMap implementation
        bool operator < (const SimulatorMapKey &other) const {
            QString my1 = QString("%1%2").arg((unsigned long)document)
                                            .arg(simulationType);
            QString my2 = QString("%1%2").arg((unsigned long)other.document)
                                .arg(other.simulationType);
            return (my1 < my2);
        }
    };

    /**
     * \brief class for private members of SimulationManager
     */
class KTechLab::SimulationManagerPrivate {

  public:

    /// all registered simulator factories
    QMap<SimulatorFactoryMapKey, ISimulatorFactory*> simulatorFactoryMap;

    /// al registered element factories
    QMap<ElementMapKey, IElementFactory*> elementFactoryMap;

    /// all created simulators
    QMap<SimulatorMapKey, ISimulator*> simulatorMap;
};


// ---- the simulation manager class -------

// the static instance
SimulationManager *SimulationManager::m_self = 0;

void SimulationManager::initialize(){

    if( m_self )
        return;

    m_self = new SimulationManager();
}

SimulationManager::SimulationManager() :
    d( new SimulationManagerPrivate()){
}

SimulationManager::~SimulationManager(){
    delete d;
}

ISimulator *SimulationManager::simulatorForDocument(
                        IComponentDocument *document,
                        const QString &simulationType){
    //
    Q_ASSERT( document );
    // check if the simulator is created
    SimulatorMapKey key( document, simulationType);
    if( d->simulatorMap.contains(key) ){
        kDebug() << "returning existing simulator\n";
        return d->simulatorMap.value(key);
    }
    // not found, create a simulator
    SimulatorFactoryMapKey factoryKey;
    factoryKey.documentType = document->mimeType()->name();
    factoryKey.simulationType = simulationType;
    if( d->simulatorFactoryMap.contains( factoryKey ) ){
        kDebug() << "factory found, creating new simulator\n";
        return d->simulatorFactoryMap.value(factoryKey)->create( document );
    }
    // no factory found
    kWarning() << "no factory found for docoument type " <<
            factoryKey.documentType << " and simulation type: " <<
            simulationType << "\n";
    return NULL;
}

bool SimulationManager::destroySimulatorForDocument(IComponentDocument* document, const QString& simulationType) {
    Q_ASSERT( document );
    //
    SimulatorMapKey key( document, simulationType);
    if(d->simulatorMap.remove(key) > 0) {
        qDebug() << "removed at least 1 simulator";
        return true;
    }
    qDebug() << "cannot find simulator";
    return false;
}

void SimulationManager::registerSimulatorFactory(ISimulatorFactory *factory){
    //
    Q_ASSERT( factory );
    //
	kDebug() << "Registering factory: " << factory->simulationType();
	//
    SimulatorFactoryMapKey key;
    key.simulationType = factory->simulationType();
    key.documentType = factory->supportedDocumentMimeTypeName();
    // check for duplicates
    if( d->simulatorFactoryMap.contains( key ) ){
      kWarning() << "overwriting already registered factory for document type: " <<
          key.documentType << " and simulation type " << key.simulationType << "\n";
      // clean it up, just to be sure
      d->simulatorFactoryMap.remove(key);
    }
    // insert anyway
    kDebug() << "registering factory for document type " <<
        key.documentType << " and simulation type " << key.simulationType
        << "\n";
    d->simulatorFactoryMap.insert( key,factory);
}

void SimulationManager::unregisterSimulatorFactory(ISimulatorFactory *factory){
    //
    Q_ASSERT( factory );
    //
	kDebug() << "Unregistering simulator factory: " << factory->simulationType();
	//
    SimulatorFactoryMapKey key;
    key.simulationType = factory->simulationType();
    key.documentType = factory->supportedDocumentMimeTypeName();
    // check for duplicates
    if( d->simulatorFactoryMap.contains( key ) ){
        ISimulatorFactory * original = d->simulatorFactoryMap.value( key );
        if( original != factory ){
          kWarning() << "for the given document type and simulation type, " <<
              "not this factory instance is registered. \n";
        }
        d->simulatorFactoryMap.remove(key);
        kDebug() << "unregistered factory.\n";
    } else
        kError() << " factory not registered \n";
}

void SimulationManager::registerElementFactory(IElementFactory *factory){
    //
    Q_ASSERT( factory );
    //
    ElementMapKey key;  // this key will be reused
    key.simulationType = factory->simulationType();
    key.documentType = factory->supportedDocumentMimeTypeName();
    // iterate through the component IDs
    const QList<QString> list = factory->supportedComponentTypeIds();
    for (int i = 0; i < list.size(); ++i) {
      key.elementType = list.at(i);
      kDebug() << "registering IElement of type: " << key.toString() << "\n";
      d->elementFactoryMap.insert( key, factory );
    }
}

void SimulationManager::unregisterElementFactory(IElementFactory *factory){
    //
    Q_ASSERT( factory );
    //
    ElementMapKey key;  // this key will be reused
    key.simulationType = factory->simulationType();
    key.documentType = factory->supportedDocumentMimeTypeName();
    // iterate through the component IDs
    const QList<QString> list = factory->supportedComponentTypeIds();
    for (int i = 0; i < list.size(); ++i) {
        key.elementType = list.at(i);
        kDebug() << "unregistering IElement of type: " << key.toString() << "\n";
        if( d->elementFactoryMap.value(key) != factory )
            kWarning() << "for this element, no this factory is registered.\n";
        d->elementFactoryMap.remove( key );
    }
}

QList<QString> SimulationManager::registeredDocumentMimeTypeNames(){
    //
    QList<SimulatorFactoryMapKey> docKeyList =
                                        d->simulatorFactoryMap.uniqueKeys();
    QList<QString> ret;
    for(int i=0; i<docKeyList.size(); ++i){
        QString type = docKeyList.at(i).documentType ;
        if( ! ret.contains( type ) )
            ret.append( type );
    }
    return ret;
}

QList<IElementFactory*> SimulationManager::registeredFactories(
                            const QString &simulationType,
                            const QString &elementType,
                            const QString &documentType){

    // if all 3 values are defined
    if( (!simulationType.isEmpty()) &&
        (!elementType.isEmpty()) &&
        (!documentType.isEmpty()) )
    {
        ElementMapKey key;
        key.simulationType = simulationType;
        key.elementType = elementType;
        key.documentType = documentType;
        return d->elementFactoryMap.values(key);
    }
    // if max 2, then iterate all over
    QList<IElementFactory*> keyList =
                                d->elementFactoryMap.values();
    QList<IElementFactory*> ret;
    for(int i=0; i<keyList.size(); ++i){
        IElementFactory* fact = keyList.at(i);
        if( simulationType.isEmpty() ){
            // all simulation types
            if( ! ret.contains( fact) )
                ret.append( fact);
        } else
            if( fact->simulationType() == simulationType ){
                if( elementType.isEmpty() ){
                    // only the selected ones, any element
                    if( ! ret.contains( fact) )
                        ret.append( fact);
                } else {
                    // have to select the if it can create a specific element
                    if( fact->supportedComponentTypeIds().contains(elementType) )
                        if( ! ret.contains( fact ) )
                            ret.append( fact );
                }
            }

    }
    return ret;
}

QList<QString> SimulationManager::registeredSimulationTypes(){
    //
    QList<SimulatorFactoryMapKey> docKeyList =
                                      d->simulatorFactoryMap.uniqueKeys();
    QList<QString> ret;
    for(int i=0; i<docKeyList.size(); ++i){
        QString type = docKeyList.at(i).simulationType ;
        if( ! ret.contains( type ) )
            ret.append( type );
    }
    return ret;
}
