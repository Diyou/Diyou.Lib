/**
 * AbstractRenderer.c++ - Diyou.Engine
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
module;
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <iostream>
#include <thread>

#include <webgpu/webgpu_cpp.h>

export module Diyou:IRenderer;

import :Context;

using namespace std;
using namespace wgpu;

export class IRenderer : public virtual Context
{
  bool rendering = false;
  thread loop;

  void Loop()
  {
    while (rendering) {
      Draw();
      surface.Present();
      instance.ProcessEvents();
    }
  }

protected:
  virtual void Draw() = 0;

  void Stop()
  {
    rendering = false;
    loop.join();
  }

  void Start()
  {
    if (rendering) {
      return;
    }
    rendering = true;
#ifdef __EMSCRIPTEN__
#ifdef __EMSCRIPTEN_PTHREADS__
    loop = thread(
      [this]()
      {
        emscripten_set_main_loop_arg(
          [](void *userData)
          {
            auto &_this = *static_cast<IRenderer *>(userData);
            _this.Draw();
            _this.surface.Present();
            _this.instance.ProcessEvents();
          },
          this,
          0,
          true);
      });
#else
    emscripten_request_animation_frame_loop(
      [](double time, void *userData) -> EM_BOOL
      {
        auto &_this = *(IRenderer *)userData;
        _this.Draw();
        return _this.rendering;
      },
      this);
#endif
#else
    loop = thread(&IRenderer::Loop, this);
#endif
  }

  // TODO Update to AdapterInfo in emscripten 3.1.62
  [[nodiscard]]
  AdapterProperties getAdapterProperties() const
  {
#ifndef __EMSCRIPTEN__
    AdapterInfo info;
    adapter.GetInfo(&info);
#endif

    AdapterProperties properties;
    adapter.GetProperties(&properties);
    return properties;
  }

public:
  ShaderModule createShader(char const *code, char const *label = nullptr)
  {
    ShaderModuleWGSLDescriptor wgsl;
    wgsl.code = code;

    ShaderModuleDescriptor descriptor{.nextInChain = &wgsl, .label = label};

    return device.CreateShaderModule(&descriptor);
  }

  Buffer createBuffer(void const *data, size_t size, BufferUsage usage)
  {
    BufferDescriptor descriptor{
      .usage = BufferUsage::CopyDst | usage, .size = size};

    Buffer buffer = device.CreateBuffer(&descriptor);
    device.GetQueue().WriteBuffer(buffer, 0, data, size);
    return buffer;
  }
};
