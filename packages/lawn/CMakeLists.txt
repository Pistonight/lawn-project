# CMakeLists.txt
cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(PlantsVsZombies)
if (MSVC)
    # Debugging library mismatch issue
    # need to be before add_executable
    # putting it on top
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # a stupid fix
endif()

# Target
add_executable(${PROJECT_NAME} WIN32)
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 23
    CXX_STANDARD 23
    CXX_EXTENSIONS NO
)

include(cmake/UserConfig.cmake)

# Enable Hot Reload for MSVC compilers if supported.
if(MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    add_link_options($<$<CONFIG:Debug>:/INCREMENTAL>)
    add_compile_options($<$<CONFIG:Debug>:/ZI>)
endif()

if (MSVC)
    add_compile_options(/MP)
endif()

option(GITHUB_ACTIONS "Building in GitHub Actions" OFF)

if(MSVC AND GITHUB_ACTIONS)
    message(STATUS "GitHub Actions, adding /SAFESEH:NO, screw you BASS")
    add_link_options(/SAFESEH:NO)
endif()

# woohooo now we can use folder filters
set(CMAKE_USE_FOLDERS ON)

# PISTON: library reorganization
add_subdirectory(../resod-framework resod-framework)
add_subdirectory(../resod-lib resod-lib)

# Source Files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.rc"
    "${CMAKE_SOURCE_DIR}/src-mixin/*.cpp"
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Include Paths
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src-mixin"
)

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ResoddedFramework
)

# Defines
target_compile_definitions(${PROJECT_NAME} PRIVATE
    WIN32
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PISTON_PATCH_UPSTREAM
    PISTON_PATCH
    PISTON_MIXIN
    LAWN_DEBUG_TOOLS
)

if (MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_FORCE_SYNCHRONOUS_PDB_WRITES ON)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# Export
set(TARGET_DIR "target/lawn/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>")
set(EXPORT_DIR "${CMAKE_SOURCE_DIR}/../../${TARGET_DIR}/out")

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${EXPORT_DIR}"
)
