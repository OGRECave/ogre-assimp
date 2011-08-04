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

