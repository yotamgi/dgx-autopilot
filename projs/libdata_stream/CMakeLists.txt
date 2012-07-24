
##
## For those who want to work with it
##
set(LIBDS_INCLUDE ${CMAKE_SOURCE_DIR}/libdata_stream/include PARENT_SCOPE)
set(LIBDS_LIB     ${CMAKE_SOURCE_DIR}/libdata_stream/libdata_stream.so PARENT_SCOPE)


##
## The library
##

include_directories(${Boost_INCLUDE_DIRS})
include_directories(include)
include_directories(src)

set(DATA_STREAM_SRC src/stream_connection.cc 
				    src/async_stream_connection.cc 
				    src/util/tcpip_connection.cc 
				    src/util/udpip_connection.cc 
				    src/filters/gyro_to_av_matrix.cc 
				    src/filters/matrix_to_euler_filter.cc 
				    src/filters/acc_compass_rot.cc)

add_library(data_stream SHARED ${DATA_STREAM_SRC})
target_link_libraries(data_stream ${Boost_LIBRARIES})
target_link_libraries(data_stream ${ARMADILLO_LIBRARY})

##
## The tests
##
enable_testing()
include_directories(${GTEST_INCLUDE_DIRS})

set(GTEST_SRC tests/integral_test.cc 
			 tests/static_filter_test.cc 
			 tests/low_pass_test.cc 
			 tests/watch_test.cc 
			 tests/tcpip_conn_test.cc 
			 tests/udpip_conn_test.cc 
			 tests/stream_conn_test.cc 
			 tests/async_stream_conn_test.cc 
			 tests/func_filter_test.cc 
			 tests/recorder_test.cc 
			 tests/fps_test.cc)

add_executable(gtest-libds ${GTEST_SRC})
target_link_libraries(gtest-libds ${GTEST_BOTH_LIBRARIES} pthread data_stream)

#add_test(AllTestsInFoo foo)

