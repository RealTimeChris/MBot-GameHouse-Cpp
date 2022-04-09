# Set this value...
set(VCPKG_BASE_DIR "C:/vcpkg")
# Or this value
set(CMAKE_CONFIG_FILE_PATH "")

if (EXISTS ${VCPKG_BASE_DIR})
	set(THE_TOOLCHAIN_PATH "${VCPKG_BASE_DIR}/scripts/buildsystems/vcpkg.cmake")

	set(CMAKE_TOOLCHAIN_FILE "${THE_TOOLCHAIN_PATH}" CACHE PATH "The toolchain file.")
	set(ENV{CMAKE_TOOLCHAIN_FILE} "${THE_TOOLCHAIN_PATH}")
	string(TOLOWER "${CMAKE_HOST_SYSTEM_NAME}" OS_NAME)
	set(VCPKG_INSTALLED_DIR_NEW "${VCPKG_BASE_DIR}/installed/x64-windows")
elseif (EXISTS ${CMAKE_CONFIG_FILE_PATH})
	set(DiscordCoreAPI_DIR ${CMAKE_CONFIG_FILE_PATH})
else()
	message(FATAL_ERROR "Please, set the VCPKG base directory, or the location of the DiscordCoreAPIConfig.cmake file!")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(generate_product_version)

cmake_minimum_required(VERSION 3.21.2)

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

add_executable (
	MBot-GameHouse
	"main.cpp"
	"${VERSION_FILES_OUTPUT_VARIABLE}"
)

find_package(DiscordCoreAPI CONFIG REQUIRED)

target_compile_features(
	MBot-GameHouse PUBLIC
	"$<$<PLATFORM_ID:Windows>:cxx_std_23>"
	"$<$<PLATFORM_ID:Linux>:cxx_std_20>"
)

target_compile_options(
	MBot-GameHouse PUBLIC
	"$<$<PLATFORM_ID:Windows>:$<$<CONFIG:Release>:/O2>>"
	"$<$<PLATFORM_ID:Windows>:$<$<CONFIG:Debug>:/MDd>>"
	"$<$<PLATFORM_ID:Linux>:-lpthread>"
	"$<$<PLATFORM_ID:Windows>:/bigobj>"
	"$<$<PLATFORM_ID:Windows>:/W4>"
	"$<$<PLATFORM_ID:Windows>:/WX>"
	"$<$<PLATFORM_ID:Windows>:/MP>"
	"$<$<PLATFORM_ID:Linux>:-O3>"
)

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
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
	PDB_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>"
)
