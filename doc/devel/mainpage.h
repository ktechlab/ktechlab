/**

\mainpage

\section intro Introduction

This page in the documentation will describe the general architecture of KTechLab,
from developer point of view.

\section terms Terminology

Various terms used in this document are described here.

\li Component: basic block of a circuit
\li Element: a model corresponding to a Component, used in simulation
\li Pin: a point where connections can be made to a component
\li Connector: a link between two pins
\li Wire: model of a connector, used in simulation
\li Circuit: a set of components and connectors
\li Simulator: class that simulates the behaviour of a circuit

--
\li CNode: component node, used in simulation, might stand for many Pins;
    CNode:Pin -> 1:many

\li Circuit Model: the storage place for the description of the structure and status of a circuut

\li *Item: graphical classes, inherited from QGraphicsItem

\section src_org Organization of the source code

KTechLab internally is based on KDevPlatform.
See http://www.kdevelop.org/mediawiki/index.php/KDevelop_4#Platform_Information .

The source code has similar layout as the above mentioned platform.
It uses plugins.

There are three big parts:
\li interfaces: used by the main program and can used by any plugin
\li main program: it has the "main" method
\li plugins: can use only the interfaces, and they are loaded/unloaded dynamically during runtime

The plugins are of two types:
\li project plugins: these are loaded at program startup, and unladed at exit
\li regular plugins: can be loaded/unloaded any time

Currently the following plugins exist:
\li ktlproject: KTechLab project plugin. The only project plugin.
\li basic_ec: set of basic electronic components
\li circuit: support for the Circuit document type
\li flowcode: support for the FlowCode document type -- currently not functional
\li mcu_pic: support for PICs -- currently not functional
\li probevis: probe visualisation -- currently not functional
\li simulator support for simulating circuits

The following subsections describe various aspects of the program.

\subsection main Main function

TODO

\subsection sim_manager SimulationManager

TODO

\subsection electr_simulation Workings of circuit simulation

When an electronic circuit is simulated, the following processes take place:
\li user interaction with the circuit: the model is changed in order to reflect the new situation;
    event flow: user -> gui -> circuit scene -> model -> simulator update, based on the model
\li visual representation redraw: the components are drawn periodically;
    event flow: gui refresh timer ticks -> drawing performed, based only the state of the model;
    approximate timer frequency: less than 50 - 60 Hz
\li simulation time advance: time passes, so some signals (current, voltage) might change;
    event flow: simulation timer ticks -> simulator calculates new voltage and current values ->
        simulator stores the calculated values in the model

The CircuitModel, subclass of IDocumentModel, currently consists of a set of components and connectors,
both having the type of QVariantMap.

Models of components have the base type IElement. This interfaces has references to
IPins, and IPins are connected with IWires.

CNode and CBranch are used in the internal workings of the simulator, so those
are not supposed to be exported from the simulator plugin.

Relationships:
\li Component:IElement -- 1:many
\li IElement:IPin -- 1:many
\li IPin:CNode -- many:1

During one time step, in the simulation of a circuit the following steps
have to be taken:
\li if the circuit model is changed, the simulator data structures
    (IPins, IWires and IElements) have to be rebuilt.
    Next, the number of nodes is reduced, due to connections.
    Here the CNodes and CBranches are used.
    Some nodes are marked pure digital, others are analogic.
    Finally, the circuit equations are created from the IElements.
\li the simulation time is incremented
\li the circuit equations are created from the IElements
\li next, the digital logic is stepped once, and the resulting voltages
    are saved
\li the digital values are stored in the matrix and the matrix equations
    are solved for the currents and voltages
\li if the circuit contains nonlinear or reactive elements, the process of
    stepping analog components and solving of the matrix has to be repeated.
\li having the currents and the voltages from CNodes and CBranches known,
    by using Kirchoff's equations, the voltages and currents in
    IPins and IWires have to be calculated
\li the final voltages and currents are stored in the circuit model

The simulation usese the Modified Nodal Analysis. See as reference:
\li http://en.wikipedia.org/wiki/Modified_nodal_analysis
\li http://qucs.sourceforge.net/tech/technical.html

Theoretical part, shamelessly copied from the the second link
(http://qucs.sourceforge.net/tech/node14.html):

Suppose a circuit has
\li N nodes
\li M independent voltage sources

The structure of the circuit equation matrix:
\code
[ A ] * [ x ] = [ z ]

or

[ G B ] * [ v ] = [ i ]
[ C D ]   [ j ]   [ e ]
\endcode

The matixes are:
\li A - (N+M)*(N+M) matrix, consists only of known quantities
\li x -  (N+M)*1 vector, holds the unknown quantities
            (node voltages and
            the currents through the independent voltage sources)
\li z - is an (N+M)*1 vector that holds only known quantities

The A matrix consists of:
\li G - N*N matrix, determined by the interconnections between the circuit elements
\li B - N*M matrix, determined by the connection of the voltage sources
\li C - M*N matrix, determined by the connection of the voltage sources
        (B and C are closely related, particularly when only independent sources are considered)
\li D - M*M matrix, is zero if only independent sources are considered

The x matrix, storing the unkown values, consists of:
\li v - 1*N, matrix of unknown voltages in the nodes
\li j - 1*M, matrix of unknown currents through the voltage sources

The z matrix consists of:
\li i - 1*N matrix with each element of the matrix corresponding to a particular node.
        The value of each element of i is determined by the sum of current sources
        pointing into the corresponding node.
        If there are no current sources connected to the node, the value is zero.
\li e - 1*M matrix with each element of the matrix equal in value
        to the corresponding independent voltage source.

In the implementation,
the e part of the matrix z is called v.

The equation A*x = b is stored in matrixes, contained in an IElementSet.
The IElementSet allocates A,x and b matrixes of the needed size, and
assigns rows and columns for all IElements.
This way the base type IElement provides methods to translate
the number of the IPins
belonging to the element into positions in the matrixes.

\subsubsection sim_obj_hierarchy Object allocation hierarchy in the simulator

Inside the simulator, many objects are created and
many objects hold pointers to other objects.
However, it must be clearly defined, which object creates
a new object of a given type and role,
and the same creator object should destruct the created object.
This subsection presents such relations between the classes.

CircuitTransientSimulator:
\li m_allElementList: all elements in the simulation
\li m_allWireList: all wires in the simulation
\li m_pinGroups: all PinGroup objects

IElement:
\li m_pins: the pins of the specific element



\section component_map Key / Value pairs used in component models

In the document model there are components and connectors.
Both are are stored as QVariantMap.
This section describes the stucture of these QVariantMaps.
Generally, the QVariantMap returned by
IDocumentModel::components() and
IDocumentModel::connectors()
is a map ID -> QVariantMap, where
ID is idenfidier of the component, and the value is
a QVariantMap describing the component and connector.

The following subsections document the QVariantMaps used in different types
of documents.

\subsection circuit_qvariantmaps Circuit document

\li id: the ID of the component or connector

\subsubsection circuit_generic_maps Generic components

\li angle: 0, 90, 180, 270: the angle of rotation of a component
\li fileName: the name of the graphics, representing the component
\li flip: 0 or 1
\li type: the type of the component
\li x: X coordinate of the component
\li y: Z coordinate of the component
\li z: Z coordinate of the component, this represents the drawing order

\subsubsection circuit_connector_maps Connectors

\li end-node-cid
\li end-node-id (if end-node-is-child == 0)
\li end-node-is-child: 0 or 1
\li end-node-parent
\li manual-route: 0 or 1
\li route: string or point coordinates
\li start-node-id (if start-node-is-child == 0)
\li start-node-is-child: 0 or 1
\li start-node-cid: 
\li start-node-parent

\subsubsection circuit_component_pin_ids IDs of Pins, used in components

Battery == Independent Voltage Source
\li "n1", left, negative (or reverse?)
\li "p1", right, posibtve

Fixed Voltage
\li "p1", right, the voltage

Ground
\li "p1", right, ground

Current Source
\li "n1", left, output
\li "p1", right, output

Voltage Signal
\li "n1", left, negative (or reverse?)
\li "p1", right, posibtve 

Current Signal
\li "n1", left, current in
\li "p1", right, current out

Current Controlled Current Source
\li "n1", left, current control 1
\li "n2", left, current out 1
\li "p1", right, current control 2
\li "p2", right, current out 2

Current Controlled Voltage Source
\li "n1", left, current control 1
\li "n2", left, voltage out 1
\li "p1", right, current control 2
\li "p2", right, voltage out 2

Voltage Controlled Current Source
\li "n1", left, voltage control 1
\li "n2", left, current out 1
\li "p1", right, voltage control 2
\li "p2", right, current out 2

Voltage Controlled Voltage Source
\li "n1", left, voltage control 1
\li "n2", left, voltage out 1
\li "p1", right, voltage control 2
\li "p2", right, voltage out 2


Resistor
\li "n1", left
\li "p1", right

Capacitor
\li "n1" : left
\li "p1" : right

Inductor
\li "n1" : left
\li "p1" : right

Resistor DIP
\li "n0" .. "n64" .. , left
\li "p0" .. "p64" .. , right

Potentiometer
\li "p1", middle pin
\li "n1", one end
\li "n2", other end

Variable Capacitor
\li "n1" : left
\li "p1" : right

Variable Resistor
\li "n1" : left
\li "p1" : right


Diode
\li "n1", left, in
\li "p1", right, out

NPN transistor
\li "b", Base
\li "c", Collector
\li "e", Emitter

PNP transistor
\li "b", Base
\li "c", Collector
\li "e", Emitter

n-JFET
\li "D", drain
\li "G", gate
\li "S", source

p-JFET
\li "D", drain
\li "G", gate
\li "S", source

n-MOSFET
\li "g", gate
\li "d", drain
\li "s", source

p-MOSFET
\li "g", gate
\li "d", drain
\li "s", source

Switch Push-to-Make
\li "n1", left
\li "p1", right

Switch Push-to-Brake
\li "n1", left
\li "p1", right

Keypad
\li "row_0" .. "row_3"
\li "col_0" .. "col_8"

SPST - single package single terminal
\li "n1", left
\li "p1", right


DPST - dual package single terminal
\li "n1", left, A
\li "n2", left, B
\li "p1", right, A
\li "p2", right, B

SPDT - single package dual terminal
\li "n1", left, common 
\li "p1", right, choice
\li "p2", right, choice

DPDT - dual package dual terminal
\li "n1", left, common A
\li "n2", left, common B
\li "p1", right, choice A
\li "p2", right, choice A
\li "p3", right, choice B
\li "p4", right, choice B

Rotary
\li "in" : common pin
\li "pin_0" .. "pin_8" ..  : selectable outputs

LED
\li "n1", left, in
\li "p1", right, out

Signal Lamp
\li "n1" : left, one end
\li "p1" : right, other end

Bidirectional LED
\li "n1", left
\li "p1", right

Seven Segment
\li "a" .. "f" : segments
\li 0xB7 ("·") : decimal dot
\li "-v" : common anode / cathode

Matrix Display
\li "row_0" .. "row_7" ...
\li "col_0" .. "col_7" ...

Bar Graph Display
\li "p_0" .. "n_64" .. : inputs for current
\li "n_0" .. "n_64" .. : outputs for current

Voltmeter
\li "n1", left, minus
\li "p1", right, plus

Ammeter
\li "n1", left
\li "p1", right

Logic Probe
\li "p1", right

Voltage Probe
\li "n1", left
\li "p1", right

Current Probe
\li "n1", left
\li "p1", right


Logic Input
\li "p2", right, output

Logic Output
\li "n1", left, input

Clock Input
\li "p1", right, output

Buffer
\li "n1", left, input
\li "p1", right, output

Inverter
\li "n1", left, input
\li "p1", right, output

AND gate
\li "p1", right, output
\li "in1" .. "in64" .. : left, input

NAND gate
\li "p1", right, output
\li "in1" .. "in64" .. : left, input

OR gate
\li "p1", right, output
\li "in1" .. "in64" .. : left, input

NOR gate
\li "p1", right, output
\li "in1" .. "in64" .. : left, input

XOR gate
\li "p1", right, output
\li "in1" .. "in64" .. : left, input

XNOR gate
\li "p1", right, output
\li "in1" .. "in64" .. : left, input


Bus
\li "n1", left
\li "out_0" .. "out_64" .. : output pins, right

External Connection
\li "n1", left, the only connection

Serial Port
\li "CD"
\li "TD"
\li "TDR"
\li "GND"
\li "CTS"
\li "RI"

Parallel Port
\li "STROBE",
\li "AUTO",
\li "INIT",
\li "SELECT"
\li "ERROR",
\li "ONLINE",
\li "PE",
\li "ACK",
\li "BUSY"
\li "D1" .. "D8" : data

PIC
the pins are coming from thge MircoPackage class...
TODO find out the pins...

JK Flip-Flop
\li "n1", left, J in
\li "n2", left, clock in
\li "n3", left, K in
\li "p1", right, Q out
\li "p2", right, inverted Q out

SR Flip-Flop
\li "n1", left, S input
\li "n2", left, R input
\li "p1", right, Q output
\li "p2", right, inverted Q output

D Flip-Flop
\li "n1", left, D input
\li "n2", left, clock input
\li "p1", right, Q output
\li "p2", right, inverted Q output

555
\li "Dis", left, dicharge
\li "Th", left, threshold
\li "Trg", left, trigger
\li "Vcc", top, supply
\li "Res", top, reset
\li "Gnd", bottom, ground
\li "CV", bottom, control
\li "out", right, output

RAM
\li "CS", left, chip select
\li "OE", left, output enable
\li "WE", left, write enable
\li "A0" .. "A8" .. , left: address in
\li "DI0" .. "DI8" .. , right: data in
\li "DO0" .. "DO8" .. , right: data out

Adder
\li "A", left, in 1
\li "B", left, in 2
\li ">", left, carry in
\li "S", right, sum out
\li "C", right, carry out

Multiplexer
\li "A0" .. "A64" .. : address inputs
\li "X0" .. "X64" .. : data inputs
\li "X" : data output

Demultiplexer
\li "A0" .. "A64" : address bits
\li "X" : input
\li "X0" .. "X64" : outputs

Magnitude Comparator
\li "A0" .. "A64" .. : inputs A
\li "B0" .. "B64" .. : inputs B
\li "I: A\>B", input
\li "I: A\<B", input
\li "I: A=B", input
\li "O: A\>B", output
\li "O: A\<B", output
\li "O: A=B", output

Op Amp
\li "n1", left, noninverting input
\li "n2", left, inverting input
\li "p1", right, out

Analog-Digital
\li "In" : analog input
\li "0" .. "number of bits - 1" : digital outputs

Digital-Analog
\li "Out" : analog output
\li "0" .. "number of bits - 1" : digital inputs

Binary Counter
\li "en" : enable
\li ">" : clock in
\li "u/d" : up/down count
\li "r" : reset
\li "A" .. "Z" .. : outputs

Matrix Display Driver
\li "D0" .. "D7" : data in
\li "C0" .."C4" : column out
\li "R0" .. "R6" : row out

BCD to 7 Segment
\li "A", number in
\li "B", number in
\li "C", number in
\li "D", number in
\li "lt", lamp test
\li "rb", ripple bank
\li "en", enable (store)
\li "a", output
\li "b", output
\li "c", output
\li "d", output
\li "e", output
\li "f", output
\li "g", output

Subcircuit
\li "0" .. "64" .. : connections

\subsection flowcode_qvariantmaps Flowcode documents

TODO

\section new_elements Adding support for new Elements (component models)

TODO

\section new_routers Adding support for new routing algorithms

TODO

\section new_doctype Adding support for new document types

TODO write this document

*/

