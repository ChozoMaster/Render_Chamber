SUBDIRLIST(subdirectories ${CMAKE_CURRENT_SOURCE_DIR})
foreach(dir ${subdirectories})
	add_subdirectory(${dir})
endforeach()
