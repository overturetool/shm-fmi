cmake_minimum_required (VERSION 2.8.5)
project (libshmfmu C CXX)

include(CheckCXXCompilerFlag)

set(CMAKE_VERBOSE_MAKEFILE on)

find_package(Protobuf REQUIRED)



# protobuf outputs to current dir
set(CMAKE_INCLUDE_CURRENT_DIR on)

#if (LINUX)
#      set(CMAKE_CXX_FLAGS ${CMAKE_CSS_FLAGS} "-fPIC")
#endif()




CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -Wl,-Bstatic -lprotobuf -lstdc++ -lpthread  -Wl,-Bdynamic")

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-DFMI_COSIMULATION")

include_directories(includes/fmi)
include_directories(${libshmfmi_SOURCE_DIR})


add_library(${PROJECT_NAME} SHARED fmu.cpp FmuContainer.cpp JavaLauncher.cpp ConfigFile.cpp)

#target_compile_options(${PROJECT_NAME} PRIVATE  -Wl,-Bstatic -lstdc++ -lpthread  -Wl,-Bdynamic)

#include_directories(${libshmfmi_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} libshmfmi)


# add_library(imp_protobuf STATIC IMPORTED)
 #set_property(TARGET imp_protobuf PROPERTY IMPORTED_LOCATION /usr/local/lib/libprotobuf.a)

#message(STATUS "protobuf libs = ${PROTOBUF_LIBRARIES}")
#target_link_libraries(${PROJECT_NAME} imp_protobuf)



target_link_libraries(${PROJECT_NAME} rt)
target_link_libraries(${PROJECT_NAME} dl)


#add_library(${PROJECT_NAME} fmu.cpp FmuContainer.cpp JavaLauncher.cpp Config.cpp)

#target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})

set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Expose A's public includes (including the ones of A's dependencies
# transitively) to other subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
                                     CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
