# Helper CMake script to compile ETL library sources
# ETL should be added as a submodule into project, into top-level directory of project
# Author: akowalew

# Find all cpp files in ETL src directory
file(GLOB etl_SOURCES
    "${CMAKE_SOURCE_DIR}/etl/src/*.cpp"
)

message(STATUS "etl_SOURCES=${etl_SOURCES}")

# Add library target
add_library(etl
	${etl_SOURCES}
)

set_target_properties(etl PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS OFF
	LINK_FLAGS "-Wl,--gc-sections"
	COMPILE_FLAGS "-Wall -Werror -Wextra -ffunction-sections -fno-exceptions -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-threadsafe-statics"
)
