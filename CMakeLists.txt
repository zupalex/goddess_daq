cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (goddess_daq)

#Specify the install prefix as the exec directory unless specified
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	message(STATUS "Install Prefix not specified.")
	file(MAKE_DIRECTORY exec)
	get_filename_component(INSTALL_DIR ${CMAKE_SOURCE_DIR}/exec REALPATH)
	set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR} CACHE PATH
			"Install Prefix" FORCE)
endif ()
message(STATUS "Installing to ${CMAKE_INSTALL_PREFIX}")

#Define the default build type
IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Release CACHE STRING
		"Build type, options are: None Debug Release
			RelWithDebInfo MinSizeRel."
		FORCE)
	message(STATUS "Build type not defined, using default.")
ENDIF(NOT CMAKE_BUILD_TYPE)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wno-unused-but-set-variable -pedantic")
endif()

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

#Add custom modules to module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

include(${CMAKE_MODULE_PATH}/ExportDYLD.cmake)

CheckForExportDYLD()

include(${CMAKE_MODULE_PATH}/DealWithUserStuffs.cmake)

DealWithUserStuffs()

#Find the ROOT package and configure the flags and paths
find_package (ROOT REQUIRED)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS} -fPIC")
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

#Find the zlib fro compression
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

add_definitions(-DLINUX -D_FILE_OFFSET_BITS=64)

add_subdirectory(hribf)

#Include our headers and continue in the source directory
include_directories(include)
include_directories(goddess/include)
include_directories(scripts/include)
add_subdirectory(goddess/source)
add_subdirectory(source)
add_subdirectory(scripts)
install(DIRECTORY chatfiles DESTINATION ${CMAKE_INSTALL_PREFIX})

install(FILES share/crmat.LINUX share/AGATA_crmat.dat share/dgscal.dat share/map.dat share/.rootlogon.C DESTINATION ${CMAKE_INSTALL_PREFIX})

file(GLOB GODDESS_CONF_FILES "share/*.config")
install(FILES ${GODDESS_CONF_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX})
