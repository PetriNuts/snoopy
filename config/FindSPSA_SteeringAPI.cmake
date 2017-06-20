#.rst:
# FindSPSA_SteeringAPI
# --------
#
# Find SPSA_SteeringAPI
#
# Find the SPSA_SteeringAPI includes and library 
# This module defines
#
# ::
#
#   SPSA_SteeringAPI_INCLUDE_DIR, where to find header.h, etc.
#   SPSA_SteeringAPI_LIBRARIES, the libraries needed to use SPSA_SteeringAPI.
#   SPSA_SteeringAPI_FOUND, If false, do not try to use SPSA_SteeringAPI.
#
# also defined, but not for general use are
#
# ::
#
#   SPSA_SteeringAPI_LIBRARY, where to find the SPSA_SteeringAPI library.

find_path(SPSA_SteeringAPI_INCLUDE_DIR NAMES spsa/spsa.h
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../SPSA_SteeringAPI
          ${CMAKE_CURRENT_SOURCE_DIR}/../SP_SteeringAPI
          $ENV{SPSA_STEERINGAPI}
    PATH_SUFFIXES include spsa
    )

find_library(SPSA_SteeringAPI_LIBRARY_RELEASE 
    NAMES SPSA_SteeringAPI libSPSA_SteeringAPI
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../SPSA_SteeringAPI
          ${CMAKE_CURRENT_SOURCE_DIR}/../SP_SteeringAPI
          $ENV{SPSA_STEERINGAPI}
    PATH_SUFFIXES Release Release_MacOSX_Univ Release_Linux
    )

find_library(SPSA_SteeringAPI_LIBRARY_DEBUG 
    NAMES SPSA_SteeringAPI libSPSA_SteeringAPI
        HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../SPSA_SteeringAPI
        ${CMAKE_CURRENT_SOURCE_DIR}/../SP_SteeringAPI
        $ENV{SPSA_STEERINGAPI}
    PATH_SUFFIXES Debug Debug_MacOSX_Univ Debug_Linux
    )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(SPSA_SteeringAPI_LIBRARY ${SPSA_SteeringAPI_LIBRARY_DEBUG})
else()
  set(SPSA_SteeringAPI_LIBRARY ${SPSA_SteeringAPI_LIBRARY_RELEASE})
endif()

# handle the QUIETLY and REQUIRED arguments and set SPSA_SteeringAPI_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SPSA_SteeringAPI FOUND_VAR SPSA_SteeringAPI_FOUND
      REQUIRED_VARS SPSA_SteeringAPI_LIBRARY SPSA_SteeringAPI_INCLUDE_DIR)

if(SPSA_SteeringAPI_FOUND)
  list(APPEND SPSA_SteeringAPI_LIBRARIES ${SPSA_SteeringAPI_LIBRARY} )
  list(APPEND SPSA_SteeringAPI_INCLUDE_DIRS ${SPSA_SteeringAPI_INCLUDE_DIR} )
endif()

mark_as_advanced( SPSA_SteeringAPI_INCLUDE_DIR )
