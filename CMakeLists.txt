cmake_minimum_required(VERSION 3.24...4.2)

set(PLUGIN_AUTHOR "gabriel-andreescu" CACHE STRING "Plugin author name")
set(PLUGIN_LICENSE "MIT" CACHE STRING "Plugin license")

set(VCPKG_DISABLE_METRICS ON)
set(VCPKG_INSTALL_OPTIONS "--no-print-usage")

project(
        NoIdleVanityCameraSKSE
        VERSION 1.0.0
        DESCRIPTION "Vanity camera begone"
        LANGUAGES CXX
)

if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a build directory and run CMake from there.")
endif ()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif ()

include(CheckIPOSupported)
check_ipo_supported(RESULT SUPPORTS_IPO OUTPUT IPO_OUTPUT)
if (SUPPORTS_IPO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
endif ()

set(DEPLOY_DIR "" CACHE PATH "Deployment directory (e.g., C:/Games/Skyrim SE/Data)")

add_subdirectory(extern/CommonLibSSE-NG)

include(extern/CommonLibSSE-NG/cmake/CommonLibSSE.cmake)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
        @ONLY
)

set(SOURCES
        src/main.cpp
        "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
)

add_commonlibsse_plugin(
        ${PROJECT_NAME}
        SOURCES ${SOURCES}
        AUTHOR "${PLUGIN_AUTHOR}"
)

target_compile_features(
        ${PROJECT_NAME}
        PUBLIC
        cxx_std_23
)

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        CommonLibSSE::CommonLibSSE
)

target_precompile_headers(
        ${PROJECT_NAME}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.h"
)

if (MSVC)
    target_compile_options(
            ${PROJECT_NAME}
            PRIVATE
            /EHsc
            /W4
            /WX
            /utf-8
            /permissive-
            $<$<CONFIG:RelWithDebInfo>:/Ox>
    )

    target_link_options(
            ${PROJECT_NAME}
            PRIVATE
            $<$<CONFIG:Debug>:/DEBUG>
            $<$<CONFIG:Release>:/DEBUG>
            $<$<CONFIG:RelWithDebInfo>:/DEBUG>
            $<$<CONFIG:RelWithDebInfo>:/OPT:REF>
            $<$<CONFIG:RelWithDebInfo>:/OPT:ICF>
            $<$<CONFIG:RelWithDebInfo>:/INCREMENTAL:NO>
    )
endif ()

add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_CURRENT_BINARY_DIR}/.cache"
        VERBATIM
)

if (DEPLOY_DIR AND EXISTS "${DEPLOY_DIR}")
    set(PLUGIN_DIR "${DEPLOY_DIR}/SKSE/Plugins")

    add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying to: ${PLUGIN_DIR}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PLUGIN_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${PLUGIN_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${PLUGIN_DIR}"
            VERBATIM
    )
endif ()

