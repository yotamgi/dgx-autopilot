##########################################################################
# Includes
############################################################################

DGX_PATH=../../projs
include $(DGX_PATH)/Makefile.include

############################################################################
# Variable Definitions
############################################################################

CXX=g++
MOC = moc-qt4
CXXFLAGS=-Wall -g -O3
OBJDIR=./obj
AUTOGENDIR = ./autogen

############################################################################
# Targets
############################################################################
all: device host
.PHONY:
host: $(OBJDIR) $(AUTOGENDIR) cockpit_player
.PHONY:
device: 
.PHONY:
clean:
	rm -rf $(AUTOGENDIR) $(OBJDIR) ./autopilot ./autopilot_device ./cockpit_player

$(OBJDIR):
	mkdir $(OBJDIR)
$(AUTOGENDIR):
	mkdir $(AUTOGENDIR)
	
$(OBJDIR)/cockpit_player.o: cockpit_player.cc
	$(CXX) -c $(CXXFLAGS) cockpit_player.cc -o $(OBJDIR)/cockpit_player.o $(AUTOPILOT_CXXFLAGS) $(GROUND_STATION_CXXFLAGS)
	
$(OBJDIR)/cockpit_player_moc.o: cockpit_player.h
	$(MOC) cockpit_player.h -o $(AUTOGENDIR)/cockpit_player_moc.cxx
	$(CXX) -c $(CXXFLAGS) $(AUTOGENDIR)/cockpit_player_moc.cxx -o $(OBJDIR)/cockpit_player_moc.o $(AUTOPILOT_CXXFLAGS) $(GROUND_STATION_CXXFLAGS)

$(OBJDIR)/cockpit_player_main.o: cockpit_player_main.cc
	$(CXX) -c $(CXXFLAGS) cockpit_player_main.cc -o $(OBJDIR)/cockpit_player_main.o $(AUTOPILOT_CXXFLAGS) $(GROUND_STATION_CXXFLAGS)
 
cockpit_player: $(OBJDIR)/cockpit_player_main.o $(OBJDIR)/cockpit_player.o $(OBJDIR)/cockpit_player_moc.o
	$(CXX) $(OBJDIR)/cockpit_player_main.o $(OBJDIR)/cockpit_player.o $(OBJDIR)/cockpit_player_moc.o -o cockpit_player $(AUTOPILOT_LDFLAGS)  $(GROUND_STATION_LDFLAGS) -Wl,--rpath,$(LIBDS_LIBDIRS):$(GROUND_STATION_LIBDIRS):$(AUTOPILOT_LIBDIRS)

