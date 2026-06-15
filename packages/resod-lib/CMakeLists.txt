project(ResodLib)
add_library(${PROJECT_NAME} STATIC)

# Source Files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/src/*.cpp"
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Include Paths
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/bass/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/FidelityFX-FSR/include"
)
target_include_directories(${PROJECT_NAME} PRIVATE
    # needed by imgui files
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/include/imgui"
)

# Libraries
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
)

# Defines
target_compile_definitions(${PROJECT_NAME} PUBLIC
    WIN32
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)
