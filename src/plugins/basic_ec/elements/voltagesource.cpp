/***************************************************************************
 *    Voltage source model                                                     *
 *       used to simulate voltage sources in KTechLab                          *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "voltagesource.h"

#include <QStringList>

using namespace KTechLab;

VoltageSource::VoltageSource(QVariantMap parent):
    IElement(parent, 2, 2, 1,
                QString("p1,n1").split(",") )
{
    m_v = 5;
}

VoltageSource::~VoltageSource()
{

}

void VoltageSource::actOnSimulationStep(double currentTime)
{

}

void VoltageSource::fillMatrixCoefficients()
{
    A_b(0, 0) = -1;
    A_c(0, 0) = -1;
    A_b(1, 0) = 1;
    A_c(0, 1) = 1;

    b_v(0) = m_v;
}


#if 0
VoltageSource::VoltageSource(const double voltage)
		: Element::Element() {
	m_v = voltage;
	m_numCBranches = 1;
	m_numCNodes = 2;
}

VoltageSource::~VoltageSource() {
}
#endif

void VoltageSource::setVoltage(const double v) {
    #if 0
	if (m_v == v) return;

	if (p_eSet)
		p_eSet->setCacheInvalidated();

	m_v = v;

	add_initial_dc();
    #endif
}

/* FIXME:
	Often times, you want to use one of these to set a voltage in a circuit
	but not pass
current. This creates a singular matrix.
Depending on how Matrix is set up, it might either cause the
circuit to glitch out or will create a
virtual one ohm resistor either into a component that
shouldn't be conducting and draw/source that current from wherever it is avaliable. =P

VoltageSignal also exhibits this problem.
*/
void VoltageSource::add_initial_dc() {
    #if 0
	if (!b_status)
		return;

	A_b(0, 0) = -1;
	A_c(0, 0) = -1;
	A_b(1, 0) = 1;
	A_c(0, 1) = 1;

	b_v(0) = m_v;
    #endif
}

void VoltageSource::updateCurrents() {
    #if 0
	if (!b_status) return;

	double i = p_cbranch[0]->current();

	p_cnode[0]->sourceCurrent(i);
	p_cnode[1]->sinkCurrent(i);
    #endif
}

