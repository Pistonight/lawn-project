project(TodLib)

add_library(${PROJECT_NAME} STATIC)

# Source Files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
# popcap...WHYYYY
set(EXCLUDE_FILES
    "Sexy.TodLib/TodDrawTriangle.cpp"
    "Sexy.TodLib/TodDrawTriangleInc.cpp"
)

foreach(EXCLUDED_FILE IN LISTS EXCLUDE_FILES)
    list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/${EXCLUDED_FILE}")
endforeach()
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Include Paths
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    SexyAppFramework
)

# Defines
target_compile_definitions(${PROJECT_NAME} PUBLIC
    WIN32
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)
target_compile_definitions(${PROJECT_NAME} PRIVATE PISTON_PATCH)

# PISTON_PATCH: Chinese characters causing compiler error
if (MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_FORCE_SYNCHRONOUS_PDB_WRITES ON)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()
