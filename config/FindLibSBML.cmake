#.rst:
# FindLibSBML
# -----------
#
# Try to find the LibSBML library
#
# Once done this will define
#
# ::
#
#   LibSBML_FOUND - System has LibSBML
#   LibSBML_INCLUDE_DIR - The LibSBML include directory
#   LibSBML_LIBRARIES - The libraries needed to use LibSBML
#   LibSBML_VERSION_STRING - the version of LibSBML found (since CMake 2.8.8)

# use pkg-config to get the directories and then use these values
# in the find_path() and find_library() calls
find_package(PkgConfig QUIET)
PKG_CHECK_MODULES(PC_LibSBML QUIET libsbml)
set(LibSBML_DEFINITIONS ${PC_LibSBML_CFLAGS_OTHER})

find_path(LibSBML_INCLUDE_DIR NAMES sbml/SBMLTypes.h
   HINTS
   ${PC_LibSBML_INCLUDEDIR}
   ${PC_LibSBML_INCLUDE_DIRS}
   PATH_SUFFIXES libsbml
   )

find_library(LibSBML_LIBRARY NAMES sbml-static sbml libsbml-static libsbml
   HINTS
   ${PC_LibSBML_LIBDIR}
   ${PC_LibSBML_LIBRARY_DIRS}
   )

if(PC_LibSBML_VERSION)
    set(LibSBML_VERSION_STRING ${PC_LibSBML_VERSION})
elseif(LibSBML_INCLUDE_DIR AND EXISTS "${LibSBML_INCLUDE_DIR}/sbml/common/libsbml-version.h")
    file(STRINGS "${LibSBML_INCLUDE_DIR}/sbml/common/libsbml-version.h" libxml2_version_str
         REGEX "^#define[\t ]+LIBSBML_DOTTED_VERSION[\t ]+\".*\"")

    string(REGEX REPLACE "^#define[\t ]+LIBSBML_DOTTED_VERSION[\t ]+\"([^\"]*)\".*" "\\1"
           LibSBML_VERSION_STRING "${libxml2_version_str}")
    unset(libxml2_version_str)
endif()

# handle the QUIETLY and REQUIRED arguments and set LibSBML_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibSBML FOUND_VAR LibSBML_FOUND
                                  REQUIRED_VARS LibSBML_LIBRARY LibSBML_INCLUDE_DIR
                                  VERSION_VAR LibSBML_VERSION_STRING)

if(LibSBML_FOUND)
  list(APPEND LibSBML_LIBRARIES ${LibSBML_LIBRARY} )
  list(APPEND LibSBML_INCLUDE_DIRS ${LibSBML_INCLUDE_DIR} )
endif()

mark_as_advanced(LibSBML_INCLUDE_DIR)
