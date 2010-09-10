/*
    KTechLab, and IDE for electronics
    Copyright (C) 2010 Zoltan Padrah

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef KTECHLAB_DIRECTSIMULATORTEST_H
#define KTECHLAB_DIRECTSIMULATORTEST_H

#include <QObject>

namespace KTechLab {

class CircuitTransientSimulator;

class ISimulator;

class IDocumentModel;

class DirectSimulatorTest : public QObject
{
    Q_OBJECT

    private slots:
        void initTestCase();
        void cleanupTestCase();

        void init();

        void addResistor();
        void addCapacitor();

    private:
        IDocumentModel *model;
        ISimulator *simulator;
        CircuitTransientSimulator *transSim;

    friend class CircuitTransientSimulator;
};

}

#endif // KTECHLAB_DIRECTSIMULATORTEST_H
