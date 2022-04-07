function(findlibz RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	if (UNIX)
		find_library(LIBZ_RELEASE_LIBRARY NAME "${LIBRARY_PREFIX}z${LIBRARY_SUFFIX}" PATHS "${RELEASE_ROOT_DIR}")
		find_library(LIBZ_DEBUG_LIBRARY NAME "${LIBRARY_PREFIX}z${LIBRARY_SUFFIX}" PATHS "${DEBUG_ROOT_DIR}")
		if (EXISTS "${LIBZ_RELEASE_LIBRARY}" AND EXISTS "${LIBZ_DEBUG_LIBRARY}")
			add_library(Libz UNKNOWN IMPORTED GLOBAL)
            add_library(LIBZ::Libz ALIAS Libz)
            set_target_properties(Libz PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                IMPORTED_LOCATION "${LIBZ_RELEASE_LIBRARY}")
            set_property(TARGET Libz APPEND PROPERTY
                IMPORTED_CONFIGURATIONS RELEASE)
            set_target_properties(Libz PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
                IMPORTED_LOCATION_RELEASE "${LIBZ_RELEASE_LIBRARY}")
            set_property(TARGET Libz APPEND PROPERTY
                IMPORTED_CONFIGURATIONS DEBUG)
            set_target_properties(Libz PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
                IMPORTED_LOCATION_DEBUG "${LIBZ_DEBUG_LIBRARY}") 
			message(STATUS "Found Libz!")
		else()
			message(FATAL_ERROR "Couldn't find Libz!")
		endif()
	endif()
endfunction()