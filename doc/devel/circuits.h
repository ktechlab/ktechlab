/**
\page circuits Creating and simulating electrical circuits

After having read this part of the documentation, you will know about the components
of KTechLab related to electrical circuits. You will learn about the plugins involved
and the interfaces used to communicate between different plugins. This will lead to a point
where you can start writing your own plugins to extend existing functionality.

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

\section plugins Plugins related to circuits

\subsection overview How plugins work together

There are several plugins involved in handling circuit-files. The following diagram shows, how these
plugins interact:

\image html communication_ktlcircuit.svg "Interaction of circuit-related plugins"

As you can see, the different plugins communicate via public interfaces. The
KTechLab::KTLCircuitPlugin uses KTechLab::IComponentFactory implementations to
provide KTechLab::IComponentItem instances for the circuit files, which can be
drawn on a KTechLab::CircuitScene. On the other hand, there
is the simulation part that uses KTechLab::IElementFactory implementitions to provide
KTechLab::IElement instances. These are used by the simulator to simulate the corresponding
model of the component. Plugins providing at least one of these factories are considered to
be component plugins. The circuit plugin provides a GUI to the user and stores all data concerning
a circuit into a KTechLab::CircuitModel. This model stores information like which component is
connected with which pin to another components pin. It also contains persistant information about how these
“connections” are layed out (routed) on the screen.

\subsection ktlcircuit The KTechLab::KTLCircuitPlugin
\p
The circuit plugin provides support for KTechLab’s circuit-files. When the user tries to
open a file with the application/x-circuit mime-type, the plugin will take care to load the
contents into a data model implementing the KTechLab::ICircuitModel interface. The circuit
will be rendered by an KTechLab::IDocumentScene onto a QGraphicsView. The rendering
of components will be themed (see also: \ref KTechLab::Theme). Through this user
interface, the user will be able to manipulate the circuit. With the help of
KTechLab::IRoutingInformation the connectors get rerouted, whenever a component
has been moved by the user. Plugins providing this extension can also aid the user
laying out the components by aligning them to a grid.

\p
Plugins providing the ISimulator extension can be used to simulate the circuit.
The simulated data can be visualized directly in the circuit, if the simulating
plugin provides the results of its computations via the KTechLab::INAModel interface.

\section circuit_simulation Simulation of circuits

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

\section new_elements Adding support for new Elements (component models)

TODO

*/