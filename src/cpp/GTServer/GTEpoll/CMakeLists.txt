# add cmake version control
cmake_minimum_required(VERSION 2.8)

# set build type and options
set(CMAKE_BUILD_TYPE 'DEBUG')
set(CMAKE_CXX_FLAGS_DEBUG '$ENV{CXXFLAGS} -O0 -Wall -g -ggdb')
set(CMAKE_CXX_FLAGS_RELEASE '$ENV{CXXFLAGS} -O2 -Wall)


# project info
project(GTEpoll)

# add definition
add_definitions(-LINUX)

# include header
include_directories(.)
include_directories(./GTEpollUtility/)
include_directories(./thirdparty)
include_directories(../../GTServer/GTUtlity/)

# add source files
set(UTIL_FILE_THIRDPARTY
        ../../GTServer/GTUtlity/GT_Util_CfgHelper.cpp
        ../../GTServer/GTUtlity/GT_Util_GlogWrapper.cpp
)

# add sub directory
add_subdirectory(./test/test_client ./test/test_client/build)
add_subdirectory(./test/test_server ./test/test_server/build)

# add source file
aux_source_directory(. DIR_SRC)
aux_source_directory(./GTEpollResourceControl DIR_SRC_RSRC)
aux_source_directory(./GTEpollUtility DIR_SRC_Util)

# link library
link_libraries(./thirdparty/Glog/libglog.a)
link_libraries(./thirdparty/JsonParser/libjsoncpp.a)

# set target
add_library(GTEpoll STATIC ${DIR_SRC} ${DIR_SRC_RSRC} ${DIR_SRC_Util} ${DIR_SRC_test} ${UTIL_FILR_THIRDPARTY})

# set output path
set(LIBRARY_OUTPUT_PATH ./build)
