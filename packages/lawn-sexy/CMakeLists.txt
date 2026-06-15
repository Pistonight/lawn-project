# CmakeLists.txt
project(SexyAppFramework)

add_library(${PROJECT_NAME} STATIC)

# Source Files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
# popcap...WHYYYY
set(EXCLUDE_FILES
    "SWTri_DrawTriangle.cpp"
    "SWTri_DrawTriangleInc1.cpp"
    "SWTri_DrawTriangleInc2.cpp"
    "SWTri_GetTexel.cpp"
    "SWTri_Loop.cpp"
    "SWTri_Loop.cpp"
    "SWTri_Pixel555.cpp"
    "SWTri_Pixel565.cpp"
    "SWTri_Pixel888.cpp"
    "SWTri_Pixel8888.cpp"
    "SWTri_TexelARGB.cpp"
)
foreach(EXCLUDED_FILE IN LISTS EXCLUDE_FILES)
    list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/SexyAppFramework/${EXCLUDED_FILE}")
endforeach()
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Include Paths
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Libraries
find_package(SDL3 CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(Ogg CONFIG REQUIRED)
find_package(Vorbis CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)
# if (SEXY_CRASH_HANDLER)
find_package(cpptrace CONFIG REQUIRED)
# endif()
find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC 
    SDL3::SDL3
    ZLIB::ZLIB
    OpenAL::OpenAL
    glm::glm
    Ogg::ogg
    Freetype::Freetype
    Vorbis::vorbisfile
    Vorbis::vorbis
    cpptrace::cpptrace
    # $<$<BOOL:${SEXY_CRASH_HANDLER}>:cpptrace::cpptrace>
    CURL::libcurl
    ImageLib
    PakLib
    ThirdPartyLibs
)

if(WIN32)
    find_library(AVCODEC_LIBRARY avcodec)
    find_library(AVDEVICE_LIBRARY avdevice)
    find_library(AVFORMAT_LIBRARY avformat)
    find_library(AVUTIL_LIBRARY avutil)
    find_library(SWRESAMPLE_LIBRARY swresample)
    find_library(SWSCALE_LIBRARY swresample)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/../lawn-libs/bass/lib/bass.lib
        ${AVCODEC_LIBRARY}
        ${AVDEVICE_LIBRARY}
        ${AVFORMAT_LIBRARY}
        ${AVUTIL_LIBRARY}
        ${SWRESAMPLE_LIBRARY}
        ${SWSCALE_LIBRARY}
    )

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # PISTON_PATCH - changed library organization and won't test on Linux
    message(FATAL_ERROR "Linux builds currently not supported")
    # find_package(PkgConfig REQUIRED)
    # pkg_check_modules(AVCODEC REQUIRED libavcodec)
    # pkg_check_modules(AVDEVICE REQUIRED libavdevice)
    # pkg_check_modules(AVFORMAT REQUIRED libavformat)
    # pkg_check_modules(AVUTIL REQUIRED libavutil)
    # pkg_check_modules(SWRESAMPLE REQUIRED libswresample)
    # pkg_check_modules(SWSCALE REQUIRED libswscale)
    #
    # target_include_directories(${PROJECT_NAME} PUBLIC
    #     ${AVCODEC_INCLUDE_DIRS}
    #     ${AVDEVICE_INCLUDE_DIRS}
    #     ${AVFORMAT_INCLUDE_DIRS}
    #     ${AVUTIL_INCLUDE_DIRS}
    #     ${SWRESAMPLE_INCLUDE_DIRS}
    #     ${SWSCALE_INCLUDE_DIRS}
    # )
    #
    # target_link_libraries(${PROJECT_NAME} PUBLIC
    #     ${AVCODEC_LIBRARIES}
    #     ${AVDEVICE_LIBRARIES}
    #     ${AVFORMAT_LIBRARIES}
    #     ${AVUTIL_LIBRARIES}
    #     ${SWRESAMPLE_LIBRARIES}
    #     ${SWSCALE_LIBRARIES}
    # )
    #
    # find_library(BASS_LIBRARY NAMES bass)
    # if(BASS_LIBRARY)
    #     target_link_libraries(${PROJECT_NAME} PUBLIC ${BASS_LIBRARY})
    # else()
    #     message(WARNING "BASS library not found by CMake! Falling back to linking dynamically to 'bass'.")
    #     target_link_libraries(${PROJECT_NAME} PUBLIC bass)
    # endif()
endif()


# Defines
# PISTON_PATCH: Chinese characters causing compiler error
if (MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_FORCE_SYNCHRONOUS_PDB_WRITES ON)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE PISTON_PATCH)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>

    # SAF specific
    USE_OGG_LIB
    SEXY_USE_OPENGL
    SEXY_USE_SDL3_RENDERER
    # $<$<BOOL:${SEXY_USE_IMGUI}>:SEXY_USE_IMGUI>
    # $<$<BOOL:${SEXY_USE_CONTROLLER}>:SEXY_USE_CONTROLLER>
    # $<$<BOOL:${SEXY_CRASH_HANDLER}>:SEXY_CRASH_HANDLER>

    SEXY_USE_IMGUI
    SEXY_USE_CONTROLLER
    SEXY_CRASH_HANDLER

    A_CPU
)

# option(SEXY_USE_IMGUI "Enable ImGui" ON)
# option(SEXY_USE_CONTROLLER "Enable the use of a controller" ON)
# option(SEXY_CRASH_HANDLER "Enable the use of the crash handler" ON)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    SEXY_IS_X64
)
else ()
target_compile_definitions(${PROJECT_NAME} PUBLIC
    SEXY_IS_X86
)
endif()



# set(BUILD_INFO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/SexyAppFramework/BuildInfo.h.in)
# set(BUILD_INFO_DST ${CMAKE_CURRENT_SOURCE_DIR}/src/SexyAppFramework/BuildInfo.h)
#
# add_custom_target(build_info ALL
#     COMMAND ${CMAKE_COMMAND}
#         -DSRC=${BUILD_INFO_SRC}
#         -DDST=${BUILD_INFO_DST}
#         -DSEXY_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
#         -DVERSION_FILE="${CMAKE_SOURCE_DIR}/../../LawnVersion.txt"
#         -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildInfo.cmake
#     COMMENT "Updating build info..."
# )
#
# set_source_files_properties(${BUILD_INFO_DST} PROPERTIES GENERATED TRUE)
# target_sources(${PROJECT_NAME} PRIVATE ${BUILD_INFO_DST})
# add_dependencies(${PROJECT_NAME} build_info)
