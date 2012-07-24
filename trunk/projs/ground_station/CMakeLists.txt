
##
## The library
##

include_directories(src)
include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LIBDS_INCLUDE})

set(GS_SRC src/map_stream_view.cc
		   src/3d_stream_view.cc 		
		   src/size_stream_view.cc		
		   src/size_push_gen.cc		
		   src/orientation_stream_view.cc 
		   src/external_widgets/orientation_widget.cc 
		   src/external_widgets/heading_widget.cc)

set(GS_HEADERS src/map_stream_view.h
			   src/3d_stream_view.h
			   src/size_push_gen.h
			   src/orientation_stream_view.h)

# qgis stuff
include_directories(${QT_QTXML_INCLUDE_DIR})
include_directories(/usr/include/qgis/ /usr/include/gdal/)
add_definitions(-DGUI_EXPORT= -DCORE_EXPORT=)
set (QGIS_LIBRARIES qgis_core qgis_gui geos gdal1.7.0)

# qt stuff
qt4_wrap_cpp(GS_HEADERS_MOC ${GS_HEADERS})
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_definitions(-DMEDIA_DIR=\"${CMAKE_SOURCE_DIR}/ground_station/data\")

add_library(ground_station SHARED ${GS_SRC} ${GS_HEADERS_MOC})
target_link_libraries(ground_station ${LIBDS_LIB} ${QT_LIBRARIES} ${QGIS_LIBRARIES} Irrlicht ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

##
## The executables
##

add_executable(sample_gs src/main.cc)
target_link_libraries(sample_gs ground_station)

##
## For those who want to work with it
##
set(GS_INCLUDE 	${CMAKE_SOURCE_DIR}/ground_station/include 
				${LIBDS_INCLUDE}				
				${QT_INCLUDES}
				${QT_QTXML_INCLUDE_DIR}
				/usr/include/qgis/ 
				/usr/include/gdal/
	PARENT_SCOPE)

set(GS_LIB     	${CMAKE_SOURCE_DIR}/ground_station/libground_station.so 
				${LIBDS_LIB} 	
				${QT_LIBRARIES} 
				${QGIS_LIBRARIES} 
				${Boost_FILESYSTEM_LIBRARY} 
				${Boost_SYSTEM_LIBRARY}
				Irrlicht 
				qgis_core qgis_gui geos gdal1.7.0
	PARENT_SCOPE)

set(GS_DEFINITIONS -DGUI_EXPORT= -DCORE_EXPORT= ${QT_DEFINITIONS} 
	PARENT_SCOPE)

