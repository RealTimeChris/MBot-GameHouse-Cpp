cmake_minimum_required(VERSION 3.22)
# Set this value...
set(VCPKG_ROOT_DIR "C:/vcpkg")
# Or this value
set(CMAKE_CONFIG_FILE_PATH "")

set(CMAKE_CONFIGURATION_TYPES "$CACHE{CMAKE_BUILD_TYPE}")

if (WIN32)
	set(LIBRARY_SUFFIX ".lib")
elseif(UNIX)
	set(LIBRARY_SUFFIX ".a")
endif()	

if (VCPKG_ROOT_DIR)
	if (WIN32)
		include("${VCPKG_ROOT_DIR}/scripts/buildsystems/vcpkg.cmake")
	elseif(UNIX)
		if(DEFINED VCPKG_ROOT_DIR AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
			set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "The CMake toolchain file.")
			message(STATUS "Found CMake toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
		endif()
	endif()	
	string(TOLOWER "${CMAKE_HOST_SYSTEM_NAME}" OS_NAME)
	set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT_DIR}/installed/x64-${OS_NAME}")
elseif (CMAKE_CONFIG_FILE_PATH)
	set(DiscordCoreAPI_DIR "${CMAKE_CONFIG_FILE_PATH}")
else()
	message(FATAL_ERROR "Please, set the VCPKG base directory, or the location of the DiscordCoreAPIConfig.cmake file!")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

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

project(MBot-GameHouse LANGUAGES CXX)

add_executable(
	MBot-GameHouse
	"main.cpp"
	"${PRODUCT_INFO}"
)

if (DEFINED ENV{EnableASAN})
	option(ASAN_ENABLED "Build this target with AddressSanitizer" ON)

	if(ASAN_ENABLED)
		if(MSVC)
			target_compile_options(DiscordCoreAPI-Bot PUBLIC /fsanitize=address /Zi)
			target_link_options(DiscordCoreAPI-Bot PUBLIC /fsanitize=address /DEBUG)
		else()
			target_compile_options(DiscordCoreAPI-Bot PUBLIC -fsanitize=address <additional-options>)
			add_compile_definitions(DiscordCoreAPI-Bot PUBLIC -fsanitize=address)
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
