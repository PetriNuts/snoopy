#.rst:
# FindDSSZ_UTIL
# --------
#
# Find DSSZ_UTIL
#
# Find the DSSZ_UTIL includes and library 
# This module defines
#
# ::
#
#   DSSZ_UTIL_INCLUDE_DIR, where to find header.h, etc.
#   DSSZ_UTIL_LIBRARIES, the libraries needed to use DSSZ_UTIL.
#   DSSZ_UTIL_FOUND, If false, do not try to use DSSZ_UTIL.
#
# also defined, but not for general use are
#
# ::
#
#   DSSZ_UTIL_LIBRARY, where to find the DSSZ_UTIL library.

find_path(DSSZ_UTIL_INCLUDE_DIR NAMES dssz/functions/functions.h
    PATH_SUFFIXES dssz
    )

find_library(DSSZ_UTIL_LIBRARY 
    NAMES dssz_util libdssz_util
    )

# handle the QUIETLY and REQUIRED arguments and set DSSZ_UTIL_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DSSZ_UTIL DEFAULT_MSG DSSZ_UTIL_LIBRARY DSSZ_UTIL_INCLUDE_DIR)

if(DSSZ_UTIL_FOUND)
  list(APPEND DSSZ_UTIL_LIBRARIES ${DSSZ_UTIL_LIBRARY} )
  list(APPEND DSSZ_UTIL_INCLUDE_DIRS ${DSSZ_UTIL_INCLUDE_DIR} )
endif()

mark_as_advanced( DSSZ_UTIL_INCLUDE_DIR )
