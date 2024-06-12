# CMakeLists.txt - Diyou.Engine
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.28)

option(BUILD_SHARED_LIBS "Create shared objects" ON)
option(BUILD_WASM "Compile to WASM with Emscripten" OFF)
option(WASM_PTHREADS "Enable multithreading for WASM build" OFF)
option(ENABLE_IPO "If supported use Interprocedural Optimization" ON)
option(DAWN_FETCH_DEPENDENCIES "Use fetch_dawn_dependencies.py instead of depot_tools" OFF)

list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies
)

include(Macros)
include(WASM)

# clangd support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
Configure(CMake/.clangd ${CMAKE_CURRENT_SOURCE_DIR}/.clangd)

project(Diyou CXX)

include(IPO)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
AddDependencies(SDL3 glm dawn)

# Targets
add_library(${PROJECT_NAME})
add_library(Diyou::Lib ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME}
PUBLIC
    cxx_std_23
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
$<IF:$<BOOL:${EMSCRIPTEN}>,
    emscripten
,
    webgpu_dawn
>
    SDL3::SDL3
    glm::glm
)

#[[target_precompile_headers(${PROJECT_NAME}
PUBLIC
    <webgpu/webgpu_cpp.h>
    <iostream>
)]]

# Sources
add_subdirectory(Source)
add_subdirectory(Examples EXCLUDE_FROM_ALL)