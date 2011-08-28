CXX = g++
ARCHIVE = ar
CXXFLAGS = -O2 -g -I.
LDFLAGS = -lboost_thread 

LIBDIR = ./lib

AUTOPILIT_OBJS =  hw/i2c_interface.o	\
		hw/itg3200_gyro.o				\
		hw/maestro_servo_controller.o	\
		util/watch.o

###############################################################################
## TARGETS 
###############################################################################


## dummy targets ##############################################################
all: $(OBJDIR) $(LIBDIR) $(LIBDIR)/libautopilot.a

clean:
	rm -fr */*.o *.o $(LIBDIR)

$(LIBDIR):
	mkdir $(LIBDIR)

## real targets ###############################################################

.cc.o:
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(LIBDIR)/libautopilot.a: $(AUTOPILIT_OBJS)
	$(ARCHIVE) cr  $(LIBDIR)/libautopilot.a $(AUTOPILIT_OBJS)
