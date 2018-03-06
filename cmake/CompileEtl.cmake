# Helper CMake script to compile ETL library sources
# ETL should be added as a submodule into project, into top-level directory of project
# Author: akowalew

# Find all cpp files in ETL src directory
file(GLOB etl_SOURCES
    "${CMAKE_SOURCE_DIR}/etl/src/*.cpp"
)

message(STATUS "etl_SOURCES=${etl_SOURCES}")

# Specify compile flags without exceptions and any unwind code
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-Wall \
-Wno-error \
-Wno-unused-parameter \
-Wno-old-style-declaration \
-Wno-shift-negative-value \
-Wno-sign-compare \
-Wextra \
-ffunction-sections \
-fdata-sections \
-fno-exceptions \
-fno-unwind-tables \
-fno-asynchronous-unwind-tables \
")

# Remove unused sections from library
set(CMAKE_EXE_LINKER_FLAGS "\
-Wl,--gc-sections \
")

# Add library target
add_library(etl
	${etl_SOURCES}
)

set_target_properties(etl PROPERTIES C_STANDARD 11)
set_target_properties(etl PROPERTIES C_EXTENSIONS 11)
