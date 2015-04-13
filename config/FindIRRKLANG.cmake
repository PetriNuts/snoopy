#.rst:
# FindIRRKLANG
# --------
#
# Find IRRKLANG
#
# Find the IRRKLANG includes and library 
# This module defines
#
# ::
#
#   IRRKLANG_INCLUDE_DIR, where to find header.h, etc.
#   IRRKLANG_LIBRARIES, the libraries needed to use IRRKLANG.
#   IRRKLANG_FOUND, If false, do not try to use IRRKLANG.
#
# also defined, but not for general use are
#
# ::
#
#   IRRKLANG_LIBRARY, where to find the IRRKLANG library.

find_path(IRRKLANG_INCLUDE_DIR NAMES irrKlang.h
    PATH_SUFFIXES include
    )

find_library(IRRKLANG_LIBRARY 
    NAMES irrKlang libirrklang libIrrKlang
    )

# handle the QUIETLY and REQUIRED arguments and set IRRKLANG_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(IRRKLANG DEFAULT_MSG IRRKLANG_LIBRARY IRRKLANG_INCLUDE_DIR)

if(IRRKLANG_FOUND)
  list(APPEND IRRKLANG_LIBRARIES ${IRRKLANG_LIBRARY} )
  list(APPEND IRRKLANG_INCLUDE_DIRS ${IRRKLANG_INCLUDE_DIR} )
endif()

mark_as_advanced( IRRKLANG_INCLUDE_DIR )
