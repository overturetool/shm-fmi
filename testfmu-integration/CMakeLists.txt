cmake_minimum_required (VERSION 2.8.5)
project (testfmu-integration C CXX)

include(CheckCXXCompilerFlag)

set(CMAKE_VERBOSE_MAKEFILE on)

enable_testing()

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

#get path to the FMU in libshmfmu
# We need to configure the FMU location to be used in the test
get_property(libshmfmu_lib_location TARGET libshmfmu PROPERTY LOCATION)
#message (STATUS "libshmfmu_lib_location == ${libshmfmu_lib_location}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFMI_COSIMULATION")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFMI_COSIMULATION")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFMULIB=\\\"${libshmfmu_lib_location}\\\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFMULIB=\\\"${libshmfmu_lib_location}\\\"")
####

file(GLOB CPP_FILES *.cpp)
file(GLOB C_FILES *.c)

include_directories(include/fmi)
include_directories(include)
include_directories(${libshmfmi_INCLUDE_DIRS})
include_directories(${gtest_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} fmu-loader.c fmuLogger.c FMU_Tests.cpp LibraryLoad_Tests.cpp FMU_DualLoad_Tests.cpp resource_location.cpp)

target_link_libraries(${PROJECT_NAME} gtest_main)

if (UNIX)
	if(APPLE)
		# set the protobuf lib
		
	else()
		target_link_libraries(${PROJECT_NAME} rt dl)
	endif()
endif()

if (WIN32)
        #windows        
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif() 


add_test(AllTestsIn-${PROJECT_NAME} ${PROJECT_NAME} --gtest_output=xml:gtestresults.xml)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTEST_RUNNER_DRIVER_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-driver\\\"")
add_executable(${PROJECT_NAME}-driver runner/testrunner.cpp RemoteTestDriver.cpp)

target_link_libraries(${PROJECT_NAME}-driver libshmfmi)
target_link_libraries(${PROJECT_NAME}-driver ${libshmfmi_LIBS})

