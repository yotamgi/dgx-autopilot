# REQUIRES IRRLICHT
CXX = g++
CXXFLAGS =  -O3 -I/usr/X11R6/include  -g -fPIC
LDFLAGS = -L/usr/X11R6/lib -lGL -lXxf86vm -lXext -lX11 -lIrrlicht -lboost_thread -fPIC

SWIG_PY_VER=2.7

all: swig
swig: _direction.so


direction_presenter.o: direction_presenter.cc
	$(CXX) -c direction_presenter.cc $(CXXFLAGS)

wrap_direction.cc: direction.i direction_presenter.h
	swig -c++ -python -o wrap_direction.cc direction.i 

wrap_direction.o: wrap_direction.cc
	$(CXX) -c wrap_direction.cc $(CXXFLAGS)  -I/usr/include/python$(SWIG_PY_VER)/

.PHONY:
_direction.so: direction_presenter.o wrap_direction.o
	$(CXX) direction_presenter.o wrap_direction.o $(LDFLAGS) -shared -o _direction.so

.PHONY:
clean:
	rm -f *.o wrap_direction.cc direction.py *.so *.pyc

	

