##
## CMake header
##

cmake_minimum_required (VERSION 2.6) 
project (DGX-Autopilot-demo-app)

set(CMAKE_CXX_FLAGS "-O3 -Wall -g")  

find_package(dgx REQUIRED)
find_package(Qt4)

##
## The ground station
##

if (DGX_GS_EXISTS)
	qt4_wrap_cpp(GS_MOC ground_station.h)
	include_directories(${DGX_GS_INCLUDE})
	add_definitions(${DGX_GS_DEFINITIONS})
	add_executable(ground_station gs_main.cc ground_station.cc ${GS_MOC})
	target_link_libraries(ground_station ${DGX_GS_LIB})
endif()

##
## The autopilot
##

if (DGX_SIMULATOR_EXISTS) 
	include_directories(${DGX_SIMULATOR_INCLUDE})	
	include_directories(${DGX_AUTOPILOT_INCLUDE})
	add_executable(autopilot autopilot.cc)
	target_link_libraries(autopilot ${DGX_SIMULATOR_LIB})
	target_link_libraries(autopilot ${DGX_AUTOPILOT_LIB})
else()
	add_definitions( -DDEVICE )
	include_directories(${DGX_AUTOPILOT_INCLUDE})
	add_executable(autopilot-device autopilot.cc)
	target_link_libraries(autopilot-device ${DGX_AUTOPILOT_LIB})
endif()


