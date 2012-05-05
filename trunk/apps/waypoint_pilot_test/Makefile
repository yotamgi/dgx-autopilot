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
all:  waypoint_pilot_test_device
.PHONY:
clean:
	rm -f waypoint_pilot_test waypoint_pilot_test_device

waypoint_pilot_test: waypoint_pilot_test.cc
	$(CXX) $(CXXFLAGS) waypoint_pilot_test.cc -o waypoint_pilot_test $(AUTOPILOT_CXXFLAGS) $(AUTOPILOT_LDFLAGS) $(DGX_SIMULATOR_CXXFLAGS) $(DGX_SIMULATOR_LDFLAGS) $(GROUND_STATION_CXXFLAGS) $(GROUND_STATION_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(GROUND_STATION_LIBDIRS):$(AUTOPILOT_LIBDIRS):$(DGX_SIMULATOR_LIBDIRS) 


waypoint_pilot_test_device: waypoint_pilot_test_device.cc
	$(CXX) $(CXXFLAGS) waypoint_pilot_test_device.cc -o waypoint_pilot_test_device $(AUTOPILOT_CXXFLAGS) $(AUTOPILOT_LDFLAGS)  -Wl,--rpath,$(LIBDS_LIBDIRS):$(AUTOPILOT_LIBDIRS) 


