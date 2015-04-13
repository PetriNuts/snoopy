#.rst:
# FindwxCode
# --------
#
# Find wxCode
#
# Find the wxCode includes and library 
# This module defines
#
# ::
#
#   wxCode_INCLUDE_DIR, where to find jpeglib.h, etc.
#   wxCode_LIBRARIES, the libraries needed to use wxCode.
#   wxCode_FOUND, If false, do not try to use wxCode.
#
# also defined, but not for general use are
#
# ::
#
#   wxCode_LIBRARY, where to find the wxCode library.

if(NOT wxWidgets_FOUND)
  message(FATAL_ERROR " wxWidgets not found")
endif()

#=====================================================================
# Determine whether unix or win32 paths should be used
#=====================================================================
if(WIN32 AND NOT CYGWIN AND NOT MSYS AND NOT CMAKE_CROSSCOMPILING)
  set(wxCode_FIND_STYLE "win32")
else()
  set(wxCode_FIND_STYLE "unix")
endif()

#=====================================================================
# WIN32_FIND_STYLE
#=====================================================================
if(wxWidgets_FIND_STYLE STREQUAL "win32")
  message(FATAL_ERROR " not implemented yet")
endif()

#=====================================================================
# UNIX_FIND_STYLE
#=====================================================================
if(wxWidgets_FIND_STYLE STREQUAL "unix")
  string(REGEX MATCH "([A-Za-z0-9_]+)-([A-Za-z0-9_]+)-([A-Za-z0-9_]+)?-?([0-9]+)\\.([0-9]+)"
         _RESULT "${wxWidgets_INCLUDE_DIRS}")
  string(REPLACE "-" ";" _RESULT "${_RESULT}")
  list(LENGTH _RESULT n)
  list(GET _RESULT 0 wxCode_TOOLKIT)
  list(GET _RESULT 1 wxCode_UNICODE)
  if(n EQUAL 3)
    set(wxCode_CONFIGURATION "")
    list(GET _RESULT 2 wxCode_VERSION)
  else()
    list(GET _RESULT 2 wxCode_CONFIGURATION)
    list(GET _RESULT 3 wxCode_VERSION)
  endif()
  unset(_RESULT)
endif()

if(wxCode_UNICODE STREQUAL "unicode")
  set(wxCode_UNICODE "u")
else()
  set(wxCode_UNICODE "")
endif()

if(wxCode_CONFIGURATION STREQUAL "debug")
  set(wxCode_CONFIGURATION "d")
else()
  set(wxCode_CONFIGURATION "")
endif()

if(NOT wxCode_FIND_COMPONENTS)
    message(ERROR " Please specify the components you want to use!")
    set(wxCode_FOUND false)
else()
    set(wxCode_FOUND true)
endif()

foreach(COMPONENT ${wxCode_FIND_COMPONENTS})
  string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
  find_path(wxCode_${UPPERCOMPONENT}_INCLUDE_DIR 
      NAMES wx/${COMPONENT}.h wx/${COMPONENT}/${COMPONENT}.h wx/wx${COMPONENT}.h wx/wx${COMPONENT}defs.h
      PATH_SUFFIXES wx
      )
  find_library(wxCode_${UPPERCOMPONENT}_LIBRARY 
      NAMES wxcode_${wxCode_TOOLKIT}${wxCode_UNICODE}${wxWidgets_CONFIGURATION}_${COMPONENT}-${wxCode_VERSION} libwxcode_${wxCode_TOOLKIT}${wxCode_UNICODE}${wxWidgets_CONFIGURATION}_${COMPONENT}-${wxCode_VERSION}
      )
endforeach()

# handle the QUIETLY and REQUIRED arguments and set CVODE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)

foreach(COMPONENT ${wxCode_FIND_COMPONENTS})
  string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(wxCode_${UPPERCOMPONENT} FOUND_VAR wxCode_${UPPERCOMPONENT}_FOUND
      REQUIRED_VARS wxCode_${UPPERCOMPONENT}_LIBRARY wxCode_${UPPERCOMPONENT}_INCLUDE_DIR )
endforeach()

foreach(COMPONENT ${wxCode_FIND_COMPONENTS})
  string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
  if(wxCode_${UPPERCOMPONENT}_FOUND)
    list(APPEND wxCode_LIBRARIES ${wxCode_${UPPERCOMPONENT}_LIBRARY} )
    list(APPEND wxCode_INCLUDE_DIRS ${wxCode_${UPPERCOMPONENT}_INCLUDE_DIR} )
  else()
    set(wxCode_FOUND false)
  endif()
endforeach()

mark_as_advanced( wxCode_INCLUDE_DIR )
