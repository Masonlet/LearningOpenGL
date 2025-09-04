cmake_minimum_required(VERSION 3.20)
project(LearningOpenGL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SUPPRESS_REGENERATION ON)
set(ASSET_SRC "${CMAKE_CURRENT_SOURCE_DIR}/assets")

file(GLOB_RECURSE SOURCES        CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS        CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h)
file(GLOB_RECURSE ASSET_SHADERS  CONFIGURE_DEPENDS "${ASSET_SRC}/shaders/*")
file(GLOB_RECURSE ASSET_MODELS   CONFIGURE_DEPENDS "${ASSET_SRC}/models/*")
file(GLOB_RECURSE ASSET_SCENES   CONFIGURE_DEPENDS "${ASSET_SRC}/scenes/*")
file(GLOB_RECURSE ASSET_TEXTURES CONFIGURE_DEPENDS "${ASSET_SRC}/textures/*")

add_executable(LearningOpenGL ${SOURCES} ${HEADERS})

# Visual Studio organization
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/inc PREFIX "Header Files" FILES ${HEADERS})

add_custom_target(Assets SOURCES ${ASSET_SHADERS} ${ASSET_MODELS} ${ASSET_SCENES} ${ASSET_TEXTURES})
set_target_properties(Assets PROPERTIES FOLDER "Assets")
source_group("Assets\\Shaders"  FILES ${ASSET_SHADERS})
source_group("Assets\\Models"   FILES ${ASSET_MODELS})
source_group("Assets\\Scenes"   FILES ${ASSET_SCENES})
source_group("Assets\\Textures" FILES ${ASSET_TEXTURES})

target_include_directories(LearningOpenGL PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

include(FetchContent)
FetchContent_Declare(StarletEngine GIT_REPOSITORY https://github.com/masonlet/starletengine.git GIT_TAG main)
FetchContent_MakeAvailable(StarletEngine)

target_link_libraries(LearningOpenGL PUBLIC StarletEngine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "External")

# Output directories
set_target_properties(LearningOpenGL PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:LearningOpenGL>"
)

# Default Project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LearningOpenGL)

file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/assets" ASSET_PATH)
target_compile_definitions(LearningOpenGL PRIVATE ASSET_DIR="${ASSET_PATH}")
