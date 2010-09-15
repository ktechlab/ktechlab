#include "qdebug.h"
#include "simulator.h"

int main(void){
    Simulator * s = Simulator::self();
    qDebug() << "simulator: " << s << "\n";
}

