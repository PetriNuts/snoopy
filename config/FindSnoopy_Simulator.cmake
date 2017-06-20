#.rst:
# FindSnoopy_Simulator
# --------
#
# Find Snoopy_Simulator
#
# Find the Snoopy_Simulator includes and library 
# This module defines
#
# ::
#
#   Snoopy_Simulator_INCLUDE_DIR, where to find header.h, etc.
#   Snoopy_Simulator_LIBRARIES, the libraries needed to use Snoopy_Simulator.
#   Snoopy_Simulator_FOUND, If false, do not try to use Snoopy_Simulator.
#
# also defined, but not for general use are
#
# ::
#
#   Snoopy_Simulator_LIBRARY, where to find the Snoopy_Simulator library.

find_path(Snoopy_Simulator_INCLUDE_DIR 
    NAMES spsim/spsim.h
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../Snoopy\ Simulator
          ${CMAKE_CURRENT_SOURCE_DIR}/../SP_Simulator
          $ENV{SP_SIMULATOR}
    PATH_SUFFIXES include spsim
    )

find_library(Snoopy_Simulator_LIBRARY_RELEASE 
    NAMES spsim_dr libspsim_dr spsim_sr libspsim_sr
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../Snoopy\ Simulator
          ${CMAKE_CURRENT_SOURCE_DIR}/../SP_Simulator
          $ENV{SP_SIMULATOR}
    PATH_SUFFIXES Release Release_MacOSX_Univ Release_Linux
    )

find_library(Snoopy_Simulator_LIBRARY_DEBUG 
    NAMES spsim_dd libspsim_dd spsim_sd libspsim_sd
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../Snoopy\ Simulator
          ${CMAKE_CURRENT_SOURCE_DIR}/../SP_Simulator
          $ENV{SP_SIMULATOR}
    PATH_SUFFIXES Debug Debug_MacOSX_Univ Debug_Linux
    )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(Snoopy_Simulator_LIBRARY ${Snoopy_Simulator_LIBRARY_DEBUG})
else()
  set(Snoopy_Simulator_LIBRARY ${Snoopy_Simulator_LIBRARY_RELEASE})
endif()

# handle the QUIETLY and REQUIRED arguments and set Snoopy_Simulator_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Snoopy_Simulator FOUND_VAR Snoopy_Simulator_FOUND
      REQUIRED_VARS Snoopy_Simulator_LIBRARY Snoopy_Simulator_INCLUDE_DIR)

if(Snoopy_Simulator_FOUND)
  list(APPEND Snoopy_Simulator_LIBRARIES ${Snoopy_Simulator_LIBRARY} )
  list(APPEND Snoopy_Simulator_INCLUDE_DIRS ${Snoopy_Simulator_INCLUDE_DIR} )
endif()

mark_as_advanced( Snoopy_Simulator_INCLUDE_DIR )
