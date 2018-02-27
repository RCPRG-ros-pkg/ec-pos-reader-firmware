# Helper CMake script to compile ETL library sources
# ETL should be added as a submodule into project, into top-level directory of project
# Author: akowalew

# Find all cpp files in ETL src directory
file(GLOB etl_SOURCES
    "${CMAKE_SOURCE_DIR}/etl/src/*.cpp"
)

message(STATUS "etl_SOURCES=${etl_SOURCES}")

# Specify compile flags without exceptions and any unwind code
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-std=gnu++17 \
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
")

# Remove unused sections from library
set(CMAKE_EXE_LINKER_FLAGS "\
-Wl,--gc-sections \
")

# Add library target
add_library(etl
	${etl_SOURCES}
)
