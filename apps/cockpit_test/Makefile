###########################################################################
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
.PHONY:
all: cockpit cockpit_device
.PHONY:
clean:
	rm ./cockpit ./cockpit_device


cockpit_device: cockpit_device.cc
	$(CXX) $(CXXFLAGS) cockpit_device.cc -o cockpit_device $(AUTOPILOT_CXXFLAGS) $(AUTOPILOT_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(AUTOPILOT_LIBDIRS) -lboost_filesystem -lboost_system

cockpit: cockpit.cc
	$(CXX) $(CXXFLAGS) cockpit.cc -o cockpit $(AUTOPILOT_CXXFLAGS) $(AUTOPILOT_LDFLAGS) $(GROUND_STATION_CXXFLAGS) $(GROUND_STATION_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(GROUND_STATION_LIBDIRS):$(AUTOPILOT_LIBDIRS) 


