include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

kde4_install_icons(${DATA_INSTALL_DIR}/ktechlab/icons)

install(FILES
	ammeter.png
	and.png
	assignment.png
	bidirled.png
	branch.png
	buffer.png
	bus.png
	capacitor.png
	cccs.png
	ccvs.png
	cell.png
	circuit.png
	clockinput.png
	current_source.png
	currentsignal.png
	delay.png
	diode.png
	dpdt.png
	dpst.png
	embed.png
	end.png
	external_connection.png
	external_link.png
	floatingprobe.png
	flowcode.png
	for.png
	ground.png
	ic1.png
	ic2.png
	ic3.png
	inductor.png
	interrupt.png
	item.png
	keypad.png
	led.png
	logic_input.png
	logic_output.png
	logicprobe.png
	matrixdisplay.png
	mechanics.png
	nand.png
	nemosfet.png
	njfet.png
	nor.png
	not.png
	npn.png
	opamp.png
	or.png
	pemosfet.png
	pinread.png
	pinwrite.png
	pjfet.png
	pnp.png
	portread.png
	portwrite.png
	potentiometer.png
	project_library.png
	project_program.png
	ptb.png
	ptm.png
	repeat.png
	resistor.png
	resistordip.png
	rotary.png
	seven_segment.png
	signal_lamp.png
	spdt.png
	spst.png
	start.png
	sub.png
	subcall.png
	unary.png
	variable_capacitor.png
	variable_resistor.png
	vccs.png
	vcvs.png
	voltage.png
	voltagesignal.png
	voltmeter.png
	while.png
	xnor.png
	xor.png
	
	DESTINATION ${DATA_INSTALL_DIR}/ktechlab/icons)

#original Makefile.am contents follow:

#
#METASOURCES = AUTO
#ktlicon_DATA = and.png cell.png logic_input.png logic_output.png nand.png \
#				nor.png not.png or.png xor.png current_source.png ground.png voltage.png \
#				capacitor.png led.png resistor.png signal_lamp.png seven_segment.png ammeter.png \
#				diode.png npn.png pnp.png voltmeter.png dpdt.png dpst.png ptb.png ptm.png \
#				spdt.png spst.png keypad.png opamp.png xnor.png circuit.png flowcode.png \
#				delay.png interrupt.png pinread.png pinwrite.png portread.png portwrite.png \
#				repeat.png while.png for.png assignment.png branch.png delay.png embed.png end.png \
#				for.png interrupt.png pinread.png pinwrite.png portread.png portwrite.png \
#				repeat.png start.png sub.png subcall.png unary.png while.png item.png ic1.png \
#				ic2.png ic3.png voltagesignal.png currentsignal.png clockinput.png cccs.png \
#				ccvs.png vcvs.png vccs.png mechanics.png buffer.png external_connection.png \
#				logicprobe.png potentiometer.png resistordip.png bidirled.png project_library.png \
#				project_program.png bus.png floatingprobe.png inductor.png matrixdisplay.png 		rotary.png \
#		variable_capacitor.png variable_resistor.png external_link.png nemosfet.png njfet.png pemosfet.png pjfet.png
#ktlicondir = $(kde_datadir)/ktechlab/icons
#	
#
