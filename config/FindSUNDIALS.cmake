#.rst:
# FindSUNDIALS
# --------
#
# Find SUNDIALS
#
# Find the SUNDIALS includes and library 
# This module defines
#
# ::
#
#   SUNDIALS_INCLUDE_DIR, where to find jpeglib.h, etc.
#   SUNDIALS_LIBRARIES, the libraries needed to use SUNDIALS.
#   SUNDIALS_FOUND, If false, do not try to use SUNDIALS.
#
# also defined, but not for general use are
#
# ::
#
#   SUNDIALS_LIBRARY, where to find the SUNDIALS library.

find_path(SUNDIALS_INCLUDE_DIR NAMES sundials/sundials_types.h
    PATH_SUFFIXES sundials
    )

if(NOT SUNDIALS_FIND_COMPONENTS)
    # Assume they want cvode
    set(SUNDIALS_FIND_COMPONENTS cvode nvecserial)
endif()

foreach(COMPONENT ${SUNDIALS_FIND_COMPONENTS})
  string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
  find_library(SUNDIALS_${UPPERCOMPONENT}_LIBRARY 
      NAMES sundials_${COMPONENT} libsundials_${COMPONENT}
      )
endforeach()

# handle the QUIETLY and REQUIRED arguments and set CVODE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SUNDIALS REQUIRED_VARS SUNDIALS_INCLUDE_DIR)

foreach(COMPONENT ${SUNDIALS_FIND_COMPONENTS})
  string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(SUNDIALS_${UPPERCOMPONENT} 
      REQUIRED_VARS SUNDIALS_${UPPERCOMPONENT}_LIBRARY SUNDIALS_INCLUDE_DIR)
endforeach()

list(APPEND SUNDIALS_INCLUDE_DIRS ${SUNDIALS_INCLUDE_DIR} )

foreach(COMPONENT ${SUNDIALS_FIND_COMPONENTS})
  string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
  if(SUNDIALS_${UPPERCOMPONENT}_FOUND)
    list(APPEND SUNDIALS_LIBRARIES ${SUNDIALS_${UPPERCOMPONENT}_LIBRARY} )
  else()
    set(SUNDIALS_FOUND false)
  endif()
endforeach()

mark_as_advanced( SUNDIALS_INCLUDE_DIR )
