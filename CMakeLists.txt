cmake_minimum_required(VERSION 2.6)
project(snoopy)

#custom find 'modules'
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/config/")

#check compiler for C++11 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -stdlib=libc++")
    endif()
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#snoopy
if(NOT WIN32)
    #Generate position-independent code (PIC)
    add_definitions( -fPIC )
endif(NOT WIN32)
add_definitions( -D__SNOOPY__ )
add_definitions( -D_WCHAR_H_CPLUSPLUS_98_CONFORMANCE_ )
if(APPLE)
    add_definitions( -mmacosx-version-min=10.8 )
endif()
add_definitions( -Wno-deprecated )
add_definitions( -Wno-overloaded-virtual )
add_definitions( -Wno-new-returns-null )

#wxWidgets
find_package(wxWidgets REQUIRED base core aui adv net xml html)
include(${wxWidgets_USE_FILE})
list(APPEND LIBS ${wxWidgets_LIBRARIES})

#wxCode
find_package(wxCode REQUIRED ogl freechart)
include_directories(${wxCode_INCLUDE_DIRS})
list(APPEND LIBS ${wxCode_LIBRARIES})

#boost
# The following verifyies that BOOST_ROOT is set properly.
if(NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")
    FILE( TO_CMAKE_PATH $ENV{BOOST_ROOT} BOOST_ROOT )
    if( NOT EXISTS ${BOOST_ROOT} ) 
        MESSAGE( STATUS  ${BOOST_ROOT} " does not exist. Checking if BOOST_ROOT was a quoted string.." )
        STRING( REPLACE "\"" "" BOOST_ROOT ${BOOST_ROOT} ) 
        if( EXISTS ${BOOST_ROOT} ) 
            MESSAGE( STATUS "After removing the quotes " ${BOOST_ROOT} " was now found by CMake" )
        endif( EXISTS ${BOOST_ROOT})
    endif( NOT EXISTS ${BOOST_ROOT} )
# Save the BOOST_ROOT in the cache
    if( NOT EXISTS ${BOOST_ROOT} ) 
        MESSAGE( WARNING ${BOOST_ROOT} " does not exist." )
    else(NOT EXISTS ${BOOST_ROOT})
        SET (BOOST_ROOT ${BOOST_ROOT} CACHE STRING "Set the value of BOOST_ROOT to point to the root folder of your boost install." FORCE)
    endif( NOT EXISTS ${BOOST_ROOT} )
endif(NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")

if( WIN32 AND NOT BOOST_ROOT )
    MESSAGE( WARNING "Please set the BOOST_ROOT environment variable." )
endif( WIN32 AND NOT BOOST_ROOT )

find_package(Boost REQUIRED regex)
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND LIBS ${Boost_LIBRARIES})

#OGDF
find_package(OGDF REQUIRED)
include_directories(${OGDF_INCLUDE_DIRS})
list(APPEND LIBS ${OGDF_LIBRARIES})

#SBML
find_package(LibSBML REQUIRED)
include_directories(${LibSBML_INCLUDE_DIRS})
list(APPEND LIBS ${LibSBML_LIBRARIES})

#cvode
find_package(SUNDIALS REQUIRED cvode nvecserial)
include_directories(${SUNDIALS_INCLUDE_DIRS})
list(APPEND LIBS ${SUNDIALS_LIBRARIES})

#gecode
find_package(GECODE REQUIRED driver int float kernel minimodel search set support)
include_directories(${GECODE_INCLUDE_DIRS})
list(APPEND LIBS ${GECODE_LIBRARIES})

#irrklang
find_package(IRRKLANG REQUIRED)
include_directories(${IRRKLANG_INCLUDE_DIRS})
list(APPEND LIBS ${IRRKLANG_LIBRARIES})

#gmplib
find_package(GMP)
if(GMP_FOUND)
    add_definitions( -DUSE_GMP )
    include_directories(GMP_INCLUDE_DIR)
    list(APPEND LIBS ${GMP_LIBRARIES})
    #SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-no_pie" )
endif(GMP_FOUND)

#dssz_util
find_package(DSSZ_UTIL REQUIRED)
include_directories(${DSSZ_UTIL_INCLUDE_DIRS})
list(APPEND LIBS ${DSSZ_UTIL_LIBRARIES})

#SPSA_STEERINGAPI
find_package(SPSA_SteeringAPI REQUIRED)
include_directories(${SPSA_SteeringAPI_INCLUDE_DIRS})
list(APPEND LIBS ${SPSA_SteeringAPI_LIBRARIES})

#SP_SIMULATOR
find_package(Snoopy_Simulator REQUIRED)
include_directories(${Snoopy_Simulator_INCLUDE_DIRS})
list(APPEND LIBS ${Snoopy_Simulator_LIBRARIES})


#set(BIN ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(INC ${CMAKE_CURRENT_SOURCE_DIR})
#set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(SRC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${INC})

#get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
#message("inc_dirs = ${inc_dirs}")
#message("libs = ${LIBS}")

# search source files
file(GLOB CPP_FILES ${SRC}/*.cpp)
set(SOURCE_FILES ${CPP_FILES})
set(SRC_DIRS sp_core sp_ds sp_gr sp_gui export import)
foreach(d ${SRC_DIRS})
  file(GLOB_RECURSE CPP_FILES ${SRC}/${d}/*.cpp)
  file(GLOB_RECURSE CC_FILES ${SRC}/${d}/*.cc)
  list(APPEND SOURCE_FILES ${CPP_FILES})
  list(APPEND SOURCE_FILES ${CC_FILES})
endforeach()

add_executable(snoopy ${SOURCE_FILES})
target_link_libraries(snoopy ${LIBS})
