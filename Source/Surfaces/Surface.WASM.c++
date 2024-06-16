/**
 * Surface.WASM.c++ - Diyou.Engine
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
module;
#include <SDL3/SDL.h>
#include <webgpu/webgpu_cpp.h>

export module Diyou:Surface;

using namespace std;
using namespace wgpu;

export inline Surface
SurfaceFromWindow(Instance const &instance, SDL_Window *window)
{
  auto props = SDL_GetWindowProperties(window);

  SurfaceDescriptorFromCanvasHTMLSelector chain;
  chain.selector = (char *)SDL_GetProperty(props, "🖼", nullptr);

  SurfaceDescriptor descriptor{.nextInChain = &chain};
  return instance.CreateSurface(&descriptor);
};
