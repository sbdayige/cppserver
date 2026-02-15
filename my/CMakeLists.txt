cmake_minimum_required(VERSION 3.5)
project(cppserver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable common warnings and debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

add_definitions(-D_GNU_SOURCE)

# Export compile commands for IDEs (IntelliSense)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

# Project layout
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Put binaries in the build/bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Build helper library for tools
add_library(tools STATIC ${SRC_DIR}/tools/tools.cpp)
# expose tools headers (tools/tools.h) to consumers
target_include_directories(tools PUBLIC ${SRC_DIR} ${SRC_DIR}/tools)

# Core/achieve sources (Socket, InetAddress, Epoll, ...)
file(GLOB CORE_SOURCES ${SRC_DIR}/achieve/*.cpp)
add_library(core STATIC ${CORE_SOURCES})
target_include_directories(core PUBLIC ${SRC_DIR} ${SRC_DIR}/include ${SRC_DIR}/achieve)
target_compile_options(core PRIVATE -Wall -Wextra)

add_executable(client ${SRC_DIR}/client.cpp)
target_compile_options(client PRIVATE -Wall -Wextra)
target_include_directories(client PRIVATE ${SRC_DIR} ${SRC_DIR}/tools ${SRC_DIR}/include)
target_link_libraries(client PRIVATE Threads::Threads)

add_executable(server ${SRC_DIR}/server.cpp)
target_compile_options(server PRIVATE -Wall -Wextra)
target_include_directories(server PRIVATE ${SRC_DIR} ${SRC_DIR}/tools ${SRC_DIR}/include)
target_link_libraries(server PRIVATE core tools Threads::Threads)

message(STATUS "Configuring cppserver (client + server) in ${CMAKE_CURRENT_SOURCE_DIR}")
