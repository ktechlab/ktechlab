/***************************************************************************
 *    KTechLab simulator plugin                                            *
 *       Factory for the simulator instance                                *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "simulatorfactory.h"
#include "circuittransientsimulator.h"

using namespace KTechLab;

// empty constructor
SimulatorFactory::SimulatorFactory() :
    m_simType("transient") ,
    m_documentType( "application/x-circuit" )
{
}


SimulatorFactory::~SimulatorFactory()
{

}

QString& SimulatorFactory::simulationType(){
    return m_simType;
}

QString& KTechLab::SimulatorFactory::supportedDocumentMimeTypeName()
{
    return m_documentType;
}

KTechLab::ISimulator* KTechLab::SimulatorFactory::create(IComponentDocument* )
{
    return new CircuitTransientSimulator();
}

