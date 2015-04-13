#.rst:
# FindOGDF
# --------
#
# Find OGDF
#
# Find the OGDF includes and library 
# This module defines
#
# ::
#
#   OGDF_INCLUDE_DIR, where to find header.h, etc.
#   OGDF_LIBRARIES, the libraries needed to use OGDF.
#   OGDF_FOUND, If false, do not try to use OGDF.
#
# also defined, but not for general use are
#
# ::
#
#   OGDF_LIBRARY, where to find the OGDF library.

find_path(OGDF_INCLUDE_DIR NAMES ogdf/basic/Graph.h
    PATH_SUFFIXES ogdf
    )

find_library(OGDF_LIBRARY 
    NAMES OGDF libOGDF
    )

# handle the QUIETLY and REQUIRED arguments and set OGDF_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OGDF REQUIRED_VARS OGDF_LIBRARY OGDF_INCLUDE_DIR)

if(OGDF_FOUND)
  list(APPEND OGDF_LIBRARIES ${OGDF_LIBRARY} )
  list(APPEND OGDF_INCLUDE_DIRS ${OGDF_INCLUDE_DIR} )
endif()

mark_as_advanced( OGDF_INCLUDE_DIR )
