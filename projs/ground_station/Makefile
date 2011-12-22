LIBDS_PATH = ../libdata_stream/
include $(LIBDS_PATH)/Makefile.include

LIBDIR = ./lib
SRCDIR = ./src

QT_CXXFLAGS = -pipe -D_REENTRANT -DQT_WEBKIT -DQT_NO_DEBUG -DQT_XML_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -I/usr/share/qt4/mkspecs/linux-g++-64 -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtXml -I/usr/include/qt4 
QGIS_CXXFLAGS = -DGUI_EXPORT= -DCORE_EXPORT=
QT_LDGLAGS = -L/usr/lib/x86_64-linux-gnu  -lQtXml -lQtGui -lQtCore -lpthread 
QGIS_LDFLAGS = -lqgis_core -lqgis_gui -lgeos -lgdal1.7.0

CXX = g++
CXXFLAGS += -fPIC -g -Wall $(LIBDS_CXXFLAGS) $(QT_CXXFLAGS) $(QGIS_CXXFLAGS) -O3 
LDFLAGS += -fPIC -L./lib -Wl,--rpath,$(LIBDS_LIBDIRS):$(LIBDIR) $(LIBDS_LDFLAGS) $(QT_LDGLAGS) $(QGIS_LDFLAGS) -lboost_thread

STREAMVIEW_OBJS =  $(SRCDIR)/map_stream_view.o

###############################################################################
## TARGETS 
###############################################################################


## dummy targets ##############################################################
.PHONY:
all: host device

.PHONY:
host: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libstreamview.so ground_station

.PHONY:
device: 

.PHONY:
clean:
	rm -fr $(AUTOPILIT_OBJS) */*/*.o */*.o *.o $(LIBDIR) gtest sensor_watch_test watch_test cockpit_test waypoint_pilot_test

$(LIBDIR):
	mkdir $(LIBDIR)

## real targets ###############################################################

.cc.o: Makefile
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(LIBDIR)/libstreamview.so: $(STREAMVIEW_OBJS)
	$(CXX) $(LDFLAGS) -shared  $(STREAMVIEW_OBJS) -o $(LIBDIR)/libstreamview.so 

ground_station: $(LIBDIR)/libstreamview.so $(SRCDIR)/main.o
	$(CXX) -L$(LIBDIR) $(SRCDIR)/main.o -lstreamview $(LDFLAGS) -o ground_station
