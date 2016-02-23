include(CMakeForceCompiler)
# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Darwin)

# The libgit2 CMakeFile.txt needs this to be set:
SET(CMAKE_SIZEOF_VOID_P 8)

#set(CMAKE_C_COMPILER_ID Clang)
#set(CMAKE_CXX_COMPILER_ID  Clang)

message(STATUS Compiler.. ${CMAKE_C_COMPILER_ID} ${CMAKE_CXX_COMPILER_ID})

#./build/gcc-5.2.0/build/gcc/cc1
#./target/libexec/gcc/x86_64-apple-darwin15/5.2.0/cc1


# which compilers to use for C and C++
cmake_force_c_compiler(${OSXCROSS_ROOT}/bin/x86_64-apple-darwin15-gcc GNU)
cmake_force_cxx_compiler(${OSXCROSS_ROOT}/bin/x86_64-apple-darwin15-g++ GNU)
SET(CMAKE_AR ${OSXCROSS_ROOT}/bin/x86_64-apple-darwin15-ar CACHE FILEPATH "Archiver")
SET(PKG_CONFIG_EXECUTABLE ${OSXCROSS_ROOT}/bin/x86_64h-apple-darwin15-pkg-config)

SET(CMAKE_OSX_SYSROOT ${OSXCROSS_ROOT}/SDK/MacOSX10.11.sdk)

# here is the target environment located
#SET(CMAKE_FIND_ROOT_PATH ${CMAKE_OSX_SYSROOT} ${CMAKE_OSX_SYSROOT}/usr/bin)
SET(CMAKE_FIND_ROOT_PATH ${OSXCROSS_ROOT}/macports/pkgs/opt/local ${CMAKE_OSX_SYSROOT} ${CMAKE_OSX_SYSROOT}/usr/bin ${CMAKE_OSX_SYSROOT}/usr/lib ${CMAKE_OSX_SYSROOT}/usr)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${OSXCROSS_ROOT}/SDK/MacOSX10.11.sdk/usr/lib)
SET(CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")
