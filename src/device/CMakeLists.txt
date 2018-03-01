add_library(device
	SSIMasterDevice.cpp
)

target_link_libraries(device
	tivaware_driverlib
)
