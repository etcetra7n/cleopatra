cmake_minimum_required(VERSION 3.31.0)
project(csdr_installer
    VERSION 3.4.2
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    data.h
    csdhr_installer.cpp
)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")  # For /MT
add_executable(csdhr_installer ${SOURCES})

find_package(unofficial-minizip CONFIG REQUIRED)
find_package(unofficial-argon2 CONFIG REQUIRED)
target_link_libraries(csdhr_installer PRIVATE 
    unofficial::minizip::minizip
    unofficial::argon2::libargon2
)
set(MANIFEST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/app.manifest")
set_target_properties(csdhr_installer PROPERTIES
    LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /MANIFEST"
)
