LIBDS_PATH = ../libdata_stream/
include $(LIBDS_PATH)/Makefile.include

GROUND_STATION_PATH = ../ground_station/
include $(GROUND_STATION_PATH)/Makefile.include

LIBDIR = ./lib
SRCDIR = ./src

CXX = g++
ARCHIVE = ar
CXXFLAGS += -g -I$(SRCDIR) -Wall  $(LIBDS_CXXFLAGS) $(GROUND_STATION_CXXFLAGS) -O3
LDFLAGS += -lboost_thread -L./lib $(LIBDS_LDFLAGS) $(GROUND_STATION_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(GROUND_STATION_LIBDIRS)

AUTOPILIT_OBJS =  $(SRCDIR)/platform/hw/i2c_interface.o	\
		$(SRCDIR)/platform/hw/itg3200_gyro.o				\
		$(SRCDIR)/platform/hw/adxl345_acc.o				\
		$(SRCDIR)/platform/hw/hmc5843_compass.o		 	\
		$(SRCDIR)/platform/hw/maestro_servo_controller.o \
		$(SRCDIR)/platform/dgx1_platform.o \
		$(SRCDIR)/gps_filter.o \
		$(SRCDIR)/waypoint_pilot.o
		
		
GTESTS_OBJS= $(SRCDIR)/tests/test_gps_filter.o

###############################################################################
## TARGETS 
###############################################################################


## dummy targets ##############################################################
.PHONY:
all: host device

.PHONY:
host: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.a sensor_watch_test cockpit_test waypoint_pilot_test gtest  

.PHONY:
device: $(OBJDIR) $(LIBDIR) $(GTEST_OBJS) $(LIBDIR)/libautopilot.a sensor_watch_test cockpit_test waypoint_pilot_test gtest 

.PHONY:
clean:
	rm -fr $(AUTOPILIT_OBJS) */*/*.o */*.o *.o $(LIBDIR) gtest sensor_watch_test watch_test cockpit_test waypoint_pilot_test

$(LIBDIR):
	mkdir $(LIBDIR)

## real targets ###############################################################

.cc.o: Makefile
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(LIBDIR)/libautopilot.a: $(AUTOPILIT_OBJS)
	$(ARCHIVE) cr  $(LIBDIR)/libautopilot.a $(AUTOPILIT_OBJS)

gtest: $(GTESTS_OBJS) $(LIBDIR) $(LIBDIR)/libautopilot.a
	$(CXX) $(GTESTS_OBJS) -lgtest_main -lgtest -lpthread -lautopilot $(LDFLAGS) -o gtest

.PHONY:
test: gtest
	./gtest --gtest_color=yes

sensor_watch_test: $(SRCDIR)/sandbox/sensor_watch_test.o $(LIBDIR) $(LIBDIR)/libautopilot.a 
	$(CXX) $(SRCDIR)/sandbox/sensor_watch_test.o -lautopilot $(LDFLAGS) -o sensor_watch_test
	
cockpit_test: $(SRCDIR)/sandbox/cockpit_test.o $(LIBDIR) $(LIBDIR)/libautopilot.a
	$(CXX) $(SRCDIR)/sandbox/cockpit_test.o -lautopilot $(LDFLAGS) -o cockpit_test

waypoint_pilot_test: $(SRCDIR)/sandbox/waypoint_pilot_test.o $(LIBDIR) $(LIBDIR)/libautopilot.a
	$(CXX) $(SRCDIR)/sandbox/waypoint_pilot_test.o -lautopilot $(LDFLAGS)  -o waypoint_pilot_test
	
