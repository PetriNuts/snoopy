#.rst:
# FindDSSD_UTIL
# --------
#
# Find DSSD_UTIL
#
# Find the DSSD_UTIL includes and library
# This module defines
#
# ::
#
#   DSSD_UTIL_INCLUDE_DIR, where to find header.h, etc.
#   DSSD_UTIL_LIBRARIES, the libraries needed to use DSSD_UTIL.
#   DSSD_UTIL_FOUND, If false, do not try to use DSSD_UTIL.
#
# also defined, but not for general use are
#
# ::
#
#   DSSD_UTIL_LIBRARY, where to find the DSSD_UTIL library.

find_path(DSSD_UTIL_INCLUDE_DIR NAMES dssd/functions/functions.h
        HINTS ENV DSSD_UTIL
        PATH_SUFFIXES include dssd
        )

find_library(DSSD_UTIL_LIBRARY
        HINTS ENV DSSD_UTIL
        NAMES dssd_util libdssd_util
        PATH_SUFFIXES lib build Release Debug build/Release build/Debug lib/Release lib/Debug
        )

# handle the QUIETLY and REQUIRED arguments and set DSSD_UTIL_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DSSD_UTIL DEFAULT_MSG DSSD_UTIL_LIBRARY DSSD_UTIL_INCLUDE_DIR)

if(DSSD_UTIL_FOUND)
  list(APPEND DSSD_UTIL_LIBRARIES ${DSSD_UTIL_LIBRARY} )
  list(APPEND DSSD_UTIL_INCLUDE_DIRS ${DSSD_UTIL_INCLUDE_DIR} )
endif()

mark_as_advanced( DSSD_UTIL_INCLUDE_DIR )
