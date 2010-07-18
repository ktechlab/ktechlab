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

#include "kdebug.h"

using namespace KTechLab;

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




