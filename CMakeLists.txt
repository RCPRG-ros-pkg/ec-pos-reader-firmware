# CMakeLists.txt
# Top-level CMakeLists, project configuration
# Author: akowalew

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(vsa-encoders-ecat-firmware VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Os")

# include(CheckIPOSupported)
# check_ipo_supported(RESULT supported OUTPUT error)
# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# helper cmake scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# SUBMODULES
# etl submodule
set(ETL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/etl")
configure_file(${ETL_PATH}/src/profiles/gcc_generic.h
	${CMAKE_CURRENT_BINARY_DIR}/include/etl_profile.h
	COPYONLY)
include_directories(
	${ETL_PATH}/src
	${CMAKE_CURRENT_BINARY_DIR}/include)
include(CompileEtl)

# embxx submodule
include_directories(embxx)

# tivaware-cmake submodule
add_subdirectory(tivaware-cmake)
include_directories(tivaware-cmake/include)

include_directories(include)
include_directories(include/app/ethercat/abcc_abp)
include_directories(include/app/ethercat/abcc_adapt)
include_directories(include/app/ethercat/abcc_appl)
include_directories(include/app/ethercat/abcc_drv)
include_directories(include/app/ethercat/abcc_drv/par)
include_directories(include/app/ethercat/abcc_drv/par30)
include_directories(include/app/ethercat/abcc_drv/serial)
include_directories(include/app/ethercat/abcc_drv/spi)
include_directories(include/app/ethercat/abcc_obj)
include_directories(include/app/ethercat/abcc_obj/nw_obj)
add_subdirectory(src)
