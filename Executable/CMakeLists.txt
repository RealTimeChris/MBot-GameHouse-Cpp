include(GenerateProductVersion)
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

add_executable(
	MBot-GameHouse
	"../main.cpp"
	"${PRODUCT_INFO}"
)

if (DEFINED ENV{EnableASAN})
	if(MSVC)
		target_compile_options(MBot-GameHouse PUBLIC "/fsanitize=address" "/Zi" "/Debug")
		target_link_options(MBot-GameHouse PUBLIC "/incremental:no" "/Debug")	
	else()
		target_compile_options(MBot-GameHouse PUBLIC "-fsanitize=address")
		target_link_options(MBot-GameHouse PUBLIC "-fsanitize=address")
	endif()
endif()

find_package(DiscordCoreAPI CONFIG REQUIRED)

target_link_libraries(
	MBot-GameHouse PUBLIC
	DiscordCoreAPI::DiscordCoreAPI
)

set_target_properties(
	MBot-GameHouse PROPERTIES 
	OUTPUT_NAME "MBot-GameHouse-Cpp"
	VS_GLOBAL_VcpkgEnabled true
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
)

if (WIN32 AND NOT EXISTS "${VCPKG_ROOT}")
	add_custom_command(
		TARGET MBot-GameHouse POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_RUNTIME_DLLS:MBot-GameHouse>" "$<TARGET_FILE_DIR:MBot-GameHouse>"
		COMMAND_EXPAND_LISTS
	)
	add_custom_command(
		TARGET MBot-GameHouse POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_PATH},${RELEASE_PDB_FILE_PATH}>" "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>/$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_NAME},${RELEASE_PDB_FILE_NAME}>"
	)
endif()
