PROJECT(mmsynth)

ADD_SUBDIRECTORY( widgets )
include_directories( ../common )
include_directories( ../synth )
include_directories( . )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DINSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}/lib/mmsynth/\" ) 

QT4_WRAP_CPP( moc_mainwindow.cpp mainwindow.h )
QT4_WRAP_CPP( moc_midi.cpp       midi.h       )
QT4_WRAP_CPP( moc_alsamidi.cpp   alsamidi.h   )
QT4_WRAP_CPP( moc_audio.cpp      audio.h      )
QT4_WRAP_CPP( moc_alsaaudio.cpp  alsaaudio.h  )

QT4_WRAP_UI( ui_mainwindow.h mainwindow.ui )

QT4_ADD_RESOURCES( mmsynth_RESOURCES ../uis/uis.qrc )

INCLUDE_DIRECTORIES( . )
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE( mmsynth main mainwindow moc_mainwindow midi alsamidi moc_alsamidi 
											  moc_midi audio moc_audio alsaaudio moc_alsaaudio jackaudio ui_mainwindow.h 
												${mmsynth_RESOURCES} )

TARGET_LINK_LIBRARIES( mmsynth asound QtUiTools synth jack mwidgets_static )

install(TARGETS mmsynth DESTINATION bin )
