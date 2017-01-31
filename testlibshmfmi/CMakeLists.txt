cmake_minimum_required (VERSION 2.8.5)
project (testlibshmfmi CXX)

enable_testing()
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

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")

file(GLOB CPP_FILES *.cpp)

add_executable(${PROJECT_NAME} ${CPP_FILES})

include_directories(${libshmfmi_INCLUDE_DIRS})
include_directories(${gtest_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} gtest_main)
target_link_libraries(${PROJECT_NAME} libshmfmi)
target_link_libraries(${PROJECT_NAME} ${libshmfmi_LIBS})

if (WIN32)
  #windows        
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif() 

if (WIN32)
	add_test(AllTestsIn-${PROJECT_NAME} wine ${PROJECT_NAME} --gtest_output=xml:gtestresults.xml)
else()
	add_test(AllTestsIn-${PROJECT_NAME} ${PROJECT_NAME} --gtest_output=xml:gtestresults.xml)
endif()
