##
## CMake header
##

cmake_minimum_required (VERSION 2.6) 
project (DGX-Autopilot-cockpit-player)

set(CMAKE_CXX_FLAGS "-O3 -Wall -g")  

find_package(dgx REQUIRED)
find_package(Qt4)

##
## The ground station
##

if (DGX_GS_EXISTS)
	qt4_wrap_cpp(CP_MOC cockpit_player.h)
	include_directories(${DGX_GS_INCLUDE} ${DGX_AUTOPILOT_INCLUDE})
	add_definitions(${DGX_GS_DEFINITIONS})
	add_executable(cockpit_player cockpit_player_main.cc cockpit_player.cc ${CP_MOC})
	target_link_libraries(cockpit_player ${DGX_GS_LIB} ${DGX_AUTOPILOT_LIB})
endif()

