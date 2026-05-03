cmake_minimum_required(VERSION 3.16)

project(lcdream LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
endif()

set(LCDREAM_VENDOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
set(LCDREAM_GLAD_TARGET glad_gl_core_33)

if(NOT EXISTS "${LCDREAM_VENDOR_DIR}/glfw/CMakeLists.txt")
    message(FATAL_ERROR "Missing vendor/glfw. Run git submodule update --init --recursive.")
endif()

if(NOT EXISTS "${LCDREAM_VENDOR_DIR}/glm/CMakeLists.txt")
    message(FATAL_ERROR "Missing vendor/glm. Run git submodule update --init --recursive.")
endif()

if(NOT EXISTS "${LCDREAM_VENDOR_DIR}/glad/cmake/CMakeLists.txt")
    message(FATAL_ERROR "Missing vendor/glad. Run git submodule update --init --recursive.")
endif()

if(NOT EXISTS "${LCDREAM_VENDOR_DIR}/imgui/imgui.h")
    message(FATAL_ERROR "Missing vendor/imgui. Run git submodule update --init --recursive.")
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)

add_subdirectory("${LCDREAM_VENDOR_DIR}/glfw" "${CMAKE_CURRENT_BINARY_DIR}/vendor/glfw" EXCLUDE_FROM_ALL)
add_subdirectory("${LCDREAM_VENDOR_DIR}/glm" "${CMAKE_CURRENT_BINARY_DIR}/vendor/glm" EXCLUDE_FROM_ALL)
add_subdirectory("${LCDREAM_VENDOR_DIR}/glad/cmake" "${CMAKE_CURRENT_BINARY_DIR}/vendor/glad")

glad_add_library(${LCDREAM_GLAD_TARGET} STATIC REPRODUCIBLE API gl:core=3.3 EXTENSIONS NONE)

set(IMGUI_DIR "${LCDREAM_VENDOR_DIR}/imgui")
add_library(imgui STATIC
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(imgui PUBLIC "${IMGUI_DIR}" "${IMGUI_DIR}/backends")
target_link_libraries(imgui PUBLIC glfw)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lcdream/src/*.cpp"
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lcdream/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lcdream/include/*.hpp"
)

add_executable(lcdream ${SOURCES} ${HEADERS})

target_precompile_headers(lcdream PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/lcdream/include/pch.h>"
)

target_include_directories(lcdream PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lcdream/include"
)

target_compile_definitions(lcdream PRIVATE GLFW_INCLUDE_NONE)

target_link_libraries(lcdream PRIVATE
    glfw
    glm::glm
    ${LCDREAM_GLAD_TARGET}
    imgui
)
