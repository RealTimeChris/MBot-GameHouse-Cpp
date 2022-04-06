function(find_vcpkg_root_dir)
	cmake_path(GET CMAKE_SOURCE_DIR ROOT_PATH ROOT_PATH_NEW)
	message(STATUS "Locating the VCPKG root directory...")
	if (WIN32)
		set(EXECUTABLE_SUFFIX ".exe")
	endif()
	file(GLOB_RECURSE OUTPUT_LIST LIST_DIRECTORIES false RELATIVE "${CMAKE_SOURCE_DIR}" "${ROOT_PATH_NEW}*vcpkg/vcpkg${EXECUTABLE_SUFFIX}")

	foreach(CURRENT_FILE ${OUTPUT_LIST})
		get_filename_component(VCPKG_ROOT_DIR_OLD ${CURRENT_FILE} PATH)
		set(ENV{VCPKG_ROOT_DIR} "${VCPKG_ROOT_DIR_OLD}")
	endforeach()
endfunction()
