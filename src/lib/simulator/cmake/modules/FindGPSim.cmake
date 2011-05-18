
FIND_PATH( GPSim_INCLUDE_DIR gpsim_interface.h /usr/include/gpsim /usr/local/include/gpsim )

FIND_LIBRARY( GPSim_LIBRARY NAMES gpsim PATH /usr/lib /usr/local/lib )

if( GPSim_INCLUDE_DIR AND GPSim_LIBRARY )
	set( GPSim_FOUND TRUE )
endif( GPSim_INCLUDE_DIR AND GPSim_LIBRARY )

if ( GPSim_FOUND )
	INCLUDE( CheckCXXSourceCompiles )

	set( CMAKE_REQUIRED_INCLUDES
		${GLIB2_INCLUDE_DIRS}
		)
	set( CMAKE_REQUIRED_FLAGS
		-fexceptions
		)

	CHECK_CXX_SOURCE_COMPILES(
        	"#include <gpsim/interface.h>
        	#include <gpsim/gpsim_interface.h>
        	#include <gpsim/breakpoints.h>
        	#include <gpsim/gpsim_time.h>
        	void func() { (void)cycles; (void)initialize_gpsim_core();
        	(void)load_symbol_file(0,0); }" HAVE_GPSIM_0_21_4 )

	CHECK_CXX_SOURCE_COMPILES(
        	"#include <gpsim/interface.h>
		#include <gpsim/gpsim_interface.h>
        	#include <gpsim/breakpoints.h>
        	#include <gpsim/gpsim_time.h>
        	#include <gpsim/symbol.h>
        	#include <gpsim/program_files.h>
        	void func() { (void)cycles; (void)initialize_gpsim_core(); }"
        	HAVE_GPSIM_0_21_11 )

	INCLUDE( CheckIncludeFileCXX )

	CHECK_INCLUDE_FILE_CXX( ${GPSim_INCLUDE_DIR}/ValueCollections.h HAVE_GPSIM_0_21_12 )

	if( HAVE_GPSIM_0_21_12 )
        	message( STATUS "GPSim 0.21.12 found" )
        	set( GPSIM_0_21_12 "GPSim 0.21.12 found" )
	else( HAVE_GPSIM_0_21_12 )
        	if( HAVE_GPSIM_0_21_4 )
                	message( STATUS "GPSim 0.21.4 found" )
                	set( GPSIM_0_21_4 "GPSim 0.21.4 found" )
        	else( HAVE_GPSIM_0_21_4 )
                	if( HAVE_GPSIM_0_21_11 )
                        	message( STATUS "GPSim 0.21.11 found" )
                        	set( GPSIM_0_21_11 "GPSim 0.21.11 found" )
                	else( HAVE_GPSIM_0_21_11 )
                        	message( STATUS "No GPSim found" )
                        	set( NO_GPSIM "No GPSim found" )
                	endif( HAVE_GPSIM_0_21_11 )
        	endif( HAVE_GPSIM_0_21_4 )
	endif( HAVE_GPSIM_0_21_12 )

	INCLUDE( CheckLibraryExists )
	CHECK_LIBRARY_EXISTS( readline readline /usr/lib HAVE_LIBREADLINE )

	find_package( Curses )

	if( HAVE_LIBREADLINE )
		set( GPSIM_INCLUDE_DIRS 
			${GPSim_INCLUDE_DIR}
			${GLIB2_INCLUDE_DIRS}
			)
		set( GPSIM_LIBRARY
			-lreadline
			${GPSim_LIBRARY}
			${GLIB2_LIBRARY}
			)
		set( GPSIM_FOUND TRUE )
	else( HAVE_LIBREADLINE )

	endif( HAVE_LIBREADLINE )
endif( GPSim_FOUND)
