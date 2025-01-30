cmake_minimum_required(VERSION 3.31.0)
project(cleopatra
    VERSION 1.0.0
    LANGUAGES CXX
)
#############################
set(CMAKE_BUILD_TYPE Release)
#############################
set(CMAKE_CXX_STANDARD 20)
set(SOURCES
    daemon/daemon.cpp
)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(include)
add_executable(daemon WIN32 ${SOURCES} com.cleopatra.daemon.rc)
target_link_libraries(daemon curl)
install(
    TARGETS daemon
    RUNTIME DESTINATION bin
)
