LIBDS_PATH = ../libdata_stream/
include $(LIBDS_PATH)/Makefile.include

LIBDIR = ./lib
SRCDIR = ./src

CXX = g++
CXXFLAGS += -g -I$(SRCDIR) -Wall  $(LIBDS_CXXFLAGS) -O3 -fPIC
LDFLAGS += -L./lib -fPIC -Wl,--rpath,$(LIBDS_LIBDIRS):$(LIBDIR) $(LIBDS_LDFLAGS) \
			-lboost_thread -lboost_filesystem-mt -lboost_system-mt -lgps

AUTOPILIT_OBJS =  $(SRCDIR)/platform/hw/i2c_interface.o	\
		$(SRCDIR)/platform/hw/l3g4200d_gyro.o			\
		$(SRCDIR)/platform/hw/itg3200_gyro.o				\
		$(SRCDIR)/platform/hw/adxl345_acc.o				\
 		$(SRCDIR)/platform/hw/bmp085_baro.o				\
		$(SRCDIR)/platform/hw/ads1115_adc.o				\
		$(SRCDIR)/platform/hw/hmc5843_compass.o		 	\
		$(SRCDIR)/platform/hw/maestro_servo_controller.o \
		$(SRCDIR)/platform/hw/beagleboard_gpio.o \
		$(SRCDIR)/platform/hw/gps.o \
		$(SRCDIR)/platform/dgx1_platform.o \
		$(SRCDIR)/platform/flapron_controller.o \
		$(SRCDIR)/platform/platform_export_import.o \
		$(SRCDIR)/cockpit.o \
		$(SRCDIR)/stability_augmenting_pilot.o \
		$(SRCDIR)/waypoint_pilot.o \
		$(SRCDIR)/fusion_filter.o
		
		
GTESTS_OBJS= 

###############################################################################
## TARGETS 
###############################################################################


## dummy targets ##############################################################
.PHONY:
all: host device

.PHONY:
host: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.so sensor_watch_test 

.PHONY:
device: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.so sensor_watch_test gtest gps_test servo_test baro_test pitot_test

.PHONY:
clean:
	rm -fr $(AUTOPILIT_OBJS) */*/*.o */*.o *.o $(LIBDIR) gtest sensor_watch_test watch_test gps_test

$(LIBDIR):
	mkdir $(LIBDIR)

## real targets ###############################################################

.cc.o: Makefile
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(LIBDIR)/libautopilot.so: $(AUTOPILIT_OBJS)
	$(CXX) -shared  -o $(LIBDIR)/libautopilot.so $(AUTOPILIT_OBJS) $(LDFLAGS)

gtest: $(GTESTS_OBJS) $(LIBDIR) $(LIBDIR)/libautopilot.so
	$(CXX) $(GTESTS_OBJS) $(LDFLAGS) -o gtest -lgtest_main -lgtest -lpthread -lautopilot 

.PHONY:
test: gtest
	./gtest --gtest_color=yes

sensor_watch_test: $(SRCDIR)/sandbox/sensor_watch_test.o $(LIBDIR) $(LIBDIR)/libautopilot.so 
	$(CXX) $(SRCDIR)/sandbox/sensor_watch_test.o -lautopilot $(LDFLAGS) -o sensor_watch_test

servo_test: $(SRCDIR)/sandbox/servo_test.o $(LIBDIR) $(LIBDIR)/libautopilot.so 
	$(CXX) $(SRCDIR)/sandbox/servo_test.o -lautopilot $(LDFLAGS) -o servo_test

gps_test: $(SRCDIR)/sandbox/gps_test.o $(LIBDIR) $(LIBDIR)/libautopilot.so 
	$(CXX) $(SRCDIR)/sandbox/gps_test.o -lautopilot $(LDFLAGS) -o gps_test
	
baro_test: $(SRCDIR)/sandbox/baro_test.o $(LIBDIR) $(LIBDIR)/libautopilot.so 
	$(CXX) $(SRCDIR)/sandbox/baro_test.o -lautopilot $(LDFLAGS) -o baro_test
	
pitot_test: $(SRCDIR)/sandbox/pitot_test.o $(LIBDIR) $(LIBDIR)/libautopilot.so 
	$(CXX) $(SRCDIR)/sandbox/pitot_test.o -lautopilot $(LDFLAGS) -o pitot_test
	
