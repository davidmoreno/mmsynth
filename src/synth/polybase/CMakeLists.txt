include_directories( .. )
include_directories( ../../common )

ADD_LIBRARY( polybase SHARED polybase )

install( TARGETS polybase DESTINATION lib/mmsynth/synth/ )
