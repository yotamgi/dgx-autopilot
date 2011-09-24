# REQUIRES IRRLICHT

LIBDS_PATH = ../libdata_stream
include $(LIBDS_PATH)/Makefile.include

CXX = g++
CXXFLAGS +=  -I/usr/X11R6/include  -g -fPIC  $(LIBDS_CXXFLAGS) -Iinclude
LDFLAGS += -L/usr/X11R6/lib -lGL -lXxf86vm -lXext -lX11 -lIrrlicht -lboost_thread -fPIC $(LIBDS_LDFLAGS)

LIBDIR=./lib
BINDIR=./bin
SRCDIR=./src
MEDIADIR=./media

DUMMY_OBJS = ./src/dummy_client/dummy.o
PRESENTER_OBJS = ./src/stream_presenter_3d/main.o

# Makefile interface targets
.PHONY: all device host clean
all: device host 
device: $(BINDIR) $(BINDIR)/dummy_client
host: $(LIBDIR) $(LIBDIR)/libstream_presenter.so $(BINDIR)/presenter
clean: 
	rm -rf $(LIBDIR) $(BINDIR) */*.o */*/*.o

$(BINDIR):
	mkdir $(BINDIR)
$(LIBDIR):
	mkdir $(LIBDIR)

.cc.o:
	$(CXX) -c $(CXXFLAGS) $<  -o $*.o

$(SRCDIR)/stream_presenter_3d/stream_presenter.o: $(SRCDIR)/stream_presenter_3d/stream_presenter.cc
	$(CXX) -c $(CXXFLAGS)  $(SRCDIR)/stream_presenter_3d/stream_presenter.cc -o $(SRCDIR)/stream_presenter_3d/stream_presenter.o -DMEDIA_DIR=\"$(MEDIADIR)\"

$(LIBDIR)/libstream_presenter.so: $(LIBDIR) $(SRCDIR)/stream_presenter_3d/stream_presenter.o
	$(CXX) $(SRCDIR)/stream_presenter_3d/stream_presenter.o $(LDFLAGS) -shared -o $(LIBDIR)/libstream_presenter.so

$(BINDIR)/dummy_client: $(DUMMY_OBJS) $(LIBDIR)/libstream_presenter.so
	$(CXX) $(DUMMY_OBJS) $(LDFLAGS) -o $(BINDIR)/dummy_client -L$(LIBDIR) -lstream_presenter  -Wl,-rpath,$(LIBDIR):$(LIBDS_LIBDIRS)

$(BINDIR)/presenter: $(BINDIR) $(PRESENTER_OBJS)
	$(CXX) $(PRESENTER_OBJS) -L$(LIBDIR) -lstream_presenter $(LDFLAGS) -o $(BINDIR)/presenter  -Wl,-rpath,$(LIBDIR):$(LIBDS_LIBDIRS)

