/**

\mainpage

\section intro Introduction

This page in the documentation will describe the general architecture of KTechLab,
from developer point of view. KTechLab is an application suitable for developing,
exploring and learning electronics. It ships a set of plugins and tools to let the
user master different tasks common in this field.

\section src_org Organization of the source code

KTechLab internally is based on KDevPlatform.
See http://www.kdevelop.org/mediawiki/index.php/KDevelop_4#Platform_Information .

The source code has similar layout as the above mentioned platform.
It uses the plugin infrastructure to provide support for different aspects of working with
electronics.

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
\li automatic_router: layout connections between components
\li flowcode: support for the FlowCode document type -- currently not functional
\li mcu_pic: support for PICs -- currently not functional
\li probevis: probe visualisation -- currently not functional
\li simulator support for simulating circuits

The plugins can be categorized like in this map:
\image html pluginmap.svg Plugin map

The following subsections describe various aspects of the program.

\subsection main Main function

TODO

\subpage circuits

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

\section new_routers Adding support for new routing algorithms

TODO

\section new_doctype Adding support for new document types

TODO write this document

*/

