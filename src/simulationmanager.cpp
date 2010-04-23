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
#include "interfaces/ielementfactory.h"

#include "QMap"

using namespace KTechLab;

// ----- private data for simulation manager ------
class KTechLab::SimulationManagerPrivate {

/*
there is:
- simulation type
- document type
- component type
- simulator factory
- element factory

mappings:
(simulation type, document type) -> simulator factory
(simulation type, document type, element type) -> element factory
*/
    class SimulatorMapKey {
      public:
        QString simulationType;
        QString documentType;
    };

    class ElementMapKey {
      public:
        QString simulationType;
        QString documentType;
        QString elementType;
    };

    QMap<SimulatorMapKey, ISimulatorFactory*> simulatorFactoryMap;
    QMap<ElementMapKey, IElementFactory*> elementFactoryMap;
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
    d( new SimulationManagerPrivate){
}

SimulationManager::~SimulationManager(){
    delete d;
}

ISimulator *SimulationManager::simulatorForDocument(
                        IComponentDocument *document,
                        const QString &simulationType){
    // TODO implement
    return NULL;
}

void SimulationManager::registerSimulatorFactory(ISimulatorFactory *factory){
    // TODO implement
}

void SimulationManager::unregisterSimulatorFactory(ISimulatorFactory *factory){
    // TODO implement
}

void SimulationManager::registerElementFactory(IElementFactory *factory){
    // TODO implement
}

void SimulationManager::unregisterElementFactory(IElementFactory *factory){
    // TODO implement
}

QList<QString> SimulationManager::registeredDocumentMimeTypeNames(){
    // TODO implement
  QList<QString> ret;
  return ret;
}

QList<IElementFactory*> SimulationManager::registeredFactories(
                            const QString &simulationType){
    // TODO implement
  QList<IElementFactory*> ret;
  return ret;
}

QList<QString> SimulationManager::registeredSimulationTypes(){
    // TODO implement
  QList<QString> ret;
  return ret;
}
