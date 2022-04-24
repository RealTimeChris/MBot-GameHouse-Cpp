generate_product_version(
	PRODUCT_INFO
	NAME "MBot-GameHouse-Cpp"
	ICON "${CMAKE_SOURCE_DIR}/Chris 02.ico"
	ORIGINAL_FILENAME "MBot-GameHouse-Cpp.exe"
	VERSION_MAJOR "1"
	VERSION_MINOR "0"
	VERSION_PATCH "0"
	VERSION_REVISION "0"
)

find_curl("${CURL_RELEASE_ROOT}" "${CURL_DEBUG_ROOT}" "${CURL_INCLUDE_DIR}")
find_ffmpeg("${FFMPEG_RELEASE_ROOT}" "${FFMPEG_DEBUG_ROOT}" "${FFMPEG_INCLUDE_DIR}")
find_glib("${GLIB_RELEASE_ROOT}" "${GLIB_DEBUG_ROOT}" "${GLIB_INCLUDE_DIR}")
find_intl("${GLIB_RELEASE_ROOT}" "${GLIB_DEBUG_ROOT}" "${GLIB_INCLUDE_DIR}")
find_libz("${LIBZ_RELEASE_ROOT}" "${LIBZ_DEBUG_ROOT}" "${LIBZ_INCLUDE_DIR}")
find_openssl("${OPENSSL_RELEASE_ROOT}" "${OPENSSL_DEBUG_ROOT}" "${OPENSSL_INCLUDE_DIR}")
find_opus("${OPUS_RELEASE_ROOT}" "${OPUS_DEBUG_ROOT}" "${OPUS_INCLUDE_DIR}")
find_sodium("${SODIUM_RELEASE_ROOT}" "${SODIUM_DEBUG_ROOT}" "${SODIUM_INCLUDE_DIR}")
find_nlohmann_json("${NLOHMANN_INCLUDE_DIR}")

add_executable(
	MBot-GameHouse
	"../main.cpp"
	"${PRODUCT_INFO}"
)

if (DEFINED ENV{EnableASAN})
	option(ASAN_ENABLED "Build this target with AddressSanitizer" ON)

	if(ASAN_ENABLED)
		if(MSVC)
			target_compile_options(MBot-GameHouse PUBLIC /fsanitize=address /Zi)
			target_link_options(MBot-GameHouse PUBLIC /fsanitize=address /DEBUG)
		else()
			target_compile_options(MBot-GameHouse PUBLIC -fsanitize=address <additional-options>)
			add_compile_definitions(MBot-GameHouse PUBLIC -fsanitize=address)
		endif()
	endif()
endif()

find_package(DiscordCoreAPI CONFIG REQUIRED)

target_link_libraries(
	MBot-GameHouse PUBLIC
	DiscordCoreAPI::DiscordCoreAPI
	"${VCPKG_INSTALLED_DIR}/lib/mongoc-1.0${LIBRARY_SUFFIX}"
	"${VCPKG_INSTALLED_DIR}/lib/mongocxx${LIBRARY_SUFFIX}"
	"${VCPKG_INSTALLED_DIR}/lib/bsoncxx${LIBRARY_SUFFIX}"
)

set_target_properties(
	MBot-GameHouse PROPERTIES 
	OUTPUT_NAME "MBot-GameHouse-Cpp"
	VS_GLOBAL_VcpkgEnabled true
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
	PDB_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
)

if (WIN32 AND NOT VCPKG_ROOT)
	add_custom_command(
		TARGET MBot-GameHouse POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:MBot-GameHouse> $<TARGET_FILE_DIR:MBot-GameHouse>
		COMMAND_EXPAND_LISTS
	)
endif()
