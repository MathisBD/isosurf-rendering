# Requirements
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main Project
project(test VERSION 1.0)
add_executable(test src/main.cpp)
target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND) # use GLVND or legacy GL libraries ?
find_package(OpenGL REQUIRED)
target_include_directories(test PUBLIC ${OPENGL_INCLUDE_DIRS})
target_link_libraries(test ${OPENGL_LIBRARIES})

# GLEW 
find_package(GLEW REQUIRED)
target_include_directories(test PUBLIC ${GLEW_INCLUDE_DIRS})
target_link_libraries(test ${GLEW_LIBRARIES})
add_compile_definitions(GLEW_STATIC) # statically link to GLEW 

# GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(test glfw)