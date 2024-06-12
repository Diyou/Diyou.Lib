/**
 * Surface.Linux.c++ - Diyou.Engine
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

#include <cassert>

using namespace std;
using namespace wgpu;

export module Diyou:Surface;

export inline Surface
SurfaceFromWindow(Instance const &instance, SDL_Window *window)
{
  SurfaceDescriptor descriptor;
  auto props = SDL_GetWindowProperties(window);

  // X11
  if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
    SurfaceDescriptorFromXlibWindow chain;
    chain.display =
      SDL_GetProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
    chain.window =
      SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    descriptor.nextInChain = &chain;
    return instance.CreateSurface(&descriptor);
  }
  // wayland
  if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
    SurfaceDescriptorFromWaylandSurface chain;
    chain.display =
      SDL_GetProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
    chain.surface =
      SDL_GetProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
    descriptor.nextInChain = &chain;
    return instance.CreateSurface(&descriptor);
  }

  assert(descriptor.nextInChain != nullptr);
  return instance.CreateSurface(&descriptor);
}
