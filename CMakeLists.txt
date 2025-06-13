cmake_minimum_required(VERSION 3.10)
project(LearningOpenGL)

set(CMAKE_CXX_STANDARD 17)

# Include structure
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

# Visual Studio organization (harmless elsewhere)
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_SOURCE_DIR}/inc PREFIX "Header Files" FILES ${HEADERS})

# Add Executable
add_executable(LearningOpenGL ${SOURCES} ${HEADERS})

find_package(OpenGL REQUIRED)

if(WIN32)
    # Determine 32- vs 64-bit
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH_DIR "x64")
    else()
        set(ARCH_DIR "x86")
    endif()

    # Include and link for Windows
    include_directories(${CMAKE_SOURCE_DIR}/${ARCH_DIR}/inc)
    set(GLFW_LIB_PATH "${CMAKE_SOURCE_DIR}/${ARCH_DIR}/lib/glfw3.lib")
    target_link_libraries(LearningOpenGL OpenGL::GL "${GLFW_LIB_PATH}")

elseif(UNIX)
    # On Linux, just use system packages
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
    include_directories(${GLFW_INCLUDE_DIRS})
    target_link_libraries(LearningOpenGL OpenGL::GL ${GLFW_LIBRARIES})
endif()

# Output directories
set_target_properties(LearningOpenGL PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
