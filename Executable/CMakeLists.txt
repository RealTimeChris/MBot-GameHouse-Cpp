include(GenerateProductVersion)
generate_product_version(
	PRODUCT_INFO
	NAME "MBot-Janny-Cpp"
	ICON "${CMAKE_SOURCE_DIR}/Chris 03.ico"
	ORIGINAL_FILENAME "MBot-Janny-Cpp.exe"
	VERSION_MAJOR "1"
	VERSION_MINOR "0"
	VERSION_PATCH "0"
	VERSION_REVISION "0"
)

add_executable(
	MBot-Janny
	"../main.cpp"
	"${PRODUCT_INFO}"
)

if (DEFINED ENV{EnableASAN})
	if(MSVC)
		target_compile_options(MBot-Janny PUBLIC "/fsanitize=address" "/Zi" "/Debug")
		target_link_options(MBot-Janny PUBLIC "/incremental:no" "/Debug")	
	else()
		target_compile_options(MBot-Janny PUBLIC "-fsanitize=address")
		target_link_options(MBot-Janny PUBLIC "-fsanitize=address")
	endif()
endif()

find_package(DiscordCoreAPI CONFIG REQUIRED)

target_link_libraries(
	MBot-Janny PUBLIC
	DiscordCoreAPI::DiscordCoreAPI
)

set_target_properties(
	MBot-Janny PROPERTIES 
	OUTPUT_NAME "MBot-Janny-Cpp"
	VS_GLOBAL_VcpkgEnabled true
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}"
)

if (WIN32 AND NOT EXISTS "${VCPKG_ROOT}")
	add_custom_command(
		TARGET MBot-Janny PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_PATH},${RELEASE_PDB_FILE_PATH}>" "${CMAKE_INSTALL_PREFIX}"
	)
endif()
