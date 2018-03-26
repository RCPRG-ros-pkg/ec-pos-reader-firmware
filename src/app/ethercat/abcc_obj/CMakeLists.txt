add_subdirectory(nw_obj)

add_library(abcc_obj
	ad_obj.c
	app_obj.c
	etn_obj.c
	opcua_obj.c
	safe_obj.c
	sync_obj.c
)

target_link_libraries(abcc_obj
	abcc_obj_nw_obj
)
