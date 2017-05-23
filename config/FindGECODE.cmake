#.rst:
# FindGECODE
# --------
#
# Find GECODE
#
# Find the GECODE includes and library 
# This module defines
#
# ::
#
#   GECODE_INCLUDE_DIRS, where to find header.h, etc.
#   GECODE_LIBRARIES, the libraries needed to use GECODE.
#   GECODE_FOUND, If false, do not try to use GECODE.
#
# also defined, but not for general use are
#
# ::
#
#   GECODE_LIBRARY, where to find the GECODE library.

find_path(GECODE_INCLUDE_DIR NAMES gecode/driver.hh
        HINTS ENV GECODEDIR
        PATH_SUFFIXES gecode
        )

find_path(GECODE_CONFIG_INCLUDE_DIR NAMES gecode/support/config.hpp
        HINTS ENV GECODEDIR
        PATH_SUFFIXES gecode build Release Debug
        )

if(NOT GECODE_FIND_COMPONENTS)
  # Assume they want default
  set(GECODE_FIND_COMPONENTS driver int float kernel minimodel search set support)
endif()

foreach(COMPONENT ${GECODE_FIND_COMPONENTS})
  string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
  find_library(GECODE_${UPPERCOMPONENT}_LIBRARY
          HINTS ENV GECODEDIR
          NAMES gecode${COMPONENT} libgecode${COMPONENT} Gecode${COMPONENT}
          PATH_SUFFIXES lib build Release Debug build/Release build/Debug
          )
endforeach()

# handle the QUIETLY and REQUIRED arguments and set GECODE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GECODE DEFAULT_MSG GECODE_INCLUDE_DIR)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GECODE DEFAULT_MSG GECODE_CONFIG_INCLUDE_DIR)

foreach(COMPONENT ${GECODE_FIND_COMPONENTS})
  string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(GECODE_${UPPERCOMPONENT} DEFAULT_MSG GECODE_${UPPERCOMPONENT}_LIBRARY GECODE_INCLUDE_DIR)
endforeach()

list(APPEND GECODE_INCLUDE_DIRS ${GECODE_INCLUDE_DIR} ${GECODE_CONFIG_INCLUDE_DIR})

foreach(COMPONENT ${GECODE_FIND_COMPONENTS})
  string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
  if(GECODE_${UPPERCOMPONENT}_FOUND)
    list(APPEND GECODE_LIBRARIES ${GECODE_${UPPERCOMPONENT}_LIBRARY} )
  else()
    set(GECODE_FOUND false)
  endif()
endforeach()

mark_as_advanced( GECODE_INCLUDE_DIR )
