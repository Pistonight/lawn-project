project(ThirdPartyLibs)

add_library(${PROJECT_NAME} STATIC)

# Source Files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/*.cpp"
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Include Paths
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/EGL-Registry/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/au_reader/include" # note: not sure where this header is from
    "${CMAKE_CURRENT_SOURCE_DIR}/dr_libs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/json/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/utf8/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/stb/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/bass/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/FidelityFX-FSR/include"
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
    $<$<CONFIG:Release>:NDEBUG>
)
