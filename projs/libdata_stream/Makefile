CXX = g++
CXXFLAGS += -g -Wall -O3 -Isrc -Iinclude -fPIC
LDFLAGS = 
LIBDIR = ./lib

DATA_STREAM_OBJS = src/stream_exporter.o \
				   src/stream_importer.o \
				   src/tcpip_connection.o \
				   src/filters/gyro_to_av_matrix.o \
				   src/filters/matrix_to_euler_filter.o \
				   src/filters/acc_compass_rot.o 
				    
GTEST_OBJS = tests/exporter_test.o  \
			 tests/integral_test.o \
			 tests/static_filter_test.o \
			 tests/low_pass_test.o \
			 tests/watch_test.o \
			 tests/tcpip_conn_test.o

all: host device 
host: $(LIBDIR)/libdata_stream.so gtest
device: $(LIBDIR)/libdata_stream.so gtest
clean:
	rm -rf $(LIBDIR) $(DATA_STREAM_OBJS) $(GTEST_OBJS) gtest

.cc.o:
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(LIBDIR):
	mkdir $(LIBDIR)
	

$(LIBDIR)/libdata_stream.so: $(LIBDIR) $(DATA_STREAM_OBJS)
	$(CXX)  $(DATA_STREAM_OBJS) -fPIC -shared -o $(LIBDIR)/libdata_stream.so
	
gtest: $(GTEST_OBJS) $(LIBDIR) $(LIBDIR)/libdata_stream.so
	$(CXX) $(LDFLAGS) $(GTEST_OBJS) -Llib -ldata_stream -lpthread -lgtest -lgtest_main -lboost_thread \
			 -Wl,--rpath,./lib -o  gtest 

	  
