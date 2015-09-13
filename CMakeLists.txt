# just what apt gets me
cmake_minimum_required(VERSION 2.8)

# Project name
project(PyRenderer)

# The c++11 cmake thing never works
set(CMAKE_CXX_FLAGS "-std=c++11")

# Find these libraries
find_package(SDL2)
find_package(OpenGL)
find_package(GLEW)

# Some platform specifics
if (WIN32)
	add_definitions(-DGLEW_STATIC)
	set(GLM_DIR $ENV{SDK}/glm)
	set(PYTHON_LIBRARY C:/Python34/libs/python34_d.lib)
	set(PYTHON_INCLUDE_DIR C:/Python34/include)
else (WIN32)
	set(GLM_DIR /usr/include/glm)
endif (WIN32)

# Library variables
set(GL_LIBS ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
set(SDL2_LIBS ${SDL2_LIBRARY})

set(GL_INC ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
set(SDL2_INC ${SDL2_INCLUDE_DIR})

# Local files
file(GLOB SOURCES source/*.cpp)
file(GLOB HEADERS include/*.h)
file(GLOB_RECURSE RES Resources/*)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../PyLiaison ${CMAKE_CURRENT_BINARY_DIR}/PyLiaison)

# include dirs
include_directories(${GLM_DIR} ${GL_INC} ${SDL2_INC}  ${PYTHON_INCLUDE_DIR} 
					${CMAKE_CURRENT_SOURCE_DIR}/../PyLiaison/include 
					${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(PyRenderer ${SOURCES} ${HEADERS} ${SHADERS} ${SCRIPTS})
target_link_libraries(PyRenderer ${SDL2_LIBS} ${GL_LIBS} ${PYTHON_LIBRARY} pyliaison)
