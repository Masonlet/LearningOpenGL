cmake_minimum_required(VERSION 3.20)

set(NAME_PROJECT LearningOpenGL)
set(PROJECT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_INC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
set(EXTERNAL "${CMAKE_CURRENT_SOURCE_DIR}/external")

project(${NAME_PROJECT})

# Source and header file collection
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${PROJECT_SRC}/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${PROJECT_INC}/*.hpp)

# Define the main executable
add_executable(${NAME_PROJECT} ${SOURCES} ${HEADERS})

# Organization for Visual Studio
source_group(TREE ${PROJECT_SRC} PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${PROJECT_INC} PREFIX "Header Files" FILES ${HEADERS})

# Include path for LearningOpenGL headers
target_include_directories(${NAME_PROJECT} PRIVATE ${PROJECT_INC})

# Dependencies
include(FetchContent)

# GLAD
add_library(glad STATIC ${EXTERNAL}/src/glad/glad.c)
target_include_directories(glad PUBLIC ${EXTERNAL}/inc)

# GLFW
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_MakeAvailable(glfw)

# OpenGL
find_package(OpenGL REQUIRED)

# Link Everything
target_link_libraries(${NAME_PROJECT} PRIVATE glad glfw OpenGL::GL)

# Seperate external solutions
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "External")
set_target_properties(glad PROPERTIES FOLDER "External")
set_target_properties(glfw PROPERTIES FOLDER "External")
set_target_properties(update_mappings PROPERTIES FOLDER "External")
set_target_properties(uninstall PROPERTIES FOLDER "External")

# Visual Studio Default
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${NAME_PROJECT})
