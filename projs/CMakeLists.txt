cmake_minimum_required (VERSION 2.6) 
project (DGX-Autopilot)

set(CMAKE_CXX_FLAGS "-O3 -Wall -g")  


##
## The external packages
##

find_package(GTest REQUIRED)
find_package(Armadillo REQUIRED)
find_package(Boost 1.49.0 REQUIRED COMPONENTS thread filesystem system)
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)
find_package(Qt4)
find_package(GDAL)

if (EXISTS /usr/lib/x86_64-linux-gnu/libIrrlicht.so OR EXISTS /usr/lib/libIrrlicht.so)
  set(IRRLICHT_FOUND TRUE)
else()
  set(IRRLICHT_FOUND FALSE)
endif()

##
## The subprojects
##

set (BUILD_SIMULATOR ${IRRLICHT_FOUND})
if (QT4_FOUND AND IRRLICHT_FOUND AND GDAL_FOUND)
	set (BUILD_GS TRUE)
else()
	set (BUILD_GS FALSE)
endif()

add_subdirectory(libdata_stream)

add_subdirectory(autopilot)

if (BUILD_SIMULATOR) 
	add_subdirectory(simulator)
endif()

if (BUILD_GS) 
    add_subdirectory(ground_station)
endif()

configure_file(dgx-config.cmake.in dgx-config.cmake)
