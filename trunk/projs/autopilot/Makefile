LIBDS_PATH = ../libdata_stream/
include $(LIBDS_PATH)/Makefile.include

GROUND_STATION_PATH = ../ground_station/
include $(GROUND_STATION_PATH)/Makefile.include

LIBDIR = ./lib
SRCDIR = ./src

CXX = g++
ARCHIVE = ar
CXXFLAGS += -g -I$(SRCDIR) -Wall  $(LIBDS_CXXFLAGS) -O3 -fPIC
LDFLAGS += -L./lib -fPIC -Wl,--rpath,$(LIBDS_LIBDIRS):$(GROUND_STATION_LIBDIRS):$(LIBDIR) $(LIBDS_LDFLAGS) -lboost_thread

AUTOPILIT_OBJS =  $(SRCDIR)/platform/hw/i2c_interface.o	\
		$(SRCDIR)/platform/hw/itg3200_gyro.o				\
		$(SRCDIR)/platform/hw/adxl345_acc.o				\
		$(SRCDIR)/platform/hw/hmc5843_compass.o		 	\
		$(SRCDIR)/platform/hw/maestro_servo_controller.o \
		$(SRCDIR)/platform/dgx1_platform.o \
		$(SRCDIR)/gps_filter.o \
		$(SRCDIR)/waypoint_pilot.o \
		$(SRCDIR)/fusion_filter.o 
		
		
GTESTS_OBJS= $(SRCDIR)/tests/test_gps_filter.o

###############################################################################
## TARGETS 
###############################################################################


## dummy targets ##############################################################
.PHONY:
all: host device

.PHONY:
host: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.so sensor_watch_test cockpit_test cockpit_test_device waypoint_pilot_test gtest  

.PHONY:
device: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.so sensor_watch_test cockpit_test_device waypoint_pilot_test gtest 

.PHONY:
clean:
	rm -fr $(AUTOPILIT_OBJS) */*/*.o */*.o *.o $(LIBDIR) gtest sensor_watch_test watch_test cockpit_test* waypoint_pilot_test

$(LIBDIR):
	mkdir $(LIBDIR)

## real targets ###############################################################

.cc.o: Makefile
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

# specific objs that need different cxxflags
$(SRCDIR)/sandbox/cockpit_test.o: $(SRCDIR)/sandbox/cockpit_test.cc
	$(CXX) -c $(CXXFLAGS)  $(GROUND_STATION_CXXFLAGS) $(SRCDIR)/sandbox/cockpit_test.cc -o $(SRCDIR)/sandbox/cockpit_test.o
$(SRCDIR)/sandbox/waypoint_pilot_test.o: $(SRCDIR)/sandbox/waypoint_pilot_test.cc
	$(CXX) -c $(CXXFLAGS)  $(GROUND_STATION_CXXFLAGS) $(SRCDIR)/sandbox/waypoint_pilot_test.cc -o $(SRCDIR)/sandbox/waypoint_pilot_test.o
	

$(LIBDIR)/libautopilot.so: $(AUTOPILIT_OBJS)
	$(CXX) -shared  -o $(LIBDIR)/libautopilot.so $(AUTOPILIT_OBJS) $(LDFLAGS)

gtest: $(GTESTS_OBJS) $(LIBDIR) $(LIBDIR)/libautopilot.so
	$(CXX) $(GTESTS_OBJS) $(LDFLAGS) -o gtest -lgtest_main -lgtest -lpthread -lautopilot 

.PHONY:
test: gtest
	./gtest --gtest_color=yes

sensor_watch_test: $(SRCDIR)/sandbox/sensor_watch_test.o $(LIBDIR) $(LIBDIR)/libautopilot.so 
	$(CXX) $(SRCDIR)/sandbox/sensor_watch_test.o -lautopilot $(LDFLAGS) -o sensor_watch_test
	
cockpit_test: $(SRCDIR)/sandbox/cockpit_test.o $(LIBDIR) $(LIBDIR)/libautopilot.so
	$(CXX) $(SRCDIR)/sandbox/cockpit_test.o -lautopilot  $(GROUND_STATION_LDFLAGS) $(LDFLAGS) -o cockpit_test
	
cockpit_test_device: $(SRCDIR)/sandbox/cockpit_test_device.o $(LIBDIR) $(LIBDIR)/libautopilot.so
	$(CXX) $(SRCDIR)/sandbox/cockpit_test_device.o -lautopilot  $(LDFLAGS) -o cockpit_test_device
	
waypoint_pilot_test: $(SRCDIR)/sandbox/waypoint_pilot_test.o $(LIBDIR) $(LIBDIR)/libautopilot.so
	$(CXX) $(SRCDIR)/sandbox/waypoint_pilot_test.o -lautopilot $(GROUND_STATION_LDFLAGS) $(LDFLAGS)  -o waypoint_pilot_test
	
