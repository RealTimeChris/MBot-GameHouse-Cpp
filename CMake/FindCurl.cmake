function(findcurl RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	find_library(CURL_RELEASE_LIBRARY NAME "libcurl${LIBRARY_SUFFIX}" PATHS "${RELEASE_ROOT_DIR}")
	find_library(CURL_DEBUG_LIBRARY NAME "libcurl-d${LIBRARY_SUFFIX}" PATHS "${DEBUG_ROOT_DIR}")
	if (EXISTS "${CURL_RELEASE_LIBRARY}" AND EXISTS "${CURL_DEBUG_LIBRARY}")
		add_library(Curl UNKNOWN IMPORTED GLOBAL)
        add_library(CURL::Curl ALIAS Curl)
        set_target_properties(Curl PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${CURL_RELEASE_LIBRARY}")
        set_property(TARGET Curl APPEND PROPERTY
            IMPORTED_CONFIGURATIONS RELEASE)
            set_target_properties(Curl PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
            IMPORTED_LOCATION_RELEASE "${CURL_RELEASE_LIBRARY}")
        set_property(TARGET Curl APPEND PROPERTY
            IMPORTED_CONFIGURATIONS DEBUG)
            set_target_properties(Curl PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
            IMPORTED_LOCATION_DEBUG "${CURL_DEBUG_LIBRARY}")
		message(STATUS "Found Curl!")
	else()
		message(FATAL_ERROR "Couldn't find Curl!")
	endif()
endfunction()