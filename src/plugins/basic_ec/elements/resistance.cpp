/***************************************************************************
 *    Resistance model                                                     *
 *       used to simulate resistances in KTechLab                          *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "resistance.h"

#include "kdebug.h"

using namespace KTechLab;

  /*      IElement(QVariantMap * parentInModel, IElementSet * elementSet,
                     int numNodes, int numVoltageSources); */

Resistance::Resistance(const double resistance):
    IElement(NULL, NULL, 1, 1) // FIXME create proper constructor
{
    if( resistance < 0 ){
        kError() << "BUG: negative value for a resistance?!\n";
    }
    if( resistance < 1e-9 )
        m_g = 1e9;
    else
        m_g = 1.0 / resistance;
    // TODO set the number of pins?
}

Resistance::~Resistance()
{

}

void Resistance::setConductance(const double g)
{
    #if 0
    // FIXME what's going on, in the original?
    if (g == m_g)
        return;

    if (p_eSet)
        p_eSet->setCacheInvalidated();

    m_g = g - m_g;
    add_initial_dc();

    m_g = g;
    #endif
}

void Resistance::setResistance(const double r)
{
    if( r < 1e-9 )
        setConductance(1e9);
    else
        setConductance(1. / r);
}

#if 0
// methods from initial file
void Resistance::add_initial_dc() {
    if (!b_status) return;

    A_g(0, 0) += m_g;
    A_g(1, 1) += m_g;
    A_g(0, 1) -= m_g;
    A_g(1, 0) -= m_g;
}

void Resistance::updateCurrents() {
    if (!b_status) return;

    const double i = (p_cnode[0]->voltage() - p_cnode[1]->voltage()) * m_g;

    p_cnode[0]->sinkCurrent(i);
    p_cnode[1]->sourceCurrent(i);
}
#endif

double Resistance::resistance() const
{
    return 1 / m_g;
}

double Resistance::conductance() const
{
    return m_g;
}

void KTechLab::Resistance::fillMatrixCoefficients()
{
    // copied from the original
    A_g(0, 0) += m_g;
    A_g(1, 1) += m_g;
    A_g(0, 1) -= m_g;
    A_g(1, 0) -= m_g;
}
