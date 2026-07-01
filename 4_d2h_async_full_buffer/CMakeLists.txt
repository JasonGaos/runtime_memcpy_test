cmake_minimum_required(VERSION 3.16.0)

project(D2H_Async_Full_Buffer_Test)

set(SOC_VERSION $ENV{SOC_VERSION})
set(ASCENDC_CMAKE_DIR $ENV{ASCENDC_CMAKE_DIR})
set(RUN_MODE "npu" CACHE STRING "run mode: npu")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type Release/Debug (default Debug)" FORCE)
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/out" CACHE STRING "path for install()" FORCE)

include(${ASCENDC_CMAKE_DIR}/ascendc.cmake)

include_directories(${ASCEND_CANN_PACKAGE_PATH}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
link_directories(${ASCEND_CANN_PACKAGE_PATH}/lib64)

ascendc_library(full_buffer_kernels STATIC ../common/full_buffer_kernels.cpp)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE ascendcl full_buffer_kernels)
