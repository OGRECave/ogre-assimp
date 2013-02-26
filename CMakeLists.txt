# /*
# -----------------------------------------------------------------------------
# This source file is part of
#                                     _
#   ___   __ _ _ __ ___  __ _ ___ ___(_)_ __ ___  _ __
#  / _ \ / _` | '__/ _ \/ _` / __/ __| | '_ ` _ \| '_ \
# | (_) | (_| | | |  __/ (_| \__ \__ \ | | | | | | |_) |
#  \___/ \__, |_|  \___|\__,_|___/___/_|_| |_| |_| .__/
#        |___/                                   |_|
#
# For the latest info, see http://code.google.com/p/ogreassimp/
#
# Copyright (c) 2011 Jacob 'jacmoe' Moen
#
# Licensed under the MIT license:
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# -----------------------------------------------------------------------------
#*/
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

project(ogreassimp)

set(OGREASSIMP_VERSION_STRING "0.1.1")
set(OGREASSIMP_VERSION_STRING_MAJOR "0")
set(OGREASSIMP_VERSION_STRING_MINOR "1")
set(OGREASSIMP_VERSION_STRING_PATCH "1")

if(WIN32)
    set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif(WIN32)

if(UNIX)
    if(EXISTS "/usr/local/lib/OGRE/cmake")

      set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")

    elseif(EXISTS "/usr/lib/OGRE/cmake")

      set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")

    else ()
      message(SEND_ERROR "Failed to find module path.")
    endif()
endif(UNIX)

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

find_package(OGRE REQUIRED)

if("${OGRE_VERSION_NAME}" STREQUAL "Byatis")
    add_definitions(-DUSING_BYATIS)
endif()


# Find Boost
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
    if (WIN32 OR APPLE)
        set(Boost_USE_STATIC_LIBS TRUE)
    else ()
        # Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
        set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
    endif ()
    if (MINGW)
        # this is probably a bug in CMake: the boost find module tries to look for
        # boost libraries with name libboost_*, but CMake already prefixes library
        # search names with "lib". This is the workaround.
        set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
    endif ()
    set(Boost_ADDITIONAL_VERSIONS "1.54" "1.54.0" "1.53" "1.53.0" "1.52" "1.52.0" "1.51" "1.51.0" "1.50" "1.50.0" "1.49" "1.49.0" "1.48" "1.48.0" "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" )
    # Components that need linking (NB does not include header-only components like bind)
    set(OGRE_BOOST_COMPONENTS thread date_time)
    find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
    if (NOT Boost_FOUND)
        # Try again with the other type of libs
        set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
        find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
    endif()
	if (Boost_FOUND AND WIN32 AND Boost_USE_STATIC_LIBS)
		if(Boost_VERSION GREATER 104700)
			set(OGRE_BOOST_COMPONENTS thread date_time system)
		endif()
		if(Boost_VERSION GREATER 104900)
			set(OGRE_BOOST_COMPONENTS thread date_time system chrono)
		endif()
		find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	endif()
    find_package(Boost QUIET)

    # Set up referencing of Boost
    include_directories(${Boost_INCLUDE_DIR})
    add_definitions(-DBOOST_ALL_NO_LIB)
    set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
endif()

OPTION(FIX_NEED_ZLIB "Switch on if your configuration requires zlib" OFF)

if(FIX_NEED_ZLIB)
	find_package(ZLIB REQUIRED)
	set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${ZLIB_LIBRARIES})
endif()

find_package(ASSIMP REQUIRED)

add_subdirectory(src)

add_subdirectory(tool)

