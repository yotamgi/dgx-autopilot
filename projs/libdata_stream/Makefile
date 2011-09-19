CXX = g++
CXXFLAGS += -g -Wall -O3 -Isrc
LDFLAGS = 
LIBDIR = ./lib

DATA_STREAM_OBJS = src/stream_exporter.o
GTEST_OBJS = tests/exporter_test.o 

all: host device 
host: $(LIBDIR)/libdata_stream.so gtest
device: $(LIBDIR)/libdata_stream.so gtest
clean:
	rm -f $(LIBDIR)/libdata_stream.so */*.o gtest

.cc.o:
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(LIBDIR):
	mkdir $(LIBDIR)
	

$(LIBDIR)/libdata_stream.so: $(LIBDIR) $(DATA_STREAM_OBJS)
	$(CXX)  $(DATA_STREAM_OBJS) -fPIC -shared -o $(LIBDIR)/libdata_stream.so
	
gtest: $(GTEST_OBJS) $(LIBDIR) $(LIBDIR)/libdata_stream.so
	$(CXX) $(LDFLAGS) $(GTEST_OBJS) -Llib -ldata_stream -lpthread -lgtest_main -Wl,--rpath,./lib -o  gtest 

	  