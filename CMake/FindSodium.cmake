function(findsodium RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	find_library(SODIUM_RELEASE_LIBRARY NAME "libsodium${LIBRARY_SUFFIX}" PATHS "${RELEASE_ROOT_DIR}")
	find_library(SODIUM_DEBUG_LIBRARY NAME "libsodium${LIBRARY_SUFFIX}" PATHS "${DEBUG_ROOT_DIR}")
	if (EXISTS "${SODIUM_RELEASE_LIBRARY}" AND EXISTS "${SODIUM_DEBUG_LIBRARY}")
		add_library(Sodium UNKNOWN IMPORTED GLOBAL)
        add_library(SODIUM::Sodium ALIAS Sodium)
        set_target_properties(Sodium PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${SODIUM_RELEASE_LIBRARY}")
        set_property(TARGET Sodium APPEND PROPERTY
            IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(Sodium PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
            IMPORTED_LOCATION_RELEASE "${SODIUM_RELEASE_LIBRARY}")
        set_property(TARGET Sodium APPEND PROPERTY
            IMPORTED_CONFIGURATIONS DEBUG)
        set_target_properties(Sodium PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
            IMPORTED_LOCATION_DEBUG "${SODIUM_DEBUG_LIBRARY}")
		message(STATUS "Found Sodium!")
	else()
		message(FATAL_ERROR "Couldn't find Sodium!")
	endif()
endfunction()