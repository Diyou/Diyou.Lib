/**
 * Surface.Apple.c++ - Diyou.Engine
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

using namespace std;
using namespace wgpu;

export module Diyou:Surface;

export inline Surface
SurfaceFromWindow(Instance const &instance, SDL_Window *window)
{
  auto props = SDL_GetWindowProperties(window);

  SurfaceDescriptorFromMetalLayer chain;

  auto cocoa_window =
    SDL_GetProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
  auto view = [cocoa_window contentView];

  [view setWantsLayer:YES];
  [view setLayer:[CAMetalLayer layer]];

  // Use retina if the window was created with retina support.
  [[view layer] setContentsScale:[cocoa_window backingScaleFactor]];

  chain.layer = [view layer];
  SurfaceDescriptor descriptor{.nextInChain = &chain};

  return instance.CreateSurface(&descriptor);
}
