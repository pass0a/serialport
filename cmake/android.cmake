# this is required
SET(PASSOA_COMPILER_NDK TRUE)
SET(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# specify the cross compiler
SET(CMAKE_C_COMPILER D:/dev/androidsdk/ndk/arm/bin/arm-linux-androideabi-gcc.exe)
SET(CMAKE_CXX_COMPILER D:/dev/androidsdk/ndk/arm/bin/arm-linux-androideabi-g++.exe)
#SET(CMAKE_MAKE_PROGRAM D:/dev/androidsdk/ndk/arm/bin/make.exe)
# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  D:/dev/androidsdk/ndk/arm/)
# search for programs in the build host directories (not necessary)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

add_definitions(-DPLAT_NDK)

if(__ANDROID_API__)
add_definitions(-D__ANDROID_API__=${__ANDROID_API__})
endif()

if(CMAKE_ANDROID_ARCH_ABI)
else()
set(CMAKE_ANDROID_ARCH_ABI "default")
endif()

#-mtune=cortex-a9 -mfpu=CMAKE_FIND_ROOT_PATH_MODE_INCLUDE
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie -fPIE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__ANDROID_API__=17 -pie -fPIE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__ANDROID_API__=17 -std=c++11 -march=armv7-a -fPIE")

set(M_LIBRARY m)