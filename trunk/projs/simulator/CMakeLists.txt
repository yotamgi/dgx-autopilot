
##
## For those who want to work with it
##
set(SIMULATOR_INCLUDE 	${CMAKE_SOURCE_DIR}/simulator/include 			PARENT_SCOPE)
set(SIMULATOR_LIB     	${CMAKE_SOURCE_DIR}/simulator/libsimulator.so 	PARENT_SCOPE)

##
## The library
##

include_directories(src)
include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${AUTOPILOT_INCLUDE})

set(SIMULATOR_SRC   src/camera.cc
					src/plain.cc
					src/platforms.cc
					src/simulator.cc
					src/simulator_platform.cc
					src/wind_gen.cc
					src/sensors/simulator_accelerometer.cc
					src/sensors/simulator_altmeter.cc
					src/sensors/simulator_gps.cc
					src/sensors/simulator_gyro.cc
					src/sensors/simulator_magnetometer.cc
					src/sensors/simulator_pitot.cc)



add_library(simulator SHARED ${SIMULATOR_SRC})
target_link_libraries(simulator ${AUTOPILOT_LIB} Irrlicht blas lapack)

add_definitions( -DMEDIA_DIR=\"${CMAKE_SOURCE_DIR}/simulator/media\" )

##
## The executable
##

add_executable(sim src/main.cc)
target_link_libraries(sim simulator)
