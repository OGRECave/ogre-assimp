#/******************************************************************************
# * CMakeLists.txt - main llcoi CMake script
# ******************************************************************************
# * This file is part of
# *     __ __              _
# *    / // /_____ ____   (_)
# *   / // // ___// __ \ / /
# *  / // // /__ / /_/ // /
# * /_//_/ \___/ \____//_/
# *
# * Low Level C Ogre Interface (llcoi)
# *
# * See http://code.google.com/p/llcoi/ for more information.
# *
# * Copyright (c) 2011, Llcoi Team
# *
# * License: MIT
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy
# * of this software and associated documentation files (the "Software"), to deal
# * in the Software without restriction, including without limitation the rights
# * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# * copies of the Software, and to permit persons to whom the Software is
# * furnished to do so, subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in
# * all copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# * THE SOFTWARE.
# ******************************************************************************/
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

project(OgreAssimpConverter)

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
    endif(EXISTS "/usr/local/lib/OGRE")
endif(UNIX)

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

find_package(OGRE REQUIRED)

if(NOT "${OGRE_VERSION_NAME}" STREQUAL "Byatis")
  message(SEND_ERROR "You need Ogre 1.8 Byatis to build this.")
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
    set(Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" )
    # Components that need linking (NB does not include header-only components like bind)
    set(OGRE_BOOST_COMPONENTS thread date_time)
    find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
    if (NOT Boost_FOUND)
        # Try again with the other type of libs
        set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
        find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
    endif()
    find_package(Boost QUIET)

    # Set up referencing of Boost
    include_directories(${Boost_INCLUDE_DIR})
    add_definitions(-DBOOST_ALL_NO_LIB)
    set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
endif()

find_package(ASSIMP REQUIRED)

set(HDRS
    ./include/AssimpLoader.h
)

set(SRCS
    ./src/AssimpLoader.cpp
    ./src/main.cpp
)

include_directories( ${OIS_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    "./include"
)

add_executable(OgreAssimpConverter ${HDRS} ${SRCS})

set_target_properties(OgreAssimpConverter PROPERTIES DEBUG_POSTFIX _d)

target_link_libraries(OgreAssimpConverter ${OGRE_LIBRARIES} ${ASSIMP_LIBRARIES})

if(UNIX)
    install(TARGETS OgreAssimpConverter
        RUNTIME DESTINATION bin
        CONFIGURATIONS Release RelWithDebInfo Debug)
endif(UNIX)