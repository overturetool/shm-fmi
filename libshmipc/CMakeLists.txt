cmake_minimum_required (VERSION 2.8.5)
project (libshmipc C CXX)

include(CheckCXXCompilerFlag)

set(CMAKE_VERBOSE_MAKEFILE on)



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

set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

add_executable(${PROJECT_NAME} FmiIpc.cpp ipcmain.cpp)

# Expose A's public includes (including the ones of A's dependencies
# transitively) to other subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR} CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

find_library(PTHREAD_LIBRARY NAMES libpthread.a libpthread.dylib)
target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARY})

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

target_link_libraries(${PROJECT_NAME} ${ADDITIONAL_LIBS})


