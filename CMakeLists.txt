cmake_minimum_required(VERSION 3.31.0)
project(cleopatra
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    daemon/cleopatra.cpp
)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
include_directories(include)
add_executable(cleopatra ${SOURCES})
target_link_libraries(cleopatra curl)
install(
    TARGETS cleopatra
    RUNTIME DESTINATION bin
)
