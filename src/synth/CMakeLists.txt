include_directories( ../common )

QT4_WRAP_CPP( moc_synth.cpp synth.h )

ADD_LIBRARY( synth STATIC synth moc_synth midievent )

add_subdirectory(modules)

add_subdirectory(synth4k)
add_subdirectory(polybase)
add_subdirectory(mono1)

