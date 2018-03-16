add_library(dev
	RTCDevice.cpp
	SSIMasterDevice.cpp
	SysTickDevice.cpp
)

target_link_libraries(dev
	tivaware_driverlib
)
