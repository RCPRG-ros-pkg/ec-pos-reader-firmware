add_library(device
	OutputPin.cpp
	SSIMaster.cpp
)

target_link_libraries(device
	tivaware_driverlib
)
