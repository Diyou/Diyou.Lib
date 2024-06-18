/**
 * Surface.Android.c++ - Diyou.Lib
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
  SurfaceDescriptor descriptor;
  auto props = SDL_GetWindowProperties(window);

  SurfaceDescriptorFromAndroidNativeWindow chain;
  chain.window =
    SDL_GetProperty(props, SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);

  descriptor.nextInChain = &chain;
  return instance.CreateSurface(&descriptor);
}
