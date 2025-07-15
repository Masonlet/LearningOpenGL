cmake_minimum_required(VERSION 3.20)
project(LearningOpenGL)

set(CMAKE_CXX_STANDARD 20)

# Include structure
set(INC_DIR "${CMAKE_SOURCE_DIR}/inc")
include_directories(${INC_DIR})

# Source files 
file(GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
)

# Header files
file(GLOB_RECURSE HEADERS 
  ${CMAKE_SOURCE_DIR}/inc/*.hpp
  ${CMAKE_SOURCE_DIR}/inc/*.h
)

# Assets
file(GLOB_RECURSE ASSET_SHADERS "${CMAKE_SOURCE_DIR}/assets/shaders/*")
file(GLOB_RECURSE ASSET_MODELS "${CMAKE_SOURCE_DIR}/assets/models/*")
file(GLOB_RECURSE ASSET_SCENES "${CMAKE_SOURCE_DIR}/assets/scenes/*")

# Visual Studio organization
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_SOURCE_DIR}/inc PREFIX "Header Files" FILES ${HEADERS})

source_group("Assets\\Shaders" FILES ${ASSET_SHADERS})
source_group("Assets\\Models"  FILES ${ASSET_MODELS})
source_group("Assets\\Scenes"  FILES ${ASSET_SCENES})

# Add Executable
add_executable(LearningOpenGL 
  ${SOURCES} ${HEADERS}
  ${ASSET_SHADERS} ${ASSET_MODELS} ${ASSET_SCENES}
)

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

add_custom_command(TARGET LearningOpenGL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:LearningOpenGL>/assets"
)

set_target_properties(LearningOpenGL PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:LearningOpenGL>"
)

# Default Project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LearningOpenGL)
