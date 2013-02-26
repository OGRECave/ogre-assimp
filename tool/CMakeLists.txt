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
# For the latest info, see https://bitbucket.org/jacmoe/ogreassimp
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
project(OgreAssimpConverter)

set(HDRS
    ../src/AssimpLoader.h
)

set(SRCS
    ../src/AssimpLoader.cpp
    ./main.cpp
)

include_directories( ${OIS_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    "../src"
)

add_executable(OgreAssimpConverter ${HDRS} ${SRCS})

if(WIN32)
    set_target_properties(OgreAssimpConverter PROPERTIES DEBUG_POSTFIX _d)
endif(WIN32)

target_link_libraries(OgreAssimpConverter ${OGRE_LIBRARIES} ${ASSIMP_LIBRARIES})

if(UNIX)
    install(TARGETS OgreAssimpConverter
        RUNTIME DESTINATION bin
        CONFIGURATIONS Release RelWithDebInfo Debug)
endif(UNIX)

if(WIN32)
    install(TARGETS OgreAssimpConverter
        RUNTIME DESTINATION $ENV{OGRE_HOME}/bin/release
        CONFIGURATIONS Release RelWithDebInfo)
    install(TARGETS OgreAssimpConverter
        RUNTIME DESTINATION $ENV{OGRE_HOME}/bin/debug
        CONFIGURATIONS Debug)
endif(WIN32)
