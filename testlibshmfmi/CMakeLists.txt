cmake_minimum_required (VERSION 2.8.5)
project (testlibshmfmi CXX)

include(CheckCXXCompilerFlag)

set(CMAKE_VERBOSE_MAKEFILE on)


find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ../proto/service.proto)


if (LINUX)
 #     set(CMAKE_CXX_FLAGS ${CMAKE_CSS_FLAGS} "-fPIC")
# Required on Unix OS family to be able to be linked into shared libraries.
#set_target_properties(${PROJECT_NAME}
#                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()


CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -pthread")

#add_library(shmfmi FmiIpc.cpp ${PROTO_SRCS} ${PROTO_HDRS} ExternalClient.cpp)
#add_library(shmfmi-shared SHARED FmiIpc.cpp ${PROTO_SRCS} ${PROTO_HDRS} ExternalClient.cpp)
#target_link_libraries(shmfmi-shared ${PROTOBUF_LIBRARIES})


file(GLOB CPP_FILES *.cpp)

#add_definitions(${libshmfmi_DEFINITIONS})

#include_directories(${libshmfmi_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${CPP_FILES})
include_directories(${libshmfmi_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} gtest_main)
include_directories(${gtest_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} libshmfmi)

target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJECT_NAME} rt)
target_link_libraries(${PROJECT_NAME} dl)
#target_link_libraries(${PROJECT_NAME} pthread)

