cmake_minimum_required(VERSION 3.16.0)

project(H2D_Async_Full_Buffer_Test)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type Release/Debug (default Debug)" FORCE)
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/out" CACHE STRING "path for install()" FORCE)

include_directories(${ASCEND_CANN_PACKAGE_PATH}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
link_directories(${ASCEND_CANN_PACKAGE_PATH}/lib64)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE ascendcl)
