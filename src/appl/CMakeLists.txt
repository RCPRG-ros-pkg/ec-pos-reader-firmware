add_library(appl
	appl_abcc_handler.c
	appl_adimap_alltypes.c
	appl_adimap_separate16.c
	appl_adimap_simple16.c
	appl_adimap_speed_example.c
	appl_adimap_sync.c
	appl_adimap_verif.c
)

target_link_libraries(appl
	abcc_drv
	abcc_obj
)
