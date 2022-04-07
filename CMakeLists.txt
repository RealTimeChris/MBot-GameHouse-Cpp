cmake_minimum_required (VERSION 3.22)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(FindVCPKGRootDir)
include(FindCurl)
include(FindFFMPEG)
include(FindGlib)
include(FindLibz)
include(FindOpenSSL)
include(FindOpus)
include(FindSodium)

find_vcpkg_root_dir()

set(THE_TOOLCHAIN_PATH "$ENV{VCPKG_ROOT_DIR}/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_TOOLCHAIN_FILE "${THE_TOOLCHAIN_PATH}" CACHE PATH "The toolchain file.")
set(ENV{CMAKE_TOOLCHAIN_FILE} "${THE_TOOLCHAIN_PATH}")

project(MBot-GameHouse LANGUAGES CXX)

include(generate_product_version)

generate_product_version(
   VERSION_FILES_OUTPUT_VARIABLE
   NAME "MBot-GameHouse-Cpp"
   ICON "${CMAKE_SOURCE_DIR}/Chris 02.ico"
   COMPANY_NAME "RealTimeChris Inc."
   VERSION_MAJOR 1
   VERSION_MINOR 0
   VERSION_PATCH 0
   VERSION_REVISION 0
)

find_package(DiscordCoreAPI CONFIG REQUIRED)

if(UNIX)
	set(LIBRARY_SUFFIX ".a")
	set(LIBRARY_PREFIX "lib")
elseif(WIN32)
	set(LIBRARY_SUFFIX ".lib")
	set(LIBRARY_PREFIX "")
endif()

string(TOLOWER "${CMAKE_HOST_SYSTEM_NAME}" OS_NAME)
set(VCPKG_INSTALLED_DIR "$ENV{VCPKG_ROOT_DIR}/installed/x64-${OS_NAME}/")
message("THE PATH ${VCPKG_INSTALLED_DIR}")

findcurl("${VCPKG_INSTALLED_DIR}/lib/" "${VCPKG_INSTALLED_DIR}/debug/lib/")
findffmpeg("${VCPKG_INSTALLED_DIR}/lib/" "${VCPKG_INSTALLED_DIR}/debug/lib/")
findglib("${VCPKG_INSTALLED_DIR}/lib/" "${VCPKG_INSTALLED_DIR}/debug/lib/")
findlibz("${VCPKG_INSTALLED_DIR}/lib/" "${VCPKG_INSTALLED_DIR}/debug/lib/")
findopenssl("${VCPKG_INSTALLED_DIR}/lib/" "${VCPKG_INSTALLED_DIR}/debug/lib/")
findopus("${VCPKG_INSTALLED_DIR}/lib/" "${VCPKG_INSTALLED_DIR}/debug/lib/")
findsodium("${VCPKG_INSTALLED_DIR}/lib/" "${VCPKG_INSTALLED_DIR}/debug/lib/")

add_executable (
	MBot-GameHouse
	"main.cpp"
	"Commands/CommandsList.hpp"
	"${VERSION_FILES_OUTPUT_VARIABLE}"
)

target_compile_features(
	MBot-GameHouse PRIVATE
	"$<$<PLATFORM_ID:Windows>:cxx_std_23>"
	"$<$<PLATFORM_ID:Linux>:cxx_std_20>"
)

target_include_directories(
	MBot-GameHouse PUBLIC 
	"$ENV{VCPKG_ROOT_DIR}/installed/x64-${OS_NAME}/include"
)

target_compile_options(
	MBot-GameHouse PUBLIC
	"$<$<PLATFORM_ID:Windows>:$<$<CONFIG:Release>:/O2>>"
	"$<$<PLATFORM_ID:Windows>:$<$<CONFIG:Debug>:/MDd>>"
	"$<$<PLATFORM_ID:Linux>:-std=c++2a>"
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
	"${VCPKG_INSTALLED_DIR}lib/mongoc-1.0.lib"
	"${VCPKG_INSTALLED_DIR}lib/mongocxx.lib"
	"${VCPKG_INSTALLED_DIR}lib/bsoncxx.lib"
)

set_target_properties(
	MBot-GameHouse PROPERTIES 
	OUTPUT_NAME "MBot-GameHouse-Cpp"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
	PDB_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/$<PLATFORM_ID>/$<CONFIG>"
)

