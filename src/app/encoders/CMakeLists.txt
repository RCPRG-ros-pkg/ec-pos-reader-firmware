add_library(app_encoders
	Encoders.cpp
)

target_link_libraries(app_encoders
	device
	etl
)
