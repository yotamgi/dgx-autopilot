UIC_PROG = uic-qt4
MOC_PROG = moc-qt4
RCC_PROG = rcc
CXX = g++

OBJDIR = ./obj
DATDIR = ./data

QT_CXXFLAGS = -DGUI_EXPORT="" -DCORE_EXPORT="" -DQT_SVG_LIB -DQT_GUI_LIB -DQT_XML_LIB -DQT_SQL_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -I/usr/include/qt4 -I/usr/include/qt4/QtSvg -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtXml -I/usr/include/qt4/QtSql -I/usr/include/qt4/QtNetwork -I/usr/include/qt4/QtCore 
QGIS_CXXFLAGS = -I/usr/include/gdal  -I/usr/include/qgis
QT_LIBS = -lQtSvg -lQtGui -lQtXml -lQtSql -lQtNetwork -lQtCore -lQtSql -lQtCore 
QGIS_LIBS = -lqgis_core -lqgis_gui -lgeos -lgdal1.7.0 

CXXFLAGS = $(QT_CXXFLAGS) $(QGIS_CXXFLAGS)
LD_FLAGS = -rdynamic $(QGIS_LIBS) $(QT_LIBS) 

#
# TARGETS -----------------------------
#
all: $(OBJDIR) testprog 
clean:
	rm -rf obj testprog ui_mainwindowbase.h *.cxx

$(OBJDIR):
	mkdir $(OBJDIR)

ui_mainwindowbase.h:
	$(UIC_PROG) -o ui_mainwindowbase.h $(DATDIR)/mainwindowbase.ui

moc_mainwindow.cxx:
	$(MOC_PROG) $(QT_CXXFLAGS) mainwindow.h -o moc_mainwindow.cxx 

qrc_resources.cxx:
	$(RCC_PROG) -name resources -o qrc_resources.cxx $(DATDIR)/resources.qrc

$(OBJDIR)/main.o: main.cc ui_mainwindowbase.h
	$(CXX) -c $(CXXFLAGS) main.cc -o $(OBJDIR)/main.o

$(OBJDIR)/mainwindow.o: mainwindow.cc ui_mainwindowbase.h
	$(CXX) -c $(CXXFLAGS) mainwindow.cc -o $(OBJDIR)/mainwindow.o

$(OBJDIR)/moc_mainwindow.o: moc_mainwindow.cxx
	$(CXX) -c $(CXXFLAGS) moc_mainwindow.cxx -o $(OBJDIR)/moc_mainwindow.o

$(OBJDIR)/qrc_resources.o: qrc_resources.cxx
	$(CXX) -c $(CXXFLAGS) qrc_resources.cxx -o $(OBJDIR)/qrc_resources.o

testprog: $(OBJDIR)/main.o  $(OBJDIR)/mainwindow.o $(OBJDIR)/moc_mainwindow.o $(OBJDIR)/qrc_resources.o
	$(CXX) $(OBJDIR)/main.o  $(OBJDIR)/mainwindow.o $(OBJDIR)/moc_mainwindow.o $(OBJDIR)/qrc_resources.o $(LD_FLAGS) -o testprog
