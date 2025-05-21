cmake_minimum_required(VERSION 3.10)
project(LearningOpenGL)

set(CMAKE_CXX_STANDARD 17)

# Detect target architecture (x86 or x64)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "x64")
else()
    set(ARCH_DIR "x86")
endif()

# Add the correct GLFW include and lib paths
include_directories(${CMAKE_SOURCE_DIR}/${ARCH_DIR}/inc)
link_directories(${CMAKE_SOURCE_DIR}/${ARCH_DIR}/lib)

# Your own include structure
set(INC_DIR "${CMAKE_SOURCE_DIR}/inc")
include_directories(${INC_DIR})
include_directories(${INC_DIR}/Core)
include_directories(${INC_DIR}/Engine)
include_directories(${INC_DIR}/Util)

# Source and header files
file(GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/inc/*.hpp)

# Visual Studio organization
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_SOURCE_DIR}/inc PREFIX "Header Files" FILES ${HEADERS})

# Executable
add_executable(LearningOpenGL ${SOURCES} ${HEADERS})

# Link OpenGL and GLFW
find_package(OpenGL REQUIRED)
target_link_libraries(LearningOpenGL OpenGL::GL glfw3)

# Output directories
set_target_properties(LearningOpenGL PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)