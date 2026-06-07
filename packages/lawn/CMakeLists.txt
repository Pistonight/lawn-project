# CMakeLists.txt
cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(ResoddedFramework LANGUAGES C CXX)

project(PlantsVzZombies)

# Enable Hot Reload for MSVC compilers if supported.
if(MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    add_link_options($<$<CONFIG:Debug>:/INCREMENTAL>)
    add_compile_options($<$<CONFIG:Debug>:/ZI>)
endif()

if (MSVC)
    add_compile_options(/MP)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # a stupid fix
endif()

option(GITHUB_ACTIONS "Building in GitHub Actions" OFF)

if(MSVC AND GITHUB_ACTIONS)
    message(STATUS "GitHub Actions, adding /SAFESEH:NO, screw you BASS")
    add_link_options(/SAFESEH:NO)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# woohooo now we can use folder filters
set(CMAKE_USE_FOLDERS ON)

# PISTON_PATCH library reorganization
add_subdirectory(../lawn-game lawn-game)
add_subdirectory(../lawn-sexy lawn-sexy)
add_subdirectory(../lawn-imagelib lawn-imagelib)
add_subdirectory(../lawn-paklib lawn-paklib)
add_subdirectory(../lawn-libs lawn-libs)

# Target
# PISTON_PATCH: WIN32 - don't show terminal
add_executable(${PROJECT_NAME} WIN32)

# Source Files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.rc"
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Include Paths
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    LawnProject
)

# Defines
target_compile_definitions(${PROJECT_NAME} PRIVATE
    WIN32
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)
target_compile_definitions(${PROJECT_NAME} PRIVATE PISTON_PATCH)

# PISTON_PATCH: Chinese characters causing compiler error
if (MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_FORCE_SYNCHRONOUS_PDB_WRITES ON)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# Export
set(EXPORT_DIR "${CMAKE_SOURCE_DIR}/export")

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${EXPORT_DIR}"
)
