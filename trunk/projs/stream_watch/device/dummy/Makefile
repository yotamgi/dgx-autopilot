# libdata_stream pathes
LIBDS_PATH = ../../../libdata_stream/
include $(LIBDS_PATH)/Makefile.include

# stream_watch pathes
STREAMWATCH_PATH = ../
include $(STREAMWATCH_PATH)/Makefile.include

CXX = g++
CXXFLAGS += -g -I. -Wall $(LIBDS_CXXFLAGS) $(STREAMWATCH_CXXFLAGS)
LDFLAGS += -lboost_thread -L./lib $(LIBDS_LDFLAGS) $(STREAMWATCH_LDFLAGS)

TARGET = dummy
OBJS =  dummy.o

###############################################################################
## TARGETS 
###############################################################################

.PHONY:
all: $(TARGET)

.PHONY:
clean:
	rm -fr $(OBJS) $(TARGET)

## real targets ###############################################################

.cc.o:
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(TARGET)
