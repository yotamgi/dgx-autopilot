# REQUIRES IRRLICHT

LIBDS_PATH = ../libdata_stream
include $(LIBDS_PATH)/Makefile.include

AUTOPILOT_PATH = ../autopilot
include $(AUTOPILOT_PATH)/Makefile.include

OBJDIR = ./obj
LIBDIR = ./lib
SRCDIR = ./src

MEDIA_DIR=`pwd`/media

CXX = g++
CXXFLAGS +=  -O3 -I/usr/X11R6/include  -g $(LIBDS_CXXFLAGS) $(AUTOPILOT_CXXFLAGS) -I$(SRCDIR) -fPIC -DMEDIA_DIR=\"$(MEDIA_DIR)\"
LDFLAGS += -L/usr/X11R6/lib -lGL -lXxf86vm -lXext -lX11 -lIrrlicht $(LIBDS_LDFLAGS) $(AUTOPILOT_LDFLAGS) -lboost_thread -Wl,-rpath,$(LIBDS_LIBDIRS):$(AUTOPILOT_LIBDIRS)


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

$(OBJDIR)/main.o: $(SRCDIR)/main.cc 
	$(CXX) -c $(SRCDIR)/main.cc $(CXXFLAGS) -o $(OBJDIR)/main.o

$(OBJDIR)/plain.o: $(SRCDIR)/plain.cc 
	$(CXX) -c $(SRCDIR)/plain.cc $(CXXFLAGS) -o $(OBJDIR)/plain.o

$(OBJDIR)/camera.o: $(SRCDIR)/camera.cc
	$(CXX) -c $(SRCDIR)/camera.cc $(CXXFLAGS) -o $(OBJDIR)/camera.o
	
$(OBJDIR)/wind_gen.o: $(SRCDIR)/wind_gen.cc
	$(CXX) -c $(SRCDIR)/wind_gen.cc $(CXXFLAGS) -o $(OBJDIR)/wind_gen.o
	
	
$(OBJDIR)/simulator_platform.o: $(SRCDIR)/simulator_platform.cc
	$(CXX) -c $(SRCDIR)/simulator_platform.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_platform.o

$(OBJDIR)/simulator.o: $(SRCDIR)/simulator.cc
	$(CXX) -c $(SRCDIR)/simulator.cc $(CXXFLAGS) -o $(OBJDIR)/simulator.o
	
$(OBJDIR)/platforms.o: $(SRCDIR)/platforms.cc
	$(CXX) -c $(SRCDIR)/platforms.cc $(CXXFLAGS) -o $(OBJDIR)/platforms.o
	
$(OBJDIR)/simulator_gyro.o: $(SRCDIR)/sensors/simulator_gyro.cc
	$(CXX) -c $(SRCDIR)/sensors/simulator_gyro.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_gyro.o

$(OBJDIR)/simulator_accelerometer.o: $(SRCDIR)/sensors/simulator_accelerometer.cc
	$(CXX) -c $(SRCDIR)/sensors/simulator_accelerometer.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_accelerometer.o

$(OBJDIR)/simulator_magnetometer.o: $(SRCDIR)/sensors/simulator_magnetometer.cc
	$(CXX) -c $(SRCDIR)/sensors/simulator_magnetometer.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_magnetometer.o

$(OBJDIR)/simulator_gps.o: $(SRCDIR)/sensors/simulator_gps.cc
	$(CXX) -c $(SRCDIR)/sensors/simulator_gps.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_gps.o		

$(OBJDIR)/simulator_altmeter.o: $(SRCDIR)/sensors/simulator_altmeter.cc
	$(CXX) -c $(SRCDIR)/sensors/simulator_altmeter.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_altmeter.o		

$(OBJDIR)/simulator_pitot.o: $(SRCDIR)/sensors/simulator_pitot.cc
	$(CXX) -c $(SRCDIR)/sensors/simulator_pitot.cc $(CXXFLAGS) -o $(OBJDIR)/simulator_pitot.o		

$(LIBDIR)/libdgx_simulator.so: $(LIBDIR) $(OBJDIR)/plain.o $(OBJDIR)/camera.o $(OBJDIR)/wind_gen.o $(OBJDIR)/simulator_platform.o $(OBJDIR)/simulator.o $(OBJDIR)/platforms.o  $(OBJDIR)/simulator_gyro.o $(OBJDIR)/simulator_accelerometer.o $(OBJDIR)/simulator_magnetometer.o $(OBJDIR)/simulator_gps.o $(OBJDIR)/simulator_altmeter.o $(OBJDIR)/simulator_pitot.o 
	$(CXX) -shared -fPIC $(OBJDIR)/plain.o $(OBJDIR)/camera.o $(OBJDIR)/wind_gen.o $(OBJDIR)/simulator_platform.o  $(OBJDIR)/simulator.o $(OBJDIR)/platforms.o $(OBJDIR)/simulator_gyro.o $(OBJDIR)/simulator_accelerometer.o $(OBJDIR)/simulator_magnetometer.o $(OBJDIR)/simulator_gps.o $(OBJDIR)/simulator_altmeter.o  $(OBJDIR)/simulator_pitot.o $(LDFLAGS) -o $(LIBDIR)/libdgx_simulator.so

sim: $(OBJDIR)/main.o $(LIBDIR)/libdgx_simulator.so
	$(CXX) $(OBJDIR)/main.o -L$(LIBDIR) $(LDFLAGS),$(LIBDIR)/libdgx_simulator.so -ldgx_simulator -o sim 


