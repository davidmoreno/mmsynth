INCLUDE_DIRECTORIES( /usr/include/qt4/QtDesigner/ )

INCLUDE_DIRECTORIES( ../../common )

#SET( CMAKE_CXX_FLAGS "-Wall -W -fPIC -Werror" )
ADD_DEFINITIONS( -DQT_NO_DEBUG -DQT_PLUGIN -DQDESIGNER_EXPORT_WIDGETS -DQT_SHARED -D_REENTRANT )
#SET( CMAKE_CXX_FLAGS "-Wall -W -fPIC" )

QT4_WRAP_CPP( moc_mbutton.cpp mbutton.h )
QT4_WRAP_CPP( moc_mdial.cpp mdial.h )
QT4_WRAP_CPP( moc_mslider.cpp mslider.h )
QT4_WRAP_CPP( moc_msurface.cpp msurface.h )
QT4_WRAP_CPP( moc_mwidgetplugins.cpp mwidgetplugins.h )

ADD_LIBRARY( mwidgets SHARED mwidgetplugins moc_mwidgetplugins mbutton moc_mbutton mdial moc_mdial mslider moc_mslider msurface moc_msurface )

INSTALL( TARGETS mwidgets DESTINATION /usr/lib/qt4/plugins/designer/ )
INSTALL( TARGETS mwidgets DESTINATION lib )

ADD_LIBRARY( mwidgets_static STATIC muiloader moc_mwidgetplugins mbutton moc_mbutton mdial moc_mdial mslider moc_mslider msurface moc_msurface )
