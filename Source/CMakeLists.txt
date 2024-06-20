# CMakeLists.txt - Diyou.Lib
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

if(APPLE)
    set(PLATFORM_SUFFIX .Apple)
elseif(LINUX)
    set(PLATFORM_SUFFIX .Linux)
elseif(WIN32)
    set(PLATFORM_SUFFIX .Win)
elseif(EMSCRIPTEN)
    set(PLATFORM_SUFFIX .WASM)
elseif(ANDROID)
    set(PLATFORM_SUFFIX .Android)
else()
    Message(FATAL_ERROR "Platform(${CMAKE_SYSTEM_NAME}) not supported")
endif()

target_sources(${PROJECT_NAME}
PUBLIC
    FILE_SET module_files
    TYPE CXX_MODULES
    BASE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}
    FILES
        _Module.ixx
        Window.c++
        Application.c++
        Runtime.c++
        Surfaces/Surface${PLATFORM_SUFFIX}.c++
        Context.c++
        ILoop.c++
        IUtilities.c++ 
        #Surface.cpp  
)

add_library(${PROJECT_NAME}.Spirv STATIC)
target_compile_features(${PROJECT_NAME}.Spirv
PUBLIC
    cxx_std_23
)
target_sources(${PROJECT_NAME}.Spirv
PUBLIC
    FILE_SET module_files
    TYPE CXX_MODULES
    BASE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}
    FILES
        Spirv.ixx
        Spirv.c++
)
target_link_libraries(${PROJECT_NAME}.Spirv
PRIVATE
    libtint
)

if(EMSCRIPTEN)
target_link_options(${PROJECT_NAME}.Spirv
PUBLIC
    -sSTACK_SIZE=1MB
)
set_target_properties(tint_lang_core_constant
PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION FALSE
)
endif()

target_link_libraries(${PROJECT_NAME}
PRIVATE
    ${PROJECT_NAME}.Spirv
)
