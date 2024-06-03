cmake_minimum_required(VERSION 3.20)
set(EXTLIB teldummy)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(${EXTLIB} REQUIRED usefull_macros)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
include_directories(${${EXTLIB}_INCLUDE_DIRS} ..)
link_directories(${${EXTLIB}_LIBRARY_DIRS})

add_library(${EXTLIB} SHARED ${SRC})
target_link_libraries(${EXTLIB} ${${EXTLIB}_LIBRARIES} -fPIC)
install(TARGETS ${EXTLIB} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
