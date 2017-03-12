# CMake module to find GPSim
#
# It provides the following variables:
#  GPSim_FOUND - system has the GPSim library
#  GPSim_INCLUDE_DIRS - the include directories needed to use GPSim
#  GPSim_LIBRARIES - the libraries needed to use GPSim
#  HAVE_GPSIM_0_26 - whether GPSim is >= 0.26

# Copyright (c) 2017, Pino Toscano <pino@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


# in cache already
if (GPSim_INCLUDE_DIR AND GPSim_LIBRARY)
    set(GPSim_FIND_QUIETLY TRUE)
endif ()

find_package(GLib)  # needed for using GPSim

find_path(GPSim_INCLUDE_DIR
    NAMES gpsim_interface.h
    PATH_SUFFIXES gpsim
)
find_library(GPSim_LIBRARY NAMES gpsim)

if (GPSim_INCLUDE_DIR AND GPSim_LIBRARY AND GLib_FOUND)
    set(GPSim_INCLUDE_DIRS ${GPSim_INCLUDE_DIR} ${GLib_INCLUDE_DIRS})
    set(GPSim_LIBRARIES ${GPSim_LIBRARY} ${GLib_LIBRARY})

    include(CheckCXXSourceCompiles)

    macro_push_required_vars()
    set(CMAKE_REQUIRED_INCLUDES ${GPSim_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES ${GPSim_LIBRARIES})
    set(CMAKE_REQUIRED_DEFINITIONS ${KDE4_ENABLE_EXCEPTIONS})
    check_cxx_source_compiles(
"#include <gpsim/pic-processor.h>
int main() { pic_processor *proc = NULL;
return sizeof (proc->Wreg);
}" HAVE_GPSIM_0_26)
    macro_pop_required_vars()
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GPSim
    FOUND_VAR GPSim_FOUND
    REQUIRED_VARS GPSim_LIBRARY GPSim_INCLUDE_DIR GLib_LIBRARY GLib_INCLUDE_DIRS
)

mark_as_advanced(GPSim_INCLUDE_DIR GPSim_LIBRARY)
