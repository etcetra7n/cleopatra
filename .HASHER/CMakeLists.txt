cmake_minimum_required(VERSION 3.31.0)

project(bcrypt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

add_library(bcrypt src/bcrypt.cpp src/blowfish.cpp)
target_include_directories(bcrypt PRIVATE src/)
target_include_directories(bcrypt PUBLIC include/)

add_executable(.HASHER hasher/main.cpp)
find_package(unofficial-argon2 CONFIG REQUIRED)
target_link_libraries(.HASHER PRIVATE 
    bcrypt
    unofficial::argon2::libargon2
)