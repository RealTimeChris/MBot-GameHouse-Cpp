cmake_minimum_required (VERSION 3.8)

project (MBot-GameHouse-Cpp LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
include(generate_product_version)

generate_product_version(
   VersionFilesOutputVariable
   NAME "MBot-GameHouse-Cpp"
   ICON "${CMAKE_SOURCE_DIR}/Chris 02.ico"
   FILE_DESCRIPTION "A Discord bot written in C++, using asynchronous coroutines."
   COMPANY_NAME "RealTimeChris Inc."
   VERSION_MAJOR 1
   VERSION_MINOR 0
   VERSION_PATCH 0
   VERSION_REVISION 0
)

find_package(DiscordCoreAPI CONFIG REQUIRED)

add_executable (MBot-GameHouse "main.cpp" "Commands/CommandsList.hpp" "${VersionFilesOutputVariable}")

target_link_libraries(MBot-GameHouse PRIVATE DiscordCoreAPI::DiscordCoreAPI "C:/vcpkg/installed/x64-windows/lib/mongoc-1.0.lib" "C:/vcpkg/installed/x64-windows/lib/mongocxx.lib" "C:/vcpkg/installed/x64-windows/lib/bsoncxx.lib")

set_target_properties(MBot-GameHouse PROPERTIES 
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
	PDB_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
)