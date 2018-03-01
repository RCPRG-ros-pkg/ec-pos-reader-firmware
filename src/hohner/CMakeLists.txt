add_library(hohner
	SMRS59.cpp
)

target_link_libraries(hohner
	device
	etl
)
