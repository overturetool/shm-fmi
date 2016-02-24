# the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)

SET(CMAKE_CXX_FLAGS -m32)
SET(CMAKE_C_FLAGS -m32)
SET(CMAKE_EXE_LINKER_FLAGS -m32)
SET(CMAKE_MODULE_LINKER_FLAGS -m32)
SET(CMAKE_SHARED_LINKER_FLAGS -m32)
SET(CMAKE_STATIC_LINKER_FLAGS -m32)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /usr/lib32 )

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(PTHREAD_LIBRARY /usr/lib32/libpthread.a)
