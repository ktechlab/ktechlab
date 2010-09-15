#include "qdebug.h"
#include "simulator.h"

#include "bjt.h"

int main(void){
    Simulator * sim = Simulator::self();

    sim->slotSetSimulating(false);

    BJT *q1 = new BJT(true);
    Circuit *circ = new Circuit();
    
    circ->addElement(q1);
    sim->attachCircuit(circ);

    qDebug() << "simulator: " << sim << "\n";
}

