# REQUIRES IRRLICHT

LIBDS_PATH = ../libdata_stream
include $(LIBDS_PATH)/Makefile.include

AUTOPILOT_PATH = ../autopilot
include $(AUTOPILOT_PATH)/Makefile.include


CXX = g++
CXXFLAGS +=  -O3 -I/usr/X11R6/include  -g $(LIBDS_CXXFLAGS) $(AUTOPILOT_CXXFLAGS) -I. -fPIC
LDFLAGS += -L/usr/X11R6/lib -lGL -lXxf86vm -lXext -lX11 -lIrrlicht $(LIBDS_LDFLAGS) -lboost_thread -Wl,-rpath,$(LIBDS_LIBDIRS)

OBJDIR = ./obj
LIBDIR = ./lib


.PHONY: all clean

all: host device
host: $(OBJDIR) sim
device:
clean:
	rm -rf $(OBJDIR) $(LIBDIR) sim


$(LIBDIR):
	mkdir $(LIBDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/main.o: main.cc 
	$(CXX) -c main.cc $(CXXFLAGS) -o $(OBJDIR)/main.o

$(OBJDIR)/plain.o: plain.cc 
	$(CXX) -c plain.cc $(CXXFLAGS) -o $(OBJDIR)/plain.o

$(OBJDIR)/camera.o: camera.cc
	$(CXX) -c camera.cc $(CXXFLAGS) -o $(OBJDIR)/camera.o
	
$(OBJDIR)/simulator_platform.o: simulator_platform.cc
	$(CXX) -c simulator_platform.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_platform.o

$(OBJDIR)/simulator.o: simulator.cc
	$(CXX) -c simulator.cc $(CXXFLAGS) -o $(OBJDIR)/simulator.o
	
$(OBJDIR)/simulator_gyro.o: sensors/simulator_gyro.cc
	$(CXX) -c sensors/simulator_gyro.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_gyro.o

$(OBJDIR)/simulator_accelerometer.o: sensors/simulator_accelerometer.cc
	$(CXX) -c sensors/simulator_accelerometer.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_accelerometer.o

$(OBJDIR)/simulator_magnetometer.o: sensors/simulator_magnetometer.cc
	$(CXX) -c sensors/simulator_magnetometer.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_magnetometer.o

$(OBJDIR)/simulator_gps.o: sensors/simulator_gps.cc
	$(CXX) -c sensors/simulator_gps.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_gps.o		

$(OBJDIR)/simulator_altmeter.o: sensors/simulator_altmeter.cc
	$(CXX) -c sensors/simulator_altmeter.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_altmeter.o		

$(LIBDIR)/libdgx_simulator.so: $(LIBDIR) $(OBJDIR)/plain.o $(OBJDIR)/camera.o $(OBJDIR)/simulator_platform.o $(OBJDIR)/simulator.o  $(OBJDIR)/simulator_gyro.o $(OBJDIR)/simulator_accelerometer.o $(OBJDIR)/simulator_magnetometer.o $(OBJDIR)/simulator_gps.o $(OBJDIR)/simulator_altmeter.o 
	$(CXX) -shared -fPIC $(OBJDIR)/plain.o $(OBJDIR)/camera.o $(OBJDIR)/simulator_platform.o  $(OBJDIR)/simulator.o $(OBJDIR)/simulator_gyro.o $(OBJDIR)/simulator_accelerometer.o $(OBJDIR)/simulator_magnetometer.o $(OBJDIR)/simulator_gps.o $(OBJDIR)/simulator_altmeter.o $(LDFLAGS) -o $(LIBDIR)/libdgx_simulator.so

sim: $(OBJDIR)/main.o $(LIBDIR)/libdgx_simulator.so
	$(CXX) $(OBJDIR)/main.o -L$(LIBDIR) $(LDFLAGS),$(LIBDIR)/libdgx_simulator.so -ldgx_simulator -o sim 


