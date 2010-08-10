FIND_PACKAGE(Qt4 REQUIRED)

SET( CMAKE_CXX_FLAGS "-Wall -W -fPIC" )

INCLUDE( ${QT_USE_FILE} )
LINK_LIBRARIES( ${QT_LIBRARIES} )

add_subdirectory(synth)
add_subdirectory(gui)
add_subdirectory(uis)
