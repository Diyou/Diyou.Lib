# dawn.cmake - Diyou.Engine
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(DAWN_TAG chromium/6536)
set(DAWN_URL https://dawn.googlesource.com/dawn.git)

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
#[[Slow]]
  else()
    if(WIN32)
      set(ENV{DEPOT_TOOLS_UPDATE} TRUE)
      set(WIN_TOOLS cmd /c gclient)
    endif()

    set(DEPOT_TOOLS_DIR ${CACHE_DIR}/depot_tools)
    AppendPath(BEFORE ${DEPOT_TOOLS_DIR})

    DeclareDependency(depot_tools
      https://chromium.googlesource.com/chromium/tools/depot_tools.git
      main
      PATCH ${WIN_TOOLS}
    )

    DeclareDependency(dawn
      ${DAWN_URL}
      ${DAWN_TAG}
      PATCH ${CMAKE_COMMAND} -E copy scripts/standalone.gclient .gclient && gclient sync
    )

    #[[Submodule approach
    set(DEPOT_TOOLS third_party/depot_tools)
    set(DEPOT_TOOLS_DIR ${CACHE_DIR}/dawn/${DEPOT_TOOLS})
    AppendPath(BEFORE ${DEPOT_TOOLS_DIR})

    DeclareDependency(dawn
      ${DAWN_URL}
      ${DAWN_TAG}
      SUBMODULES ${DEPOT_TOOLS}
      PATCH ${CMAKE_COMMAND} -E copy scripts/standalone.gclient .gclient ${WIN_TOOLS} && gclient sync
    )
    ]]
  endif()
endif()
