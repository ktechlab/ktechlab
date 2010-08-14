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

\section src_org Organization of the source code

KTechLab internally is based on KDevPlatform.
See http://www.kdevelop.org/mediawiki/index.php/KDevelop_4#Platform_Information
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

\section new_elements Adding support for new Elements (component models)

TODO

\section new_routers Adding support for new routing algorithms

TODO

\section new_doctype Adding support for new document types

TODO write this document

*/

