add_subdirectory(par)
add_subdirectory(par30)
add_subdirectory(serial)
add_subdirectory(spi)

add_library(abcc_drv
	abcc_cmd_seq.c
	abcc_debug_err.c
	abcc_handler.c
	abcc_link.c
	abcc_mem.c
	abcc_remap.c
	abcc_setup.c
	abcc_timer.c
)

target_link_libraries(abcc_drv
	abcc_adapt

	abcc_drv_par
	abcc_drv_par30
	abcc_drv_serial
	abcc_drv_spi
)
