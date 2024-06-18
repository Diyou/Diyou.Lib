# wasm_support.cmake - Diyou.Lib
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

if(${BUILD_WASM})
set(EMSDK_TAG 3.1.61)
set(EMSCRIPTEN_TAG 3.1.61)

set(ESMDK_DIR ${CACHE_DIR}/emsdk)
set(EMSDK_COMMAND ${ESMDK_DIR}/emsdk)
set(EMSCRIPTEN_ROOT ${ESMDK_DIR}/upstream/emscripten)
set(EMSCRIPTEN_TOOLCHAIN ${EMSCRIPTEN_ROOT}/cmake/Modules/Platform/Emscripten.cmake)

AddDependencies(emsdk)

macro(UpdateEmscripten)
execute_process(COMMAND ${EMSDK_COMMAND} install ${EMSCRIPTEN_TAG})
execute_process(COMMAND ${EMSDK_COMMAND} activate ${EMSCRIPTEN_TAG})
endmacro()

if(NOT EXISTS ${EMSCRIPTEN_TOOLCHAIN})
    updateemscripten()
endif()

function(updateIfNeeded)
include(${EMSCRIPTEN_TOOLCHAIN})
if(NOT ${EMSCRIPTEN_TAG} STREQUAL ${EMSCRIPTEN_VERSION})
    updateemscripten()
endif()
endfunction()

updateIfNeeded()
include(${EMSCRIPTEN_TOOLCHAIN})

Message(STATUS "Dependency: emscripten/${EMSCRIPTEN_VERSION}")
set(CMAKE_TOOLCHAIN_FILE ${EMSCRIPTEN_TOOLCHAIN})

# Temporary fix until full emscripten support
set(CMAKE_C_COMPILER_CLANG_SCAN_DEPS ${ESMDK_DIR}/upstream/bin/clang-scan-deps)
set(CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS ${CMAKE_C_COMPILER_CLANG_SCAN_DEPS})

set(CMAKE_SYSROOT ${EMSCRIPTEN_SYSROOT})
set(CMAKE_C_COMPILER_TARGET wasm32-unknown-emscripten)
set(CMAKE_CXX_COMPILER_TARGET wasm32-unknown-emscripten)

add_library(emscripten INTERFACE)

# See https://github.com/emscripten-core/emscripten/blob/main/src/settings.js
target_link_options(emscripten
INTERFACE
    -sUSE_WEBGPU
    -sWASM_BIGINT
    -sNO_EXIT_RUNTIME
    --output_eol=linux
$<$<CONFIG:Debug>:
    -sSAFE_HEAP
    -sNO_DISABLE_EXCEPTION_CATCHING
    -fsanitize=undefined
    --emrun
>)
target_compile_options(emscripten
INTERFACE
    -fexceptions # for precompiled headers
$<$<CONFIG:Debug>:
    -fsanitize=undefined
>)

# https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer#security_requirements
if(BUILD_WASM_PTHREADS)
set(ENABLE_IPO OFF)
Message(STATUS "Multithreading: Enabled")
target_link_options(emscripten
INTERFACE
    -sENVIRONMENT=web,worker
    -pthread
    -sPROXY_TO_PTHREAD
    #-sOFFSCREENCANVAS_SUPPORT
    #-sOFFSCREENCANVASES_TO_PTHREAD=""
    # https://github.com/emscripten-core/emscripten/issues/16836#issuecomment-1925903719
    #-Wl,-u,_emscripten_run_callback_on_thread
    #-Wl,-u,_emscripten_set_offscreencanvas_size_on_thread
    -sMALLOC=mimalloc
    -sALLOW_MEMORY_GROWTH
    #-sPTHREAD_POOL_SIZE=4
    -sPTHREAD_POOL_SIZE_STRICT=0
)
add_compile_options(-pthread)
else()
target_link_options(emscripten
INTERFACE
    -sENVIRONMENT=web
    -sMALLOC=dlmalloc
)
endif()

#set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)
#[[target_precompile_headers(emscripten
INTERFACE
    <emscripten.h>
    <emscripten/html5.h>
)]]
endif()