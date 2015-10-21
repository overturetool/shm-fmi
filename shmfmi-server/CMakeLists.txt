cmake_minimum_required (VERSION 2.8.5)
project (shmfmi-server C CXX)

include(CheckCXXCompilerFlag)

set(CMAKE_VERBOSE_MAKEFILE on)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})





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


include_directories($ENV{JAVA_HOME}/include)

if (WIN32)
	#windows	
	#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
 	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--add-stdcall-alias")
	include_directories($ENV{JAVA_HOME}/include/win32)
endif() 

if (UNIX)

	if(APPLE)
		include_directories($ENV{JAVA_HOME}/include/darwin)
	else()
		include_directories($ENV{JAVA_HOME}/include/linux)
	endif()
endif()

include_directories(${libshmfmi_SOURCE_DIR})
add_library(sharedmemory SHARED src/main/native/sharedmemory.cpp ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(sharedmemory libshmfmi)

if(APPLE)
	target_link_libraries(sharedmemory /usr/local/lib/libprotobuf.a  ${PROTOBUF_LIBRARIES})
else()
	target_link_libraries(sharedmemory ${PROTOBUF_LIBRARIES})
endif()



foreach(LETTER ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})
	
if(EXISTS "${LETTER}/libstdc++.a")
#   message (STATUS "${LETTER}")
	set(STDCPP_LIBRARY "${LETTER}/libstdc++.a")
else()
#  message (STATUS "${LETTER} ---- not these")
endif()
endforeach()

message(STATUS "Static linking with libstdc++.a in ${STDCPP_LIBRARY}")
target_link_libraries(sharedmemory ${STDCPP_LIBRARY})

find_library(PTHREAD_LIBRARY NAMES libpthread.a)
target_link_libraries(sharedmemory ${PTHREAD_LIBRARY})

if (UNIX)
	if(APPLE)

	else()
 		target_link_libraries(sharedmemory rt)
		target_link_libraries(sharedmemory dl)
	endif()
endif()

if (WIN32)
	#windows	
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif() 

