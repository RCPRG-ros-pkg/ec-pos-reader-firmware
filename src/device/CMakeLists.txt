add_library(device
	GPIO.cpp
	# RTC.cpp
	SSIMaster.cpp
	SysTick.cpp
)

target_link_libraries(device
	tivaware_driverlib
)
