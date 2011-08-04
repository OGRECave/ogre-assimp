project(OgreAssimpLoader)

set(HDRS
    ./AssimpLoader.h
)

set(SRCS
    ./AssimpLoader.cpp
)

include_directories( ${OIS_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

add_library(OgreAssimpLoader STATIC ${HDRS} ${SRCS})
set_target_properties(OgreAssimpLoader PROPERTIES DEBUG_POSTFIX _d)

target_link_libraries(OgreAssimpLoader ${OGRE_LIBRARIES} ${ASSIMP_LIBRARIES})

if(UNIX)
  set_target_properties( OgreAssimpLoader PROPERTIES OUTPUT_NAME "OgreAssimpLoader" VERSION
${OGREASSIMP_VERSION_STRING} SOVERSION ${abi_version} )
endif()

if(UNIX)
    install(TARGETS OgreAssimpLoader
        DESTINATION lib
        CONFIGURATIONS Release RelWithDebInfo Debug)

    install(FILES
        ${HDRS}
        DESTINATION include/OgreAssimpLoader)
endif(UNIX)

