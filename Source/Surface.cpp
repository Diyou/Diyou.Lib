/**
 * Surface.cpp - Diyou.Lib
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
module;
#include <SDL3/SDL_video.h>

#include <webgpu/webgpu_cpp.h>

#ifdef __APPLE__
#include <Foundation/Foundation.h>
#include <QuartzCore/CAMetalLayer.h>
#endif

export module Diyou:Context_Surface;
import :Window;
import :Context;

using namespace wgpu;

Surface
Context::GetSurface(Instance const &instance, Window const &window)
{
  SurfaceDescriptor descriptor;
  auto props = SDL_GetWindowProperties(window.handle);

#ifdef _WIN32
  SurfaceDescriptorFromWindowsHWND chain;
  chain.hwnd =
    SDL_GetProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

  chain.hinstance =
    SDL_GetProperty(props, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
#elifdef __APPLE__
  SurfaceDescriptorFromMetalLayer chain;

  auto cocoa_window =
    SDL_GetProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
  auto view = [cocoa_window contentView];

  [view setWantsLayer:YES];
  [view setLayer:[CAMetalLayer layer]];

  // Use retina if the window was created with retina support.
  [[view layer] setContentsScale:[cocoa_window backingScaleFactor]];

  chain.layer = [view layer];
#elifdef __linux__
  // X11
  ChainedStruct chain;
  if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
    SurfaceDescriptorFromXlibWindow _chain;
    _chain.display =
      SDL_GetProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
    _chain.window =
      SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    chain = _chain;
  }
  // wayland
  if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
    SurfaceDescriptorFromWaylandSurface _chain;
    _chain.display =
      SDL_GetProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
    _chain.surface =
      SDL_GetProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
    chain = _chain;
  }
#elifdef __ANDROID__
  SurfaceDescriptorFromAndroidNativeWindow chain;
  chain.window =
    SDL_GetProperty(props, SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);
#elifdef __EMSCRIPTEN__
  SurfaceDescriptorFromCanvasHTMLSelector chain;
  chain.selector = (char *)SDL_GetProperty(props, "🖼", nullptr);
#endif

  descriptor.nextInChain = &chain;
  return instance.CreateSurface(&descriptor);
}
