cmake_minimum_required (VERSION 2.8.5)
project (testlibshmfmu CXX)

include(CheckCXXCompilerFlag)

set(CMAKE_VERBOSE_MAKEFILE on)


find_package(Protobuf REQUIRED)





if (LINUX)
 #     set(CMAKE_CXX_FLAGS ${CMAKE_CSS_FLAGS} "-fPIC")
# Required on Unix OS family to be able to be linked into shared libraries.
#set_target_properties(${PROJECT_NAME}
#                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()


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

#get path to the FMU in libshmfmu
# We need to configure the FMU location to be used in the test
get_property(libshmfmu_lib_location TARGET libshmfmu PROPERTY LOCATION)
#message (STATUS "libshmfmu_lib_location == ${libshmfmu_lib_location}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFMULIB=\\\"${libshmfmu_lib_location}\\\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFMULIB=\\\"${libshmfmu_lib_location}\\\"")
####

file(GLOB CPP_FILES *.cpp)
file(GLOB C_FILES *.c)

include_directories(include/fmi)
include_directories(include)

add_executable(${PROJECT_NAME} ${CPP_FILES} ${C_FILES})
include_directories(${libshmfmi_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} gtest_main)
include_directories(${gtest_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} libshmfmi)

target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJECT_NAME} rt)
target_link_libraries(${PROJECT_NAME} dl)
#target_link_libraries(${PROJECT_NAME} pthread)

