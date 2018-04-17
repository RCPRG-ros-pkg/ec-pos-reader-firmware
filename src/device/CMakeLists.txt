add_library(device
	GPIO.cpp
	GPTM.cpp
	SSIMaster.cpp
)

target_link_libraries(device
	tivaware_driverlib
)
