cmake_minimum_required(VERSION 3.30.0)
set( CMAKE_CXX_COMPILER "g++" )
project(cleopatra
    VERSION 0.1.0
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23)
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
