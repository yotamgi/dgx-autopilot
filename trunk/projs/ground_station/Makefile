LIBDS_PATH = ../libdata_stream/
include $(LIBDS_PATH)/Makefile.include

LIBDIR = ./lib
AUTOGENDIR = ./autogen
SRCDIR = ./src
MEDIADIR=`pwd`/data

QT_CXXFLAGS = -D_REENTRANT -DQT_WEBKIT -DQT_NO_DEBUG -DQT_XML_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -I/usr/share/qt4/mkspecs/linux-g++-64 -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtXml -I/usr/include/qt4 
QGIS_CXXFLAGS = -I/usr/include/qgis -I/usr/include/gdal -DGUI_EXPORT= -DCORE_EXPORT=
IRRLICHT_CXXFLAGS =  -I/usr/X11R6/include 
IRRLICHT_LDFLAGS =  -L/usr/X11R6/lib -lGL -lXxf86vm -lXext -lX11 -lIrrlicht 
QT_LDGLAGS = -L/usr/lib/x86_64-linux-gnu  -lQtXml -lQtGui -lQtCore -lpthread 
QGIS_LDFLAGS = -lqgis_core -lqgis_gui -lgeos -lgdal1.7.0

CXX = g++
MOC = moc-qt4
CXXFLAGS += -fPIC -g -Wall -pipe $(LIBDS_CXXFLAGS) $(QT_CXXFLAGS) $(QGIS_CXXFLAGS) $(IRRLICHT_CXXFLAGS) -O2 -DMEDIA_DIR=\"$(MEDIADIR)\"
LDFLAGS += -fPIC -L./lib -Wl,--rpath,$(LIBDS_LIBDIRS):$(LIBDIR) -lboost_thread -lboost_filesystem -lboost_system -lqwt $(IRRLICHT_LDFLAGS) $(LIBDS_LDFLAGS) $(QT_LDGLAGS) $(QGIS_LDFLAGS)

STREAMVIEW_OBJS =  $(SRCDIR)/map_stream_view.o 		\
				   $(SRCDIR)/3d_stream_view.o 		\
				   $(SRCDIR)/size_stream_view.o		\
				   $(SRCDIR)/size_push_gen.o		\
				   $(SRCDIR)/orientation_stream_view.o \
				   $(SRCDIR)/external_widgets/orientation_widget.o \
				   $(SRCDIR)/external_widgets/heading_widget.o 
				   

###############################################################################
## TARGETS 
###############################################################################


## dummy targets ##############################################################
.PHONY:
all: host device

.PHONY:
host: $(OBJDIR) $(LIBDIR) $(AUTOGENDIR) $(LIBDIR)/libground_station.so ground_station

.PHONY:
device: 

.PHONY:
clean:
	rm -fr $(AUTOPILIT_OBJS) */*/*.o */*.o *.o $(AUTOGENDIR) $(LIBDIR) ground_station

$(LIBDIR):
	mkdir $(LIBDIR)
$(AUTOGENDIR):
	mkdir $(AUTOGENDIR)

## real targets ###############################################################

.cc.o: Makefile
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(SRCDIR)/map_stream_view_moc.o: $(SRCDIR)/map_stream_view.h
	$(MOC) $(QT_CXXFLAGS)  $(SRCDIR)/map_stream_view.h -o $(AUTOGENDIR)/map_stream_view_moc.cxx -p$(SRCDIR)
	$(CXX) -c $(CXXFLAGS) $(AUTOGENDIR)/map_stream_view_moc.cxx  -o $*.o
$(SRCDIR)/3d_stream_view_moc.o: $(SRCDIR)/3d_stream_view.h
	$(MOC) $(QT_CXXFLAGS)  $(SRCDIR)/3d_stream_view.h -o $(AUTOGENDIR)/3d_stream_view_moc.cxx -p$(SRCDIR)
	$(CXX) -c $(CXXFLAGS) $(AUTOGENDIR)/3d_stream_view_moc.cxx  -o $*.o 
$(SRCDIR)/size_push_gen_moc.o: $(SRCDIR)/size_push_gen.h
	$(MOC) $(QT_CXXFLAGS)  $(SRCDIR)/size_push_gen.h -o $(AUTOGENDIR)/size_push_gen.cxx -p$(SRCDIR)
	$(CXX) -c $(CXXFLAGS) $(AUTOGENDIR)/size_push_gen.cxx  -o $*.o 
$(SRCDIR)/orientation_stream_view_moc.o: $(SRCDIR)/orientation_stream_view.h
	$(MOC) $(QT_CXXFLAGS)  $(SRCDIR)/orientation_stream_view.h -o $(AUTOGENDIR)/orientation_stream_view.cxx -p$(SRCDIR)
	$(CXX) -c $(CXXFLAGS) $(AUTOGENDIR)/orientation_stream_view.cxx  -o $*.o 

$(LIBDIR)/libground_station.so: $(STREAMVIEW_OBJS)  $(SRCDIR)/map_stream_view_moc.o $(SRCDIR)/3d_stream_view_moc.o $(SRCDIR)/size_push_gen_moc.o $(SRCDIR)/orientation_stream_view_moc.o
	$(CXX) $(LDFLAGS) -shared  $(STREAMVIEW_OBJS) $(SRCDIR)/map_stream_view_moc.o $(SRCDIR)/3d_stream_view_moc.o $(SRCDIR)/size_push_gen_moc.o $(SRCDIR)/orientation_stream_view_moc.o -o $(LIBDIR)/libground_station.so  

ground_station: $(LIBDIR)/libground_station.so $(SRCDIR)/main.o
	$(CXX) -L$(LIBDIR) $(SRCDIR)/main.o -lground_station $(LDFLAGS) -o ground_station
