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

# check compiler
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

add_compile_options(-pthread
        -fPIC)

# include header
include_directories(.)
include_directories(./GTEpollUtility/)
include_directories(./thirdparty)
include_directories(../../GTServer/GTUtlity/)
include_directories(../../GTServer/third_party/JSONParser/jsoncpp-1.8.1/include)

# add source files
set(UTIL_FILE_THIRDPARTY
        ../../GTServer/GTUtlity/GT_Util_CfgHelper.cpp
        ../../GTServer/GTUtlity/GT_Util_GlogWrapper.cpp
        ../../GTServer/GTUtlity/GT_Util_OSInfo.cpp
        ../../GTServer/GTUtlity/GT_Util_FileHelper.cpp
)

# add sub directory
add_subdirectory(./test/test_client ./test/test_client/build)
add_subdirectory(./test/test_server ./test/test_server/build)

# add source file
aux_source_directory(. DIR_SRC)
aux_source_directory(./GTEpollResourceControl DIR_SRC_RSRC)
aux_source_directory(./GTEpollUtility DIR_SRC_Util)

# link library
link_libraries(${PROJECT_SOURCE_DIR}/thirdparty/glog/libglog.a)
link_libraries(${PROJECT_SOURCE_DIR}/thirdparty/JsonParser/libjsoncpp.a)


# set target
#add_library(GTEpoll STATIC  ${DIR_SRC} ${DIR_SRC_RSRC} ${DIR_SRC_Util} ${DIR_SRC_test} ${UTIL_FILR_THIRDPARTY})
add_library(GTEpoll SHARED  ${DIR_SRC} ${DIR_SRC_RSRC} ${DIR_SRC_Util} ${DIR_SRC_test} ${UTIL_FILE_THIRDPARTY})

# export function control
SET_TARGET_PROPERTIES (GTEpoll PROPERTIES LINK_FLAGS -Wl,--no-undefined,--version-script,version_ctrl.map)

# set output path
set(LIBRARY_OUTPUT_PATH ./build)

# set target link library
TARGET_LINK_LIBRARIES(GTEpoll
        -lpthread
        )
