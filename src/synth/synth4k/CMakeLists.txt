include_directories( .. )
include_directories( ../../common )

ADD_LIBRARY( synth4k SHARED synth4k )

install( TARGETS synth4k DESTINATION lib/mmsynth/synth/ )
