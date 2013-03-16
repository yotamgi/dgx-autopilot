
##
## For those who want to work with it
##
set(AUTOPILOT_INCLUDE 	${CMAKE_SOURCE_DIR}/autopilot/include 
					    ${CMAKE_SOURCE_DIR}/autopilot/src 
						${LIBDS_INCLUDE}	
	PARENT_SCOPE)

set(AUTOPILOT_LIB     	${CMAKE_SOURCE_DIR}/autopilot/libautopilot.so 
						${LIBDS_LIB} 	
						${Boost_THREAD_LIBRARY}
						${Boost_FILESYSTEM_LIBRARY}
						${Boost_SYSTEM_LIBRARY}
	PARENT_SCOPE)

##
## The library
##

include_directories(src)
include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LIBDS_INCLUDE})

set(AUTOPILOT_SRC src/platform/hw/i2c_interface.cc	
				  src/platform/hw/l3g4200d_gyro.cc			
				  src/platform/hw/itg3200_gyro.cc			
				  src/platform/hw/adxl345_acc.cc				
				  src/platform/hw/ads1115_adc.cc				
				  src/platform/hw/hmc5843_compass.cc		 	
				  src/platform/hw/bmp085_baro.cc			    
                  src/platform/hw/maestro_servo_controller.cc 
                  src/platform/hw/beagleboard_gpio.cc 
                  src/platform/hw/gps.cc 
                  src/platform/dgx1_platform.cc 
                  src/platform/flapron_controller.cc 
                  src/platform/platform_export_import.cc 
                  src/cockpit.cc 
                  src/stability_augmenting_pilot.cc 
                  src/waypoint_pilot.cc 
                  src/fusion_filter.cc)

add_library(autopilot SHARED ${AUTOPILOT_SRC})
target_link_libraries(autopilot ${LIBDS_LIB} 
								${Boost_THREAD_LIBRARY} 
								${ARMADILLO_LIBRARIES}
								${Boost_FILESYSTEM_LIBRARY} 
								${Boost_SYSTEM_LIBRARY} 
								gps blas lapack)

##
## The executables
##

add_executable(sensor_watch_test src/sandbox/sensor_watch_test.cc)
target_link_libraries(sensor_watch_test autopilot)

add_executable(servo_test src/sandbox/servo_test.cc)
target_link_libraries(servo_test autopilot)

add_executable(gps_test src/sandbox/gps_test.cc)
target_link_libraries(gps_test autopilot)

add_executable(baro_test src/sandbox/baro_test.cc)
target_link_libraries(baro_test autopilot)

add_executable(pitot_test src/sandbox/pitot_test.cc)
target_link_libraries(pitot_test autopilot)

	
