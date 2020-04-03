# this is required
SET(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# specify the cross compiler
SET(CMAKE_C_COMPILER E:/usr/linaro/gcc/bin/arm-linux-gnueabihf-gcc.exe)
SET(CMAKE_CXX_COMPILER E:/usr/linaro/gcc/bin/arm-linux-gnueabihf-g++.exe)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  E:/usr/linaro/gcc/)
SET(CMAKE_INCLUDE_PREFIX  E:/usr/linaro/imx6q)
# search for programs in the build host directories (not necessary)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# configure Boost and Qt
#SET(QT_QMAKE_EXECUTABLE /opt/qt-embedded/qmake)
#SET(BOOST_ROOT /opt/boost_arm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(M_LIBRARY m)
