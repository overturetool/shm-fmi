cmake_minimum_required (VERSION 2.8.5)
project (shmfmi-server C CXX)

include(CheckCXXCompilerFlag)

set(CMAKE_VERBOSE_MAKEFILE on)

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

message("JAVA_HOME set to: ${JAVA_HOME}")

include_directories(${JAVA_HOME}/include)

if (WIN32)
	#windows	
	#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
 	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--add-stdcall-alias")
	include_directories(${JAVA_HOME}/include/win32)
endif() 

if (UNIX)

	if(APPLE)
		include_directories(${JAVA_HOME}/include/darwin)
	else()
		include_directories(${JAVA_HOME}/include/linux)
	endif()
endif()

include_directories(${libshmfmi_INCLUDE_DIRS})
add_library(sharedmemory SHARED src/main/native/sharedmemory.cpp src/main/native/sharedmemoryserver.cpp ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(sharedmemory libshmfmi)
target_link_libraries(sharedmemory ${libshmfmi_LIBS})


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

if(NOT APPLE)
#https://developer.apple.com/legacy/library/documentation/Darwin/Reference/ManPages/man3/pthread.3.html it is included in system
find_library(PTHREAD_LIBRARY NAMES libpthread.a)
target_link_libraries(sharedmemory ${PTHREAD_LIBRARY})
endif()

if (WIN32)
	#windows	
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif() 

