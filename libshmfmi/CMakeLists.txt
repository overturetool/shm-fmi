cmake_minimum_required (VERSION 2.8.5)
project (libshmfmi C CXX)

include(CheckCXXCompilerFlag)

set(CMAKE_VERBOSE_MAKEFILE on)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})


PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ../proto/service.proto)


# protobuf outputs to current dir
set(CMAKE_INCLUDE_CURRENT_DIR on)

#if (LINUX)
#      set(CMAKE_CXX_FLAGS ${CMAKE_CSS_FLAGS} "-fPIC")
#endif()
#set_target_properties(${PROJECT_NAME}
#                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

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


#add_library(${PROJECT_NAME}-cc ${PROTO_SRCS})

add_library(${PROJECT_NAME} FmiIpc.cpp ${PROTO_SRCS} ${PROTO_HDRS} ExternalClient.cpp)
#add_library(${PROJECT_NAME}-shared SHARED FmiIpc.cpp ExternalClient.cpp ${PROTO_SRCS})
#${PROTO_SRCS} ${PROTO_HDRS}
#target_link_libraries(${PROJECT_NAME}-shared ${PROTOBUF_LIBRARIES})
#target_link_libraries(${PROJECT_NAME}-shared ${PROJECT_NAME}-cc)

set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

#set_target_properties(${PROJECT_NAME}-shared
#                      PROPERTIES POSITION_INDEPENDENT_CODE ON)


# Expose A's public includes (including the ones of A's dependencies
# transitively) to other subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
                                     CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
