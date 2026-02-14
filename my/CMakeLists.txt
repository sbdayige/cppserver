cmake_minimum_required(VERSION 3.5)
project(cppserver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable common warnings and debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

add_definitions(-D_GNU_SOURCE)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(client ${SRC_DIR}/client.cpp)
target_compile_options(client PRIVATE -Wall -Wextra)
target_link_libraries(client PRIVATE Threads::Threads)

add_executable(server ${SRC_DIR}/server.cpp)
target_compile_options(server PRIVATE -Wall -Wextra)
target_link_libraries(server PRIVATE Threads::Threads)

find_package(Threads REQUIRED)

message(STATUS "Configuring cppserver (client + server) in ${CMAKE_CURRENT_SOURCE_DIR}")
