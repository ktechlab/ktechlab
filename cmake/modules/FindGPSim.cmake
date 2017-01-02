# CMake module to find GPSim
#
# possibly defined variables after this script is run:
# GPSIM_FOUND
# HAVE_GPSIM_0_26		# TODO create a GPSIM_VERSION variable, and use that
# NO_GPSIM
#
# start by looking for 'curses' library

FIND_PATH( GPSim_INCLUDE_DIR gpsim_interface.h /usr/include/gpsim /usr/local/include/gpsim )

FIND_LIBRARY( GPSim_LIBRARY NAMES gpsim PATH /usr/lib /usr/local/lib )

if( GPSim_INCLUDE_DIR AND GPSim_LIBRARY )
	set( GPSim_FOUND TRUE )
elseif( GPSim_INCLUDE_DIR AND GPSim_LIBRARY )
	message(STATUS "No GPSim found; files not present")
	SET( NO_GPSIM "No GPSim found; files not present" )
	return()
endif( GPSim_INCLUDE_DIR AND GPSim_LIBRARY )


INCLUDE( CheckCXXSourceCompiles )
INCLUDE( CheckIncludeFileCXX )

set( CMAKE_REQUIRED_INCLUDES
	${GLIB2_INCLUDE_DIRS}
	)
set( CMAKE_REQUIRED_FLAGS
	-fexceptions
	)

# HAVE_GPSIM_0_26

CHECK_CXX_SOURCE_COMPILES(
		"#include <gpsim/pic-processor.h>
		int main() { pic_processor *proc = NULL;
		return sizeof (proc->Wreg);
		}"
		HAVE_GPSIM_0_26 )

if( HAVE_GPSIM_0_26 )
	message(STATUS "GPSim 0.26 found")
	set( GPSIM_0_26 "GPSim 0.26 found" )
	set( GPSIM_INCLUDE_DIRS
		${GPSim_INCLUDE_DIR}
		${GLIB2_INCLUDE_DIRS}
		)
	set( GPSIM_LIBRARY
		${GPSim_LIBRARY}
		${GLIB2_LIBRARY}
		)
	set( GPSIM_FOUND TRUE )
	return()
endif( HAVE_GPSIM_0_26 )


# any other case

message( STATUS "No GPSim found; unknown version?" )
set( NO_GPSIM "No GPSim found; unknown version?" )
