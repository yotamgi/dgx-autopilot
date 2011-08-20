# REQUIRES IRRLICHT
CXX = g++
CXXFLAGS =  -O3 -I/usr/X11R6/include 
LDFLAGS = -L/usr/X11R6/lib -lGL -lXxf86vm -lXext -lX11 -lIrrlicht

all: sim


main.o: main.cc
	$(CXX) -c main.cc $(CXXFLAGS)

plane.o: plane.cc
	$(CXX) -c plane.cc $(CXXFLAGS)

camera.o: camera.cc
	$(CXX) -c camera.cc $(CXXFLAGS)

sim: main.o plane.o camera.o
	$(CXX) main.o plane.o camera.o $(LDFLAGS)
