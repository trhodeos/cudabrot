######## cxx flags ########
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare")

######## includes ########
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_SOURCE_DIR} ${CUDA_INCLUDE_DIRS})

######## main sources ########
set(CUDABROT_SOURCES
    cudabrot.cpp
)
set(CUDABROT_HEADERS
    cudabrot.h

    os_gl.h
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)


######## system specifics ########
set(EXTRA_LIBS "")
if (APPLE)
   set(EXTRA_LIBS ${EXTRA_LIBS} "-framework Cocoa -framework OpenGL -framework GLUT")
endif (APPLE)

######## output path ########
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

######## main library ########
add_executable(cudabrot main.cpp
  ${CUDABROT_SOURCES}
)
target_link_libraries(cudabrot
   ${CUDA_LIBRARIES}
   ${OpenGL_LIBRARY}
   ${GLUT_LIBRARY}
   ${GLEW_LIBRARY}
   ${EXTRA_LIBS}
)
