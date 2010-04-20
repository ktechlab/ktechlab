/***************************************************************************
 *    ISimulationManager interface.                                        *
 *       A controller for simulations in KTechLab                          *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include "isimulationmanager.h"

using namespace KTechLab;

ISimulationManager *ISimulationManager::m_self = 0;

ISimulationManager::ISimulationManager(){
    Q_ASSERT( m_self == NULL );
    m_self = this;
}

ISimulationManager::~ISimulationManager(){
}


ISimulationManager *ISimulationManager::self(){
    return m_self;
}

// #include "isimulationmanager.moc"
