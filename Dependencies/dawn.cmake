# dawn.cmake - Diyou.Engine
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(DAWN_TAG chromium/6530)
set(DAWN_URL https://dawn.googlesource.com/dawn)

set(DAWN_ENABLE_DESKTOP_GL OFF)
set(DAWN_ENABLE_OPENGLES OFF)
set(DAWN_ENABLE_NULL OFF)
set(DAWN_BUILD_SAMPLES OFF)
set(DAWN_BUILD_BENCHMARKS OFF)
set(TINT_BUILD_TESTS OFF)
set(TINT_BUILD_BENCHMARKS OFF)
set(TINT_BUILD_CMD_TOOLS OFF)

set(DAWN_USE_GLFW OFF)
if(LINUX)
  set(DAWN_USE_X11 ON)
  set(DAWN_USE_WAYLAND ON)
endif()

if(NOT EMSCRIPTEN)
#[[Quick]]
  if(DAWN_FETCH_DEPENDENCIES)
    DeclareDependency(dawn ${DAWN_URL} ${DAWN_TAG})
    
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
      WORKING_DIRECTORY ${CACHE_DIR}/dawn/source
      OUTPUT_VARIABLE VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    if("${DAWN_TAG}" STREQUAL "${VERSION}")
      set(DAWN_FETCH_DEPENDENCIES OFF)
    endif()
#[[Slow]]
  else()
    DeclareDependency(depot_tools
      https://chromium.googlesource.com/chromium/tools/depot_tools
      main
    )
    message(STATUS "Adding Dependency: dawn")
    FetchContent_Declare(dawn
      GIT_REPOSITORY    ${DAWN_URL}
      GIT_TAG           ${DAWN_TAG}
      GIT_SHALLOW       TRUE
      GIT_SUBMODULES    ""
      GIT_PROGRESS      TRUE
      EXCLUDE_FROM_ALL  TRUE
      BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR}/dawn
      PREFIX            ${CACHE_DIR}/dawn/prefix
      SOURCE_DIR        ${CACHE_DIR}/dawn/source
      SUBBUILD_DIR      ${CMAKE_CURRENT_BINARY_DIR}/.cache/dawn
      USES_TERMINAL_DOWNLOAD    TRUE
      USES_TERMINAL_PATCH       TRUE
      PATCH_COMMAND ${CMAKE_COMMAND} -E copy scripts/standalone.gclient .gclient && ${CACHE_DIR}/depot_tools/source/gclient sync
    )
    set(FetchList "${FetchList};dawn")
  endif()
endif()