/***************************************************************************
 *    KTechLab Circuit Simulator in Transient Analysis                     *
 *       Simulates circuit documents in time domain                        *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuittransientsimulator.h"

#include "interfaces/icomponentdocument.h"
#include "interfaces/idocumentmodel.h"


#include <kdebug.h>

using namespace KTechLab;

CircuitTransientSimulator::CircuitTransientSimulator(IComponentDocument* doc):
    ISimulator(doc)
{
    // check for the correct document type
    if(doc->documentType() != "Circuit")
    {
        kError() << "BUG: trying to simulate a non-circuit document as a circuit!\n";
        // FIXME what to do here? save all and exit?
    }
    m_doc = doc->documentModel();
    // TODO connect the dataUpdated from the document model to the simulator
}

void CircuitTransientSimulator::start()
{
    kDebug() << "start\n";
}

void CircuitTransientSimulator::pause()
{
    kDebug() << "pause\n";
}

void CircuitTransientSimulator::tooglePause()
{
    kDebug() << "togglePause\n";

}

IElement* CircuitTransientSimulator::getModelForComponent(QVariantMap* component)
{
    kDebug() << "getModelForComponent\n";
    return NULL;
}

void CircuitTransientSimulator::componentParameterChanged(QVariantMap* component)
{
    kDebug() << "componentParameterChanged signaled\n";

}

void CircuitTransientSimulator::documentStructureChanged()
{
    kDebug() << "documentStructureChanged\n";

}




