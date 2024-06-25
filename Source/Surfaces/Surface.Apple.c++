/**
 * Surface.Apple.c++ - Diyou.Lib
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

#include <Foundation/Foundation.h>
#include <QuartzCore/CAMetalLayer.h>

export module Diyou:Surface;

using namespace std;
using namespace wgpu;

export inline Surface
SurfaceFromWindow(Instance const &instance, SDL_Window *window)
{
  auto props = SDL_GetWindowProperties(window);

  SurfaceDescriptorFromMetalLayer chain;

  NSWindow &cocoa_window =
    *SDL_GetProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);

  NSView &view = *cocoa_window.contentView;

  CAMetalLayer layer;
  view.setWantsLayer(true);
  view.setLayer(&layer);

  // Use retina if the window was created with retina support.
  layer.setContentsScale(cocoa_window.backingScaleFactor);

  chain.layer = &layer;
  SurfaceDescriptor descriptor{.nextInChain = &chain};

  return instance.CreateSurface(&descriptor);
}
