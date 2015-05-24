# CMake module to find GPSim
#
# possibly defined variables after this script is run:
# GPSIM_FOUND
# HAVE_GPSIM_0_26		# TODO create a GPSIM_VERSION variable, and use that
# HAVE_GPSIM_0_21_4
# HAVE_GPSIM_0_21_12
# HAVE_GPSIM_0_21_11
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


# HAVE_GPSIM_0_21_12

CHECK_INCLUDE_FILE_CXX( ${GPSim_INCLUDE_DIR}/ValueCollections.h HAVE_GPSIM_0_21_12 )

if( HAVE_GPSIM_0_21_12 )
	message( STATUS "GPSim 0.21.12 found" )
	set( GPSIM_0_21_12 "GPSim 0.21.12 found" )
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
endif( HAVE_GPSIM_0_21_12 )


# HAVE_GPSIM_0_21_4

CHECK_CXX_SOURCE_COMPILES(
		"#include <gpsim/interface.h>
		#include <gpsim/gpsim_interface.h>
		#include <gpsim/breakpoints.h>
		#include <gpsim/gpsim_time.h>
		void func() { (void)cycles; (void)initialize_gpsim_core();
		(void)load_symbol_file(0,0); }" HAVE_GPSIM_0_21_4 )


if( HAVE_GPSIM_0_21_4 )
	message( STATUS "GPSim 0.21.4 found" )
	set( GPSIM_0_21_4 "GPSim 0.21.4 found" )
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
endif( HAVE_GPSIM_0_21_4 )


# HAVE_GPSIM_0_21_11

CHECK_CXX_SOURCE_COMPILES(
		"#include <gpsim/interface.h>
		#include <gpsim/gpsim_interface.h>
		#include <gpsim/breakpoints.h>
		#include <gpsim/gpsim_time.h>
		#include <gpsim/symbol.h>
		#include <gpsim/program_files.h>
		void func() { (void)cycles; (void)initialize_gpsim_core(); }"
		HAVE_GPSIM_0_21_11 )

if( HAVE_GPSIM_0_21_11 )
	message( STATUS "GPSim 0.21.11 found" )
	set( GPSIM_0_21_11 "GPSim 0.21.11 found" )
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
endif( HAVE_GPSIM_0_21_11 )


# any other case

message( STATUS "No GPSim found; unknown version?" )
set( NO_GPSIM "No GPSim found; unknown version?" )
