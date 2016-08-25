cmake_minimum_required (VERSION 2.8.5)
project (libshmfmi C CXX)

include(CheckCXXCompilerFlag)

set(CMAKE_VERBOSE_MAKEFILE on)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ../proto/service.proto)



# protobuf outputs to current dir
set(CMAKE_INCLUDE_CURRENT_DIR on)

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

include_directories(${libshmipc_INCLUDE_DIRS})

add_library(${PROJECT_NAME} ../libshmipc/sem_timedwait.cpp ../libshmipc/IpcBase.cpp ../libshmipc/IpcServer.cpp ../libshmipc/IpcClient.cpp ${PROTO_SRCS} ${PROTO_HDRS} FmuProxy.cpp)

set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)


# Expose A's public includes (including the ones of A's dependencies
# transitively) to other subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR} ${PROTOBUF_INCLUDE_DIRS} ${libshmipc_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} 
                                     CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)



# extra libs
set(ADDITIONAL_LIBS "")
if (UNIX)
	if(APPLE)
		# set the protobuf lib
	else()
 		set(ADDITIONAL_LIBS rt dl)
	endif()
endif()

set(${PROJECT_NAME}_LIBS ${PROTOBUF_LIBRARIES} ${ADDITIONAL_LIBS} CACHE INTERNAL "${PROJECT_NAME}: Libraries" FORCE)
