cmake_minimum_required(VERSION 3.1.0)
project(cleopatra
    VERSION 0.1.0
    LANGUAGES CXX
)
set( CMAKE_CXX_COMPILER "g++" )

set(CMAKE_CXX_STANDARD 14)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14 /W4")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wpedantic -mwindows")
endif()
find_package(CURL REQUIRED)


set(HEADERS

)

set(SOURCES
    daemon/cleopatra.cpp
)

add_executable(cleopatra ${HEADERS} ${SOURCES})

target_link_libraries(cleopatra PRIVATE CURL::libcurl)

install(
    TARGETS cleopatra
    RUNTIME DESTINATION bin
)
