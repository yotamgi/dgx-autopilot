LIBDS_PATH = ../libdata_stream/
include $(LIBDS_PATH)/Makefile.include

STREAMWATCH_PATH = ../stream_watch
include $(STREAMWATCH_PATH)/Makefile.include

CXX = g++
ARCHIVE = ar
CXXFLAGS += -g -I. -Wall  $(LIBDS_CXXFLAGS) $(STREAMWATCH_CXXFLAGS) -O3
LDFLAGS += -lboost_thread -L./lib $(LIBDS_LDFLAGS) $(STREAMWATCH_LDFLAGS)

LIBDIR = ./lib

AUTOPILIT_OBJS =  hw/i2c_interface.o	\
		hw/itg3200_gyro.o				\
		hw/adxl345_acc.o				\
		hw/hmc5843_compass.o		 	\
		hw/maestro_servo_controller.o

GTESTS_OBJS=	

###############################################################################
## TARGETS 
###############################################################################


## dummy targets ##############################################################
.PHONY:
all: host device

.PHONY:
host: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.a sensor_watch_test gtest 

.PHONY:
device: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.a sensor_watch_test gtest 

.PHONY:
clean:
	rm -fr */*.o *.o $(LIBDIR) gtest sensor_watch_test watch_test 

$(LIBDIR):
	mkdir $(LIBDIR)

## real targets ###############################################################

.cc.o:
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(LIBDIR)/libautopilot.a: $(AUTOPILIT_OBJS)
	$(ARCHIVE) cr  $(LIBDIR)/libautopilot.a $(AUTOPILIT_OBJS)

gtest: $(GTESTS_OBJS) $(LIBDIR) $(LIBDIR)/libautopilot.a
	$(CXX) $(LDFLAGS) $(GTESTS_OBJS) -lautopilot -lgtest_main -o gtest

.PHONY:
test: gtest
	./gtest --gtest_color=yes

sensor_watch_test: sandbox/sensor_watch_test.o $(LIBDIR) $(LIBDIR)/libautopilot.a 
	$(CXX) ./sandbox/sensor_watch_test.o $(LDFLAGS) -lautopilot -o sensor_watch_test
