##########################################################################
# Includes
############################################################################

DGX_PATH=../../projs
include $(DGX_PATH)/Makefile.include

############################################################################
# Variable Definitions
############################################################################

CXX=g++
MOC = moc-qt4
CXXFLAGS=-Wall -g -O3
OBJDIR=./obj
AUTOGENDIR = ./autogen

############################################################################
# Targets
############################################################################
all: device host
.PHONY:
host: $(OBJDIR) $(AUTOGENDIR) device autopilot ground_station
.PHONY:
device: $(OBJDIR) autopilot_device
.PHONY:
clean:
	rm -rf $(AUTOGENDIR) $(OBJDIR) ./autopilot ./autopilot_device ./ground_station


autopilot: autopilot.cc
	$(CXX) $(CXXFLAGS) autopilot.cc -o autopilot  $(AUTOPILOT_CXXFLAGS) $(AUTOPILOT_LDFLAGS) $(DGX_SIMULATOR_CXXFLAGS) $(DGX_SIMULATOR_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(AUTOPILOT_LIBDIRS):$(DGX_SIMULATOR_LIBDIRS) -lboost_filesystem -lboost_system 

autopilot_device: autopilot.cc
	$(CXX) $(CXXFLAGS) autopilot.cc -o autopilot_device $(AUTOPILOT_CXXFLAGS) $(AUTOPILOT_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(AUTOPILOT_LIBDIRS) -lboost_filesystem -lboost_system -DDEVICE

$(OBJDIR):
	mkdir $(OBJDIR)
$(AUTOGENDIR):
	mkdir $(AUTOGENDIR)
	
$(OBJDIR)/ground_station.o: ground_station.cc
	$(CXX) -c $(CXXFLAGS) ground_station.cc -o $(OBJDIR)/ground_station.o $(AUTOPILOT_CXXFLAGS) $(GROUND_STATION_CXXFLAGS)
	
$(OBJDIR)/ground_station_moc.o: ground_station.h
	$(MOC) ground_station.h -o $(AUTOGENDIR)/ground_station_moc.cxx
	$(CXX) -c $(CXXFLAGS) $(AUTOGENDIR)/ground_station_moc.cxx -o $(OBJDIR)/ground_station_moc.o $(AUTOPILOT_CXXFLAGS) $(GROUND_STATION_CXXFLAGS)

$(OBJDIR)/gs_main.o: gs_main.cc
	$(CXX) -c $(CXXFLAGS) gs_main.cc -o $(OBJDIR)/gs_main.o $(AUTOPILOT_CXXFLAGS) $(GROUND_STATION_CXXFLAGS)
 
ground_station: $(OBJDIR)/gs_main.o $(OBJDIR)/ground_station.o $(OBJDIR)/ground_station_moc.o
	$(CXX) $(OBJDIR)/gs_main.o $(OBJDIR)/ground_station.o $(OBJDIR)/ground_station_moc.o -o ground_station $(AUTOPILOT_LDFLAGS)  $(GROUND_STATION_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(GROUND_STATION_LIBDIRS):$(AUTOPILOT_LIBDIRS)

