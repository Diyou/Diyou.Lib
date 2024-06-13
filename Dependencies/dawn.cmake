# dawn.cmake - Diyou.Engine
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(DAWN_TAG chromium/6536)
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
    
    GitTag(${CACHE_DIR}/dawn VERSION)
    if("${DAWN_TAG}" STREQUAL "${VERSION}")
      set(DAWN_FETCH_DEPENDENCIES OFF CACHE BOOL "" FORCE)
    endif()
#[[Slow]] # FIXME Depot_Tools on windows does not work
  else()
    set(_PATH ${CACHE_DIR}/depot_tools $ENV{PATH})
    cmake_path(CONVERT "${_PATH}" TO_NATIVE_PATH_LIST _PATH NORMALIZE)
    set(ENV{PATH} "${_PATH}")

    DeclareDependency(depot_tools
      https://chromium.googlesource.com/chromium/tools/depot_tools.git
      main
      $<$<BOOL:${WIN32}>:gclient>
    )
    
    DeclareDependency(dawn
      ${DAWN_URL}
      ${DAWN_TAG}
      "${CMAKE_COMMAND} -E copy scripts/standalone.gclient .gclient && gclient sync"
    )
  endif()
endif()