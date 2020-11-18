cmake_minimum_required(VERSION 2.8)
project(twmailer)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#########
# Files #
#########

# Declare include files
include_directories(
  include
)

# Declare src files
file(GLOB SOURCES "src/**/*.cpp")

find_package (Threads)

add_executable(server src/server.cpp)
add_executable(client src/client.cpp)
target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})