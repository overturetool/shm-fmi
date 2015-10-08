cmake_minimum_required (VERSION 2.8.5)
project (shmfmi-server C CXX)

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


#set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")

if (WIN32)
		#windows	
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++ -static-libgcc -Wl,-Bstatic -lstdc++  -Wl,-Bdynamic")

 set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--add-stdcall-alias")

#message(FATAL_ERROR "JAVAHOME=$ENV{JAVA_HOME}")
#include_directories(src/jni/java-win32/include src/jni/java-win32/include/win32)
include_directories($ENV{JAVA_HOME}/include $ENV{JAVA_HOME}/include/win32)
	endif() 

if (UNIX)
#message(FATAL_ERROR "JAVAHOME=$ENV{JAVA_HOME}")
include_directories($ENV{JAVA_HOME}/include)
#      set(CMAKE_CXX_FLAGS ${CMAKE_CSS_FLAGS} "-fPIC")

if(APPLE)
include_directories($ENV{JAVA_HOME}/include/darwin)
else()
include_directories($ENV{JAVA_HOME}/include/linux)
endif()

endif()




include_directories(${libshmfmi_SOURCE_DIR})
add_library(sharedmemory SHARED src/main/native/sharedmemory.cpp ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(sharedmemory libshmfmi)
target_link_libraries(sharedmemory ${PROTOBUF_LIBRARIES})

find_library(PTHREAD_LIBRARY NAMES libpthread.a)
target_link_libraries(sharedmemory ${PTHREAD_LIBRARY})


