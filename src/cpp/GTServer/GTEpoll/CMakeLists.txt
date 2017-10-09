# add cmake version control
cmake_minimum_required(VERSION 2.8)

# set build type and options
set(CMAKE_BUILD_TYPE 'DEBUG')
set(CMAKE_CXX_FLAGS_DEBUG '$ENV{CXXFLAGS} -O0 -Wall -g -ggdb')
set(CMAKE_CXX_FLAGS_RELEASE '$ENV{CXXFLAGS} -O2 -Wall)


# project info
project(GTEpoll)

# include header
include_directories(.)

# add source file
aux_source_directory(. DIR_SRC)
aux_source_directory(./GTEpollResourceControl DIR_SRC_RSRC)
aux_source_directory(./GTEpollUtility DIR_SRC_Util)

# set target
add_library(GTEpoll STATIC ${DIR_SRC} ${DIR_SRC_RSRC} ${DIR_SRC_Util})

# set output path
set(LIBRARY_OUTPUT_PATH ./build)
