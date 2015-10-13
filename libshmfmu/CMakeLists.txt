cmake_minimum_required (VERSION 2.8.5)
project (libshmfmu C CXX)

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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFMI_COSIMULATION")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFMI_COSIMULATION")

include_directories(includes/fmi)
include_directories(${libshmfmi_SOURCE_DIR})


add_library(${PROJECT_NAME} SHARED fmu.cpp FmuContainer.cpp JavaLauncher.cpp ConfigFile.cpp)


target_link_libraries(${PROJECT_NAME} libshmfmi)
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})


#target_link_libraries(${PROJECT_NAME} "/usr/lib/gcc/i686-w64-mingw32/4.8/libstdc++.a")
#find_library(STDCPP_LIBRARY NAMES libstdc++.a PATHS ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})

foreach(LETTER ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})
	
if(EXISTS "${LETTER}/libstdc++.a")
#   message (STATUS "${LETTER}")
	set(STDCPP_LIBRARY "${LETTER}/libstdc++.a")
else()
#  message (STATUS "${LETTER} ---- not these")
endif()
endforeach()

message(STATUS "Static linking with libstdc++.a in ${STDCPP_LIBRARY}")
target_link_libraries(${PROJECT_NAME} ${STDCPP_LIBRARY})


find_library(PTHREAD_LIBRARY NAMES libpthread.a)
target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARY})


# I need this: -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic


if (UNIX)
	if(APPLE)

	else()
 		target_link_libraries(${PROJECT_NAME} rt)
		target_link_libraries(${PROJECT_NAME} dl)
	endif()
endif()


if (WIN32)
	#windows	
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif() 


##
## TODO: The only thing missing now is to add this at -lstdc++ the end of the g++ link command but before -pthread and inside the -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic
## 
## When this is done the dll produced can run on windows (when windows has mingw, I havent tested on a clean version of Windows yet)
##

set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)



# Expose A's public includes (including the ones of A's dependencies
# transitively) to other subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
                                     CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
