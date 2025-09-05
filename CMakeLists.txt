cmake_minimum_required(VERSION 3.20)
project(LearningOpenGL)

# Source and header file collection
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp)

# Define the main executable
add_executable(LearningOpenGL ${SOURCES} ${HEADERS})

# Organization for Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/inc PREFIX "Header Files" FILES ${HEADERS})

# Include path for LearningOpenGL headers
target_include_directories(LearningOpenGL PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/inc")

# Dependencies
include(FetchContent)

# GLAD
add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/external/src/glad/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/inc)

# GLFW
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)

# OpenGL
find_package(OpenGL REQUIRED)

# Link Everything
target_link_libraries(LearningOpenGL PRIVATE glad glfw OpenGL::GL)

# Visual Studio Default
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LearningOpenGL)
