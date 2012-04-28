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
all: waypoint_pilot_test
.PHONY:
clean:
	rm waypoint_pilot_test

waypoint_pilot_test: waypoint_pilot_test.cc
	$(CXX) $(CXXFLAGS) waypoint_pilot_test.cc -o waypoint_pilot_test $(AUTOPILOT_CXXFLAGS) $(AUTOPILOT_LDFLAGS) $(GROUND_STATION_CXXFLAGS) $(GROUND_STATION_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(GROUND_STATION_LIBDIRS):$(AUTOPILOT_LIBDIRS) 


