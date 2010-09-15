#include "qdebug.h"
#include "simulator.h"

#include "bjt.h"
#include "capacitance.h"

int main(void){
    Simulator * sim = Simulator::self();

    sim->slotSetSimulating(false);

    Circuit *circ = new Circuit();

    BJT *q1 = new BJT(true);
    Capacitance *c1 = new Capacitance(1e-6, 1e-9);
    
    circ->addElement(q1);
    circ->addElement(c1);

    sim->attachCircuit(circ);

    qDebug() << "simulator: " << sim << "\n";
}

