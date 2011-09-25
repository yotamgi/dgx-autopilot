# REQUIRES IRRLICHT

LIBDS_PATH = ../libdata_stream
include $(LIBDS_PATH)/Makefile.include

CXX = g++
CXXFLAGS +=  -O3 -I/usr/X11R6/include  -g $(LIBDS_CXXFLAGS)
LDFLAGS += -L/usr/X11R6/lib -lGL -lXxf86vm -lXext -lX11 -lIrrlicht $(LIBDS_LDFLAGS) -lboost_thread -Wl,-rpath,$(LIBDS_LIBDIRS)

OBJDIR = ./obj


.PHONY: all clean

all: host device
host: $(OBJDIR) sim
device:
clean:
	rm -rf obj sim


$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/main.o: main.cc 
	$(CXX) -c main.cc $(CXXFLAGS) -o $(OBJDIR)/main.o

$(OBJDIR)/plane.o: plane.cc 
	$(CXX) -c plane.cc $(CXXFLAGS) -o $(OBJDIR)/plane.o

$(OBJDIR)/camera.o: camera.cc
	$(CXX) -c camera.cc $(CXXFLAGS) -o $(OBJDIR)/camera.o

sim: $(OBJDIR)/main.o $(OBJDIR)/plane.o $(OBJDIR)/camera.o
	$(CXX) $(OBJDIR)/main.o $(OBJDIR)/plane.o $(OBJDIR)/camera.o $(LDFLAGS) -o sim


