cmake_minimum_required (VERSION 2.8.5)
project (testlibshmfmu C CXX)

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

add_executable(${PROJECT_NAME} FMU_Tests.cpp LibraryLoad_Tests.cpp ${C_FILES})

target_link_libraries(${PROJECT_NAME} gtest_main)
#target_link_libraries(${PROJECT_NAME} libshmfmi)


#set(libshmfmi_LIBS_filtered "${PROTOBUF_LIBRARIES} -lrt -ldl")
#set(libshmfmi_LIBS_filtered "${libshmfmi_LIBS}")
#message(STATUS "libshmfmi's are: ${libshmfmi_LIBS}")
#foreach(LETTER ${libshmfmi_LIBS})
	
#if(${LETTER} MATCHES "libprotobuf.a")
#   message (STATUS "Found the static lib for protobuf${LETTER}")
	#set(STDCPP_LIBRARY "${LETTER}/libstdc++.a")
#set(libshmfmi_LIBS_filtered "${libshmfmi_LIBS_filtered} ")
#else()
#  message (STATUS "${LETTER} ---- not these")
#set(libshmfmi_LIBS_filtered "${libshmfmi_LIBS_filtered};${LETTER}")
#endif()
#endforeach()
#message(STATUS "libshmfmi's altered to: ${libshmfmi_LIBS_filtered}")


#target_link_libraries(${PROJECT_NAME} ${libshmfmi_LIBS})
#target_link_libraries(${PROJECT_NAME} ${libshmfmi_LIBS_filtered})
target_link_libraries(${PROJECT_NAME} rt dl)

add_test(AllTestsIn-testlibshmfmu ${PROJECT_NAME})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTEST_RUNNER_DRIVER_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-driver\\\"")
add_executable(${PROJECT_NAME}-driver runner/testrunner.cpp RemoteTestDriver.cpp)



target_link_libraries(${PROJECT_NAME}-driver libshmfmi)
target_link_libraries(${PROJECT_NAME}-driver ${libshmfmi_LIBS})

