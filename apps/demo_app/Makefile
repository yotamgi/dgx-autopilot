##########################################################################
# Includes
############################################################################

DGX_PATH=../../projs
include $(DGX_PATH)/Makefile.include

############################################################################
# Variable Definitions
############################################################################

CXX=g++
CXXFLAGS=-Wall

############################################################################
# Targets
############################################################################
all: device host
.PHONY:
host: device autopilot ground_station
.PHONY:
device: autopilot_device
.PHONY:
clean:
	rm -f ./autopilot ./autopilot_device ./ground_station


autopilot: autopilot.cc
	$(CXX) $(CXXFLAGS) autopilot.cc -o autopilot  $(AUTOPILOT_CXXFLAGS) $(AUTOPILOT_LDFLAGS) $(DGX_SIMULATOR_CXXFLAGS) $(DGX_SIMULATOR_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(AUTOPILOT_LIBDIRS):$(DGX_SIMULATOR_LIBDIRS) -lboost_filesystem -lboost_system 

autopilot_device: autopilot.cc
	$(CXX) $(CXXFLAGS) autopilot.cc -o autopilot_device $(AUTOPILOT_CXXFLAGS) $(AUTOPILOT_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(AUTOPILOT_LIBDIRS) -lboost_filesystem -lboost_system -DDEVICE


ground_station: ground_station.cc
	$(CXX) $(CXXFLAGS) ground_station.cc -o ground_station $(AUTOPILOT_CXXFLAGS) $(AUTOPILOT_LDFLAGS) $(GROUND_STATION_CXXFLAGS) $(GROUND_STATION_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(GROUND_STATION_LIBDIRS):$(AUTOPILOT_LIBDIRS)

