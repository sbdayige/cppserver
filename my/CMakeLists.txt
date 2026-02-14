cmake_minimum_required(VERSION 3.5)
project(cppserver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable common warnings and debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

add_definitions(-D_GNU_SOURCE)

find_package(Threads REQUIRED)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Put binaries in the build/bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Build helper library for tools
add_library(tools STATIC ${SRC_DIR}/tools/tools.cpp)
target_include_directories(tools PUBLIC ${SRC_DIR})

add_executable(client ${SRC_DIR}/client.cpp)
target_compile_options(client PRIVATE -Wall -Wextra)
target_include_directories(client PRIVATE ${SRC_DIR})
target_link_libraries(client PRIVATE Threads::Threads)

add_executable(server ${SRC_DIR}/server.cpp)
target_compile_options(server PRIVATE -Wall -Wextra)
target_include_directories(server PRIVATE ${SRC_DIR})
target_link_libraries(server PRIVATE tools Threads::Threads)

message(STATUS "Configuring cppserver (client + server) in ${CMAKE_CURRENT_SOURCE_DIR}")
