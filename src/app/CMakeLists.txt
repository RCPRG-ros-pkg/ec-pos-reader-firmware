add_subdirectory(blinker)
add_subdirectory(encoders)
add_subdirectory(ethercat)

add_library(app
	Application.cpp
)

target_link_libraries(app
	app_blinker
	# app_encoders
	app_ethercat

	etl
	device
)
