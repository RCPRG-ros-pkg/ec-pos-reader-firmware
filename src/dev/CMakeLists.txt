add_library(dev
	SSIMasterDevice.cpp
	SysTickDevice.cpp
)

target_link_libraries(dev
	tivaware_driverlib
)
