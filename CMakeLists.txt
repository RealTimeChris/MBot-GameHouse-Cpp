cmake_minimum_required (VERSION 3.8)

project (MBot-GameHouse-Cpp LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(generate_product_version)

set(VCPKG_BASE_DIR "C:/vcpkg")
string(TOLOWER "${CMAKE_HOST_SYSTEM_NAME}" OS_NAME)
set(VCPKG_INSTALLED_DIR "${VCPKG_BASE_DIR}/installed/x64-${OS_NAME}")

generate_product_version(
   VERSION_FILES_OUTPUT_VARIABLE
   NAME "MBot-GameHouse-Cpp"
   ICON "${CMAKE_SOURCE_DIR}/Chris 02.ico"
   FILE_DESCRIPTION "MBot-GameHouse-Cpp.exe"
   COMPANY_NAME "RealTimeChris Inc."
   VERSION_MAJOR 1
   VERSION_MINOR 0
   VERSION_PATCH 0
   VERSION_REVISION 0
)

find_package(DiscordCoreAPI CONFIG REQUIRED)

add_executable (
	MBot-GameHouse
	"main.cpp"
	"Commands/CommandsList.hpp"
	"${VERSION_FILES_OUTPUT_VARIABLE}"
)

target_compile_features(
	MBot-GameHouse PUBLIC
	"$<$<PLATFORM_ID:Linux>:cxx_std_20>"
)

target_link_libraries(
	MBot-GameHouse PRIVATE
	DiscordCoreAPI::DiscordCoreAPI
	"${VCPKG_INSTALLED_DIR}/lib/mongoc-1.0.lib"
	"${VCPKG_INSTALLED_DIR}/lib/mongocxx.lib"
	"${VCPKG_INSTALLED_DIR}/lib/bsoncxx.lib"
)

set_target_properties(
	MBot-GameHouse PROPERTIES 
	OUTPUT_NAME "MBot-GameHouse-Cpp"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
	PDB_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
)