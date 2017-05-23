cmake_minimum_required(VERSION 2.8.5)

project(snoopy)

#custom find 'modules'
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/config/")

#default build type for single configuration targets
if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
            FORCE )
endif()

add_definitions( -D__SOURCE_DIR__="${CMAKE_CURRENT_SOURCE_DIR}" )
add_definitions( -D_USE_MATH_DEFINES )

#check compiler for C++11 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -stdlib=libc++")
    endif()
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#snoopy
#Optimization flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=core2 -mtune=core2 -funroll-loops" )
if(NOT WIN32)
    #Generate position-independent code (PIC)
    add_definitions( -fPIC )
endif(NOT WIN32)
add_definitions( -D__SNOOPY__ )
add_definitions( -D_WCHAR_H_CPLUSPLUS_98_CONFORMANCE_ )
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions( -mmacosx-version-min=10.9 )
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
if( WIN32 )
    if( NOT "$ENV{BOOST_ROOT}" STREQUAL "" )
        set(ENV{BOOST_LIBRARYDIR} $ENV{BOOST_ROOT}/stage/lib)
    else()
        MESSAGE( WARNING "Please set the BOOST_ROOT environment variable." )
    endif( NOT "$ENV{BOOST_ROOT}" STREQUAL "" )
endif( WIN32 )

set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME ON)

find_package(Boost REQUIRED regex)
if(Boost_FOUND)
    add_definitions( -DUSE_BOOST )
    include_directories(${Boost_INCLUDE_DIRS})
    list(APPEND LIBS ${Boost_LIBRARIES})
endif()

#OGDF
find_package(OGDF REQUIRED)
include_directories(${OGDF_INCLUDE_DIRS})
list(APPEND LIBS ${OGDF_LIBRARIES})

#SBML
find_package(LibSBML REQUIRED)
include_directories(${LibSBML_INCLUDE_DIRS})
list(APPEND LIBS ${LibSBML_LIBRARIES})
#XML2 used by SBML
if(NOT WIN32)
    find_package(LibXml2 REQUIRED)
    list(APPEND LIBS ${LIBXML2_LIBRARIES})
    find_package(ZLIB)
    list(APPEND LIBS ${ZLIB_LIBRARIES})
    find_package(BZip2)
    list(APPEND LIBS ${BZIP2_LIBRARIES})
endif()

#cvode
find_package(SUNDIALS REQUIRED cvode nvecserial)
include_directories(${SUNDIALS_INCLUDE_DIRS})
list(APPEND LIBS ${SUNDIALS_LIBRARIES})

#irrklang
find_package(IRRKLANG REQUIRED)
include_directories(${IRRKLANG_INCLUDE_DIRS})
list(APPEND LIBS ${IRRKLANG_LIBRARIES})

#dssz_util
find_package(DSSZ_UTIL REQUIRED)
include_directories(${DSSZ_UTIL_INCLUDE_DIRS})
list(APPEND LIBS ${DSSZ_UTIL_LIBRARIES})

#gecode
find_package(GECODE REQUIRED flatzinc driver search minimodel set int float kernel support)
include_directories(${GECODE_INCLUDE_DIRS})
list(APPEND LIBS ${GECODE_LIBRARIES})

#gmplib
find_package(GMP)
if(GMP_FOUND)
    add_definitions( -DUSE_GMP )
    include_directories(GMP_INCLUDE_DIR)
    list(APPEND LIBS ${GMP_LIBRARIES})
    #SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-no_pie" )
endif(GMP_FOUND)

#SPSA_STEERINGAPI
find_package(SPSA_SteeringAPI REQUIRED)
include_directories(${SPSA_SteeringAPI_INCLUDE_DIRS})
list(APPEND LIBS ${SPSA_SteeringAPI_LIBRARIES})

#SP_SIMULATOR
find_package(Snoopy_Simulator REQUIRED)
include_directories(${Snoopy_Simulator_INCLUDE_DIRS})
list(APPEND LIBS ${Snoopy_Simulator_LIBRARIES})


set(BIN ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
set(INC ${CMAKE_CURRENT_SOURCE_DIR})
#set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(SRC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${INC})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN})

#get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
#message("inc_dirs = ${inc_dirs}")
#message("libs = ${LIBS}")

# search source files
file(GLOB H_FILES ${SRC}/*.h)
set(HEADER_FILES ${H_FILES})
file(GLOB CPP_FILES ${SRC}/*.cpp)
set(SOURCE_FILES ${CPP_FILES})
set(SRC_DIRS sp_core sp_ds sp_gr sp_gui export import)
foreach(d ${SRC_DIRS})
  file(GLOB_RECURSE HPP_FILES ${SRC}/${d}/*.hpp)
  file(GLOB_RECURSE H_FILES ${SRC}/${d}/*.h)
  list(APPEND HEADER_FILES ${HPP_FILES})
  list(APPEND HEADER_FILES ${H_FILES})
  file(GLOB_RECURSE CPP_FILES ${SRC}/${d}/*.cpp)
  file(GLOB_RECURSE CC_FILES ${SRC}/${d}/*.cc)
  list(APPEND SOURCE_FILES ${CPP_FILES})
  list(APPEND SOURCE_FILES ${CC_FILES})
endforeach()

source_group("Header Files" FILES ${HEADER_FILES})
source_group("Source Files" FILES ${SOURCE_FILES})

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBS})

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy sp_build.cpp.in sp_build.cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ../mkAppBundleOSX.sh ${CMAKE_PROJECT_NAME} beta
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
endif()