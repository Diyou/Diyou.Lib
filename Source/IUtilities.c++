/**
 * IUtilities.c++ - Diyou.Lib
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
module;
#ifdef __linux__
#include <bits/align.h>
#endif
#include <webgpu/webgpu_cpp.h>

#include <spirv-tools/libspirv.hpp>
#include <spirv-tools/optimizer.hpp>

#include <iostream>

export module Diyou:IUtilities;

import :Context;

using namespace std;
using namespace wgpu;

export struct IUtilities : public virtual Context
{
  SurfaceCapabilities Capabilities;
  vector<PresentMode> PresentModes;
  vector<CompositeAlphaMode> AlphaModes;
  vector<TextureFormat> TextureFormats;

  IUtilities()
  {
    surface.GetCapabilities(adapter, &Capabilities);
    PresentModes = vector<PresentMode>(
      Capabilities.presentModes,
      Capabilities.presentModes + Capabilities.presentModeCount);

    AlphaModes = vector<CompositeAlphaMode>(
      Capabilities.alphaModes,
      Capabilities.alphaModes + Capabilities.alphaModeCount);

    TextureFormats = vector<TextureFormat>(
      Capabilities.formats, Capabilities.formats + Capabilities.formatCount);
  }

  TextureView CurrentSurfaceView()
  {
    SurfaceTexture surfaceTexture;
    surface.GetCurrentTexture(&surfaceTexture);
    return surfaceTexture.texture.CreateView();
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

  ShaderModule createSPIRVShader(
    vector<uint32_t> const &code,
    char const *label = nullptr)
  {
    ShaderModuleSPIRVDescriptor spirv;
    spirv.code = code.data();
    spirv.codeSize = code.size();

    ShaderModuleDescriptor descriptor{.nextInChain = &spirv, .label = label};
    return device.CreateShaderModule(&descriptor);
  }

  ShaderModule createWGSLShader(char const *code, char const *label = nullptr)
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

  static vector<uint32_t> HLSLToSPIRV(string const &code)
  {
    cout << format("Converting:\n{}", code);

    auto log_error = [](
                       spv_message_level_t,
                       char const *,
                       spv_position_t const &,
                       char const *msg) { cerr << msg << endl; };

    spvtools::SpirvTools core(SPV_ENV_UNIVERSAL_1_3);
    spvtools::Optimizer opt(SPV_ENV_UNIVERSAL_1_3);

    core.SetMessageConsumer(log_error);
    opt.SetMessageConsumer(log_error);

    vector<uint32_t> spirv;
    core.Assemble(code, &spirv);
    core.Validate(spirv);

    return spirv;
  }
};
