project("shm-fmi")

cmake_minimum_required(VERSION 2.8.10)

include(toolchains/clang-format.cmake)
# You can tweak some common (for all subprojects) stuff here. For example:

#set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

enable_testing()

#if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
#  message(SEND_ERROR "In-source builds are not allowed.")
#endif ()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

# Remove 'lib' prefix for shared libraries on Windows
if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# When done tweaking common stuff, configure the components (subprojects).
# NOTE: The order matters! The most independent ones should go first.
add_subdirectory(libshmipc)
add_subdirectory(libshmfmi) # B is a static library (depends on Boost)
add_subdirectory(libshmfmu)
add_subdirectory(shmfmi-server)
add_subdirectory(third_party/googletest/googletest)
add_subdirectory(testlibshmipc)
add_subdirectory(testlibshmfmi) # C is a shared library (depends on B and external XXX)
add_subdirectory(testlibshmfmu)
add_subdirectory(testfmu-integration)
#add_subdirectory(third_party/googletest/googletest)


#add_subdirectory(components/A) # A is a shared library (depends on C and B)

