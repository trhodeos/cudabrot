project(cudabrot)

cmake_minimum_required(VERSION 2.8)

set(CUDA_NVCC_FLAGS "--gpu-architecture=sm_11;")

######## general environment info ########
include(cmake/)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/)
if (UNIX AND NOT APPLE)
  set(LINUX 1)
endif (UNIX AND NOT APPLE)

######## package setup ########
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(CUDA REQUIRED)

######## system specifics ########
if (APPLE)
  find_library(COCOA_LIBRARY Cocoa)
endif (APPLE)

######## subdirectories ########
add_subdirectory(src)



