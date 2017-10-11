# add cmake version control
cmake_minimum_required(VERSION 2.8)

# set build type and options
set(CMAKE_BUILD_TYPE 'DEBUG')
set(CMAKE_CXX_FLAGS_DEBUG '$ENV{CXXFLAGS} -O0 -Wall -g -ggdb')
set(CMAKE_CXX_FLAGS_RELEASE '$ENV{CXXFLAGS} -O2 -Wall)


# project info
project(test_server)

# include header
include_directories(.)

add_executable(test_server GTEpoll_test_thunder_herd.cpp)

# set output path
set(EXECUTABLE_OUTPUT_PATH  ./build)