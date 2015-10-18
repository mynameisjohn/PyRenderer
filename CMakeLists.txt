# just what apt gets me
cmake_minimum_required(VERSION 2.8)

# Project name
project(PyRenderer)

# The c++11 cmake thing never works
set(CMAKE_CXX_FLAGS "-std=c++14")

# Find these libraries
find_package(SDL2)
find_package(SDL2_mixer)
find_package(OpenGL)
find_package(GLEW)

# Some platform specifics
if (WIN32)
	add_definitions(-DGLEW_STATIC)
	find_package(PythonLibs)
	# set(PYTHON_LIBRARY C:/Python35/libs/python35_d.lib)
	# set(PYTHON_INCLUDE_DIR C:/Python35/include)
	set(SDL2_LIBS ${SDL2_LIBRARY} ${SDLMAIN_LIBRARY} ${SDLMIXER_LIBRARY})
else(WIN32)
	set(PYTHON_LIBRARY /usr/local/lib/libpython3.5m.a)
	set(PYTHON_INCLUDE_DIR /usr/local/include/python3.5m)
	set(SDL2_LIBS ${SDL2_LIBRARY} ${SDLMIXER_LIBRARY})
endif (WIN32)

# Library variables
set(GL_LIBS ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})

set(GL_INC ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
set(SDL2_INC ${SDL2_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR})

# Local files
file(GLOB SOURCES source/*.cpp)
file(GLOB HEADERS include/*.h)
file(GLOB_RECURSE RES Resources/*)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../PyLiaison ${CMAKE_CURRENT_SOURCE_DIR}/PyLiaison)

# include dirs
include_directories(${GL_INC} ${SDL2_INC}  ${PYTHON_INCLUDE_DIR} 
					${CMAKE_CURRENT_SOURCE_DIR}/../PyLiaison/include 
					${CMAKE_CURRENT_SOURCE_DIR}/include
					/usr/local/include)

add_executable(PyRenderer ${SOURCES} ${HEADERS} ${SHADERS} ${SCRIPTS})
target_link_libraries(PyRenderer LINK_PUBLIC PyLiaison ${SDL2_LIBS} ${GL_LIBS} ${PYTHON_LIBRARY})
