/**
 * Surface.Win.c++ - Diyou.Engine
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

using namespace std;
using namespace wgpu;

export module Diyou:Surface;

export inline Surface
SurfaceFromWindow(Instance const &instance, SDL_Window *window)
{
  auto props = SDL_GetWindowProperties(window);

  SurfaceDescriptorFromWindowsHWND chain;
  chain.hwnd =
    SDL_GetProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

  chain.hinstance =
    SDL_GetProperty(props, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);

  SurfaceDescriptor descriptor{.nextInChain = &chain};

  return instance.CreateSurface(&descriptor);
}
