/*
    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef CIRCUITMODELTEST_H
#define CIRCUITMODELTEST_H
#include <QObject>

#include <circuit/circuitmodel.h>

class CircuitModelTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void loadFromFile();
    void initNew();

private:
    KTechLab::CircuitModel* model;
};

#endif // CIRCUITMODELTEST_H
