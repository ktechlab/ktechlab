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
                        IComponentDocument *document){
    // TODO implement
    return NULL;
}

ISimulator *SimulationManager::createSimulatorForDocument(
                        IComponentDocument *document){
    // TODO implment
    return NULL;
}
