cmake_minimum_required(VERSION 3.22)
# Set this value...
set(VCPKG_BASE_DIR "C:/vcpkg")
# Or this value
set(CMAKE_CONFIG_FILE_PATH "")

set(CMAKE_CONFIGURATION_TYPES "$CACHE{CMAKE_BUILD_TYPE}")

if (VCPKG_BASE_DIR)
	string(TOLOWER "${CMAKE_HOST_SYSTEM_NAME}" OS_NAME)
	set(VCPKG_INSTALLED_DIR_NEW "${VCPKG_BASE_DIR}/installed/x64-${OS_NAME}")
elseif (CMAKE_CONFIG_FILE_PATH)
	set(DiscordCoreAPI_DIR ${CMAKE_CONFIG_FILE_PATH})
else()
	message(FATAL_ERROR "Please, set the VCPKG base directory, or the location of the DiscordCoreAPIConfig.cmake file!")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(GenerateProductVersion)

generate_product_version(
   VERSION_FILES_OUTPUT_VARIABLE
   NAME "MBot-GameHouse-Cpp"
   ICON "${CMAKE_SOURCE_DIR}/Chris 02.ico"
   ORIGINAL_FILENAME "MBot-GameHouse-Cpp.exe"
   VERSION_MAJOR 1
   VERSION_MINOR 0
   VERSION_PATCH 0
   VERSION_REVISION 0
)

project(MBot-GameHouse LANGUAGES CXX)

add_executable(
	MBot-GameHouse
	"main.cpp"
	"${VERSION_FILES_OUTPUT_VARIABLE}"
)

find_package(DiscordCoreAPI CONFIG REQUIRED)

target_link_libraries(
	MBot-GameHouse PUBLIC
	DiscordCoreAPI::DiscordCoreAPI
	"${VCPKG_INSTALLED_DIR_NEW}/lib/mongoc-1.0.lib"
	"${VCPKG_INSTALLED_DIR_NEW}/lib/mongocxx.lib"
	"${VCPKG_INSTALLED_DIR_NEW}/lib/bsoncxx.lib"
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

if (WIN32)
	add_custom_command(TARGET MBot-GameHouse POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:MBot-GameHouse> $<TARGET_FILE_DIR:MBot-GameHouse>
	  COMMAND_EXPAND_LISTS
	)
endif()
