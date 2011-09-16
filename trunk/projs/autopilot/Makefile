LIBDS_PATH = ../libdata_stream/
include $(LIBDS_PATH)/Makefile.include

STREAMWATCH_PATH = ../stream_watch/device
include $(STREAMWATCH_PATH)/Makefile.include

CXX = g++
ARCHIVE = ar
CXXFLAGS = -g -I. -Wall  $(LIBDS_CXXFLAGS) $(STREAMWATCH_CXXFLAGS)
LDFLAGS = -lboost_thread -L./lib $(LIBDS_LDFLAGS) $(STREAMWATCH_LDFLAGS)

LIBDIR = ./lib

AUTOPILIT_OBJS =  hw/i2c_interface.o	\
		hw/itg3200_gyro.o				\
		hw/maestro_servo_controller.o

GTESTS_OBJS=	./tests/integral_test.o  \
				./tests/static_filter_test.o

###############################################################################
## TARGETS 
###############################################################################


## dummy targets ##############################################################
.PHONY:
all: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.a orientation_watch_test orientation_test gtest 

.PHONY:
clean:
	rm -fr */*.o *.o $(LIBDIR) gtest orientation_watch_test watch_test

$(LIBDIR):
	mkdir $(LIBDIR)

## real targets ###############################################################

.cc.o:
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(LIBDIR)/libautopilot.a: $(AUTOPILIT_OBJS)
	$(ARCHIVE) cr  $(LIBDIR)/libautopilot.a $(AUTOPILIT_OBJS)

gtest: $(GTESTS_OBJS) $(LIBDIR) $(LIBDIR)/libautopilot.a
	$(CXX) $(LDFLAGS) $(GTESTS_OBJS) -lautopilot -o orientation_watch_test -lgtest_main -o gtest

.PHONY:
test: gtest
	./gtest --gtest_color=yes

orientation_test: sandbox/orientation_test.o $(LIBDIR) $(LIBDIR)/libautopilot.a 
	$(CXX) ./sandbox/orientation_test.o $(LDFLAGS) -lautopilot -o orientation_test

orientation_watch_test: sandbox/orientation_watch_test.o $(LIBDIR) $(LIBDIR)/libautopilot.a 
	$(CXX) ./sandbox/orientation_watch_test.o $(LDFLAGS) -lautopilot -o orientation_watch_test
