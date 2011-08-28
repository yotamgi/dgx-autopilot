CXX = g++
ARCHIVE = ar
CXXFLAGS = -g -I. -Wall 
LDFLAGS = -lboost_thread -L./lib

LIBDIR = ./lib

AUTOPILIT_OBJS =  hw/i2c_interface.o	\
		hw/itg3200_gyro.o				\
		hw/maestro_servo_controller.o	\
		util/watch.o

GTESTS_OBJS=	./tests/integral_test.o

###############################################################################
## TARGETS 
###############################################################################


## dummy targets ##############################################################
.PHONY:
all: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.a

.PHONY:
clean:
	rm -fr */*.o *.o $(LIBDIR) gtest orientation_watch_test

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
	
orientation_watch_test: sandbox/orientation_watch_test.o $(LIBDIR) $(LIBDIR)/libautopilot.a 
	$(CXX) ./sandbox/orientation_watch_test.o $(LDFLAGS) -lautopilot -o orientation_watch_test
