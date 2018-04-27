add_library(device
	OutputPin.cpp
)

target_link_libraries(device
	tivaware_driverlib
)
