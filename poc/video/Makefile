all: encode qt-decode

encode: encode.cc
	g++ encode.cc  -lavcodec -lavformat -lavutil -lavdevice -lswscale -o encode

qt-decode: qt_video.cc
	g++ qt_video.cc -DGUI_EXPORT="" -DCORE_EXPORT="" -DQT_SVG_LIB -DQT_GUI_LIB -DQT_XML_LIB -DQT_SQL_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -I/usr/include/qt4 -I/usr/include/qt4/QtSvg -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtXml -I/usr/include/qt4/QtSql -I/usr/include/qt4/QtNetwork -I/usr/include/qt4/QtCore -lQtGui -lavcodec -lavformat  -lboost_thread-mt -o qt-decode
