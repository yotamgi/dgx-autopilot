LIBDS_PATH = ../libdata_stream/
include $(LIBDS_PATH)/Makefile.include

STREAM_WATCH_PATH = ../stream_watch
include $(STREAM_WATCH_PATH)/Makefile.include

LIBDIR = ./lib
SRCDIR = ./src

CXX = g++
ARCHIVE = ar
CXXFLAGS += -g -I$(SRCDIR) -Wall  $(LIBDS_CXXFLAGS) $(STREAM_WATCH_CXXFLAGS) -O3
LDFLAGS += -lboost_thread -llapack -L./lib $(LIBDS_LDFLAGS) $(STREAM_WATCH_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(STREAM_WATCH_LIBDIRS)

AUTOPILIT_OBJS =  $(SRCDIR)/platform/hw/i2c_interface.o	\
		$(SRCDIR)/platform/hw/itg3200_gyro.o				\
		$(SRCDIR)/platform/hw/adxl345_acc.o				\
		$(SRCDIR)/platform/hw/hmc5843_compass.o		 	\
		$(SRCDIR)/platform/hw/maestro_servo_controller.o \
		$(SRCDIR)/platform/dgx1_platform.o \
		$(SRCDIR)/gps_filter.o			
		
GTESTS_OBJS= $(SRCDIR)/tests/test_gps_filter.o

###############################################################################
## TARGETS 
###############################################################################


## dummy targets ##############################################################
.PHONY:
all: host device

.PHONY:
host: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.a sensor_watch_test cockpit_test gtest  

.PHONY:
device: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.a sensor_watch_test cockpit_test gtest 

.PHONY:
clean:
	rm -fr $(AUTOPILIT_OBJS) */*/*.o */*.o *.o $(LIBDIR) gtest sensor_watch_test watch_test cockpit_test

$(LIBDIR):
	mkdir $(LIBDIR)

## real targets ###############################################################

.cc.o: Makefile
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(LIBDIR)/libautopilot.a: $(AUTOPILIT_OBJS)
	$(ARCHIVE) cr  $(LIBDIR)/libautopilot.a $(AUTOPILIT_OBJS)

gtest: $(GTESTS_OBJS) $(LIBDIR) $(LIBDIR)/libautopilot.a
	$(CXX) $(GTESTS_OBJS) $(LDFLAGS) -lgtest_main -lgtest -lpthread -lautopilot -o gtest

.PHONY:
test: gtest
	./gtest --gtest_color=yes

sensor_watch_test: $(SRCDIR)/sandbox/sensor_watch_test.o $(LIBDIR) $(LIBDIR)/libautopilot.a 
	$(CXX) $(SRCDIR)/sandbox/sensor_watch_test.o $(LDFLAGS) -lautopilot -o sensor_watch_test
	
cockpit_test: $(SRCDIR)/sandbox/cockpit_test.o $(LIBDIR) $(LIBDIR)/libautopilot.a
	$(CXX) $(SRCDIR)/sandbox/cockpit_test.o $(LDFLAGS) -lautopilot -o cockpit_test
