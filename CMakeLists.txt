# CMakeLists.txt
# Top-level CMakeLists, project configuration
# Author: akowalew

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(mw1001c-ethercat-firmware)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

# # etl submodule
# set(ETL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/etl")
# configure_file(${ETL_PATH}/src/profiles/gcc_generic.h
# 	${CMAKE_CURRENT_BINARY_DIR}/include/etl_profile.h
# 	COPYONLY)
# include_directories(
# 	${ETL_PATH}/src
# 	${CMAKE_CURRENT_BINARY_DIR}/include)

# # embxx submodule
# # add_subdirectory(embxx)
# include_directories(embxx)

# tivaware-cmake submodule
add_subdirectory(tivaware-cmake)
include_directories(tivaware-cmake/include)

include_directories(include)
add_subdirectory(src)
