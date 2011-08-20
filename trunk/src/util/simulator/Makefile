# REQUIRES IRRLICHT
CXX = g++
CXXFLAGS =  -O3 -I/usr/X11R6/include 
LDFLAGS = -L/usr/X11R6/lib -lGL -lXxf86vm -lXext -lX11 -lIrrlicht

all: sim


main.o: main.cc
	$(CXX) -c main.cc $(CXXFLAGS)

sim: main.o
	$(CXX) main.o $(LDFLAGS)
