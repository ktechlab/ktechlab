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

using namespace KTechLab;

class KTechLab::SimulationManagerPrivate {
};

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
                        QString *simulationType){
    // TODO implement
    return NULL;
}

ISimulator *SimulationManager::createSimulatorForDocument(
                        IComponentDocument *document,
                        QString *simulationType){
    // TODO implment
    return NULL;
}

void registerElementFactory(IElementFactory *factory){
}

QList<QString> SimulationManager::allRegisteredDocumentMimeTypeNames(){
  QList<QString> ret;
  return ret;
}
    
QList<IElementFactory> SimulationManager::factoriesForSimulationType(QString *simulationType){
  QList<IElementFactory> ret;
  return ret;
}
    
QList<IElementFactory> SimulationManager::allRegisteredFactories(){
  QList<IElementFactory> ret;
  return ret;
}

QList<QString> SimulationManager::allRegisteredSimulationTypes(){
  QList<QString> ret;
  return ret;
}
