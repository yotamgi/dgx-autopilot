# REQUIRES IRRLICHT
CXX = g++
CXXFLAGS =  -O3 -I/usr/X11R6/include  -g
LDFLAGS = -L/usr/X11R6/lib -lGL -lXxf86vm -lXext -lX11 -lIrrlicht

OBJDIR = ./obj


.PHONY: all clean

all: $(OBJDIR) sim
clean:
	rm -rf obj sim


$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/main.o: main.cc 
	$(CXX) -c main.cc $(CXXFLAGS) -o $(OBJDIR)/main.o

$(OBJDIR)/plane.o: plane.cc 
	$(CXX) -c plane.cc $(CXXFLAGS) -o $(OBJDIR)/plane.o

$(OBJDIR)/camera.o: camera.cc
	$(CXX) -c camera.cc $(CXXFLAGS) -o $(OBJDIR)/camera.o

sim: $(OBJDIR)/main.o $(OBJDIR)/plane.o $(OBJDIR)/camera.o
	$(CXX) $(OBJDIR)/main.o $(OBJDIR)/plane.o $(OBJDIR)/camera.o $(LDFLAGS) -o sim


