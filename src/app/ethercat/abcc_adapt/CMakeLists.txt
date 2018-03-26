add_library(abcc_adapt
	abcc_sys_adapt.c
)

target_link_libraries(abcc_adapt
	tivaware_driverlib
)
