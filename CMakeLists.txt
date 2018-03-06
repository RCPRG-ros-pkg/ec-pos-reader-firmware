# CMakeLists.txt
# Top-level CMakeLists, project configuration
# Author: akowalew

cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(vsa-encoders-ecat-firmware)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

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

# THIS PROJECT
# configuration: sources, flags, standard
set(CMAKE_CXX_FLAGS_DEBUG "-g -Os")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-Wall \
-Werror \
-Wextra \
-ffunction-sections \
-fdata-sections \
-fno-rtti \
-fno-exceptions \
-fno-unwind-tables \
-fno-asynchronous-unwind-tables \
-fno-threadsafe-statics \
--specs=nano.specs \
")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-Wall \
-Werror \
-Wextra \
-ffunction-sections \
-fdata-sections \
-fno-exceptions \
-fno-unwind-tables \
-fno-asynchronous-unwind-tables \
--specs=nano.specs \
")


set(LD_SCRIPT "${CMAKE_SOURCE_DIR}/src/tm4c123gh6pm.ld")
set(CMAKE_EXE_LINKER_FLAGS "\
-Wl,--gc-sections \
-Wl,-Map=main.map \
-Wl,-T ${LD_SCRIPT} \
")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
include_directories(include/abcc_abp)
include_directories(include/abcc_adapt)
include_directories(include/abcc_drv)
include_directories(include/abcc_drv/par)
include_directories(include/abcc_drv/par30)
include_directories(include/abcc_drv/serial)
include_directories(include/abcc_drv/spi)
include_directories(include/abcc_obj)
include_directories(include/abcc_obj/nw_obj)
include_directories(include/appl)
add_subdirectory(src)
