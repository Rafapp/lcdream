cmake_minimum_required(VERSION 3.16)

project(lcdream LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lcdream/src/*.cpp"
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lcdream/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lcdream/include/*.hpp"
)

add_executable(lcdream ${SOURCES} ${HEADERS})

target_include_directories(lcdream PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lcdream/include"
)
