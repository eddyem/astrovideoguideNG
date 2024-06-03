cmake_minimum_required(VERSION 3.20)
set(PROJ loccorr)
set(MAJOR_VERSION "0")
set(MID_VERSION "0")
set(MINOR_VERSION "1")
set(VERSION "${MAJOR_VERSION}.${MID_VERSION}.${MINOR_VERSION}")

project(${PROJ} VERSION ${VERSION} LANGUAGES C)
message("\n\nPlease, check current version: ${VERSION}\n\n")

# list of options
# three-state DEBUG option
set(DEBUG OFF CACHE STRING "Debug mode: on/part/off (full debug, release with debug info, full release)")
set_property(CACHE DEBUG PROPERTY STRINGS PART ON OFF)
#option(DEBUG "Compile in debug mode" OFF)
option(LCDUMMY "Dummy local corrector plugin" ON)
option(TELDUMMY "Dummy telescope plugin" ON)

# here is one of two variants: all .c in directory or .c files in list
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCES)

# default flags
message("\n\nDIR: ${CMAKE_CURRENT_SOURCE_DIR}\n\n")
# fmacro-prefix-map: https://stackoverflow.com/a/53848526/1965803  - replace full path with short
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -Wextra -fno-builtin-strlen -fmacro-prefix-map=\"${CMAKE_CURRENT_SOURCE_DIR}\"/=\"\" ")

message("Check install dir prefix: ${CMAKE_INSTALL_PREFIX}")

add_definitions(-D_XOPEN_SOURCE=1234 -D_DEFAULT_SOURCE -D_GNU_SOURCE
        -DPACKAGE_VERSION=\"${VERSION}\" -DMINOR_VERSION=\"${MINOR_VERSION}\"
        -DMID_VERSION=\"${MID_VERSION}\" -DMAJOR_VERSION=\"${MAJOR_VESION}\")

set(CMAKE_COLOR_MAKEFILE ON)

# cmake -DDEBUG=on/part/off -> debug / release with debug info / release
if(DEBUG STREQUAL "PART") # part
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -fdata-sections -ffunction-sections -flto=auto")
    add_definitions(-DEBUG)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -flto=auto")
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
    set(CMAKE_VERBOSE_MAKEFILE "ON")
elseif(DEBUG) # on
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -g3 -ggdb -Werror")
    add_definitions(-DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_VERBOSE_MAKEFILE "ON")
else() # off
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -fdata-sections -ffunction-sections -flto=auto")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -flto=auto")
    set(CMAKE_BUILD_TYPE Release)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
#find_package(CFITSIO REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(${PROJ} REQUIRED usefull_macros improc ccdcapture)

include(FindOpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    add_definitions(-DOMP_FOUND)
endif()

# Dummy plugins
if(LCDUMMY)
    add_subdirectory(Dummy_loccorr)
endif()
if(TELDUMMY)
    add_subdirectory(Dummy_telescope)
endif()

# exe & lib files; install
include(GNUInstallDirs)
#add_library(${PROJLIB} SHARED ${LIBSRC})
add_executable(${PROJ} ${SOURCES})
target_link_libraries(${PROJ} ${${PROJ}_LIBRARIES} -lm ${CMAKE_DL_LIBS})
#target_link_libraries(${PROJLIB} ${CFITSIO_LIBRARIES} ${${PROJLIB}_LIBRARIES})
include_directories(${${PROJ}_INCLUDE_DIRS} .)
link_directories(${${PROJ}_LIBRARY_DIRS} )
set(PCFILE "${CMAKE_BINARY_DIR}/${PROJLIB}.pc")
#configure_file("${PROJLIB}.pc.in" ${PCFILE} @ONLY)
#set_target_properties(${PROJLIB} PROPERTIES VERSION ${VERSION})
#set_target_properties(${PROJLIB} PROPERTIES PUBLIC_HEADER ${LIBHEADER})

# Installation of the program
include(GNUInstallDirs)
install(TARGETS ${PROJ}
    #LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
#install(TARGETS ${PROJ} DESTINATION "bin")
#install(TARGETS ${PROJLIB} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#install(TARGETS PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#install(FILES ${PCFILE} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
