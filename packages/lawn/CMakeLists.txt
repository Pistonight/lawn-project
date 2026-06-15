# CMakeLists.txt
cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(PlantsVzZombies LANGUAGES C CXX)

# Target
# PISTON: WIN32 - don't show terminal
add_executable(${PROJECT_NAME} WIN32)

include(cmake/UserConfig.cmake)

# Enable Hot Reload for MSVC compilers if supported.
if(MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    add_link_options($<$<CONFIG:Debug>:/INCREMENTAL>)
    add_compile_options($<$<CONFIG:Debug>:/ZI>)
endif()

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # a stupid fix
    # Prevent PDB access conflicts when multiple CL.EXE instances compile in parallel.
    add_compile_options(/FS)
    add_compile_options(/MP)

    # PISTON: patch
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_FORCE_SYNCHRONOUS_PDB_WRITES ON)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
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

# PISTON: library reorganization
add_subdirectory(../resod-framework resod-framework)
add_subdirectory(../resod-lib resod-lib)

# Source Files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.rc"
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

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
target_compile_definitions(${PROJECT_NAME} PRIVATE PISTON_PATCH)

# Generate BuildInfo.h
set(BUILD_INFO_SRC ${CMAKE_SOURCE_DIR}/../resod-framework/src/SexyAppFramework/BuildInfo.h.in)
set(BUILD_INFO_DST ${CMAKE_SOURCE_DIR}/../resod-framework/src/SexyAppFramework/BuildInfo.h)

add_custom_target(build_info ALL
    COMMAND ${CMAKE_COMMAND}
        -DSRC=${BUILD_INFO_SRC}
        -DDST=${BUILD_INFO_DST}
        -DSEXY_PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
        -DVERSION_FILE="${CMAKE_SOURCE_DIR}/../../LawnVersion.txt"
        -P ${CMAKE_SOURCE_DIR}/cmake/BuildInfo.cmake
    COMMENT "Updating build info..."
)

set_source_files_properties(${BUILD_INFO_DST} PROPERTIES GENERATED TRUE)
target_sources(${PROJECT_NAME} PRIVATE ${BUILD_INFO_DST})
add_dependencies(${PROJECT_NAME} build_info)

# Export
set(EXPORT_DIR "${CMAKE_SOURCE_DIR}/export")

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${EXPORT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${EXPORT_DIR}"
)
