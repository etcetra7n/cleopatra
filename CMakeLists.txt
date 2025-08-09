cmake_minimum_required(VERSION 3.31.0)
project(cleopatra
    VERSION 3.4.2
    LANGUAGES CXX
)

#############################
set(CMAKE_CXX_STANDARD 23)
set(SOURCES
    daemon/daemon.h
    daemon/main.cpp
    daemon/utils.cpp
    daemon/runner.cpp
    daemon/logger.cpp
    daemon/send_results.cpp
    daemon/com.cleopatra.daemon.rc
)


#set(CMAKE_RC_FLAGS 
#    "/I\"C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/um\" \
#     /I\"C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/shared\"")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
include_directories("include")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    add_executable(csdhr ${SOURCES})
else()
    message(STATUS "Building in Release mode")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")  # For /MT
    add_executable(csdhr WIN32 ${SOURCES})
endif()

find_package(CURL REQUIRED)
target_link_libraries(csdhr PRIVATE 
    CURL::libcurl
)
#find_package(OpenSSL REQUIRED)
#OpenSSL::SSL
#OpenSSL::Crypto
