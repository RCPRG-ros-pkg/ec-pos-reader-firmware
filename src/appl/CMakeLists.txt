add_library(appl
	appl_abcc_handler.c
	appl_adimap_alltypes.c
	appl_adimap_speed_example.c
	appl_adimap_sync.c
)

target_link_libraries(appl
	abcc_drv
	abcc_obj
)
