add_subdirectory(abcc_adapt)
# add_subdirectory(abcc_appl)
add_subdirectory(abcc_drv)
add_subdirectory(abcc_obj)

add_library(app_ethercat
	EtherCAT.cpp
)

target_link_libraries(app_ethercat
	abcc_adapt
	# abcc_appl
	abcc_drv
	abcc_obj
)
