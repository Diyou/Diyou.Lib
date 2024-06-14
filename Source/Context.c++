/**
 * Context.c++ - Diyou.Engine
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
module;
#include <functional>
#include <iostream>

#include <webgpu/webgpu_cpp.h>

export module Diyou:Context;

import :Window;
import :Surface;

using namespace std;
using namespace wgpu;

export struct Context
{
  Instance instance;
  Surface surface;
  Adapter adapter;
  Device device;

  static inline void From(
    unique_ptr<Window> window,
    function<void(Window &&, Context &&)> const &callback)
  {
    struct UserData
    {
      unique_ptr<Window> window;
      Context context;
      function<void(Window &&, Context &&)> callback;
    };

    auto *userdata = new UserData(::move(window), {}, callback);

    auto &context = userdata->context;
    auto &instance = context.instance;
    auto &surface = context.surface;

    // InstanceDescriptor descriptor{.features = {.timedWaitAnyEnable = true}};

    instance = CreateInstance();

    surface = SurfaceFromWindow(instance, userdata->window->handle);

    RequestAdapterOptions options{
      .compatibleSurface = surface,
      .powerPreference = PowerPreference::HighPerformance,
      //  .backendType = BackendType::OpenGL
    };

    instance.RequestAdapter(
      &options,
      [](
        WGPURequestAdapterStatus status,
        WGPUAdapter cAdapter,
        char const *message,
        void *userdata)
      {
        auto &userData = *static_cast<UserData *>(userdata);
        if ((RequestAdapterStatus)status != RequestAdapterStatus::Success) {
          delete &userData;
          throw message;
        }

        auto &context = userData.context;
        auto &adapter = context.adapter;

        adapter = Adapter::Acquire(cAdapter);

#ifdef __EMSCRIPTEN__
        DeviceDescriptor descriptor{
          .deviceLostCallback =
            [](WGPUDeviceLostReason reason, char const *message, void *userData)
          {
            auto &context = *static_cast<Context *>(userData);
            context.onDeviceLost(
              static_cast<DeviceLostReason>(reason), message);
          },
          .deviceLostUserdata = &context};
#else
        DeviceLostCallbackInfo lost{
          .callback =
            [](
              WGPUDevice const *device,
              WGPUDeviceLostReason reason,
              char const *message,
              void *userData)
          {
            auto &context = *static_cast<Context *>(userData);
            context.onDeviceLost(
              static_cast<DeviceLostReason>(reason), message);
          },
          .userdata = &context};
        UncapturedErrorCallbackInfo error{
          .callback =
            [](WGPUErrorType type, char const *message, void *userData)
          {
            auto &context = *static_cast<Context *>(userData);
            context.onError(static_cast<ErrorType>(type), message);
          },
          .userdata = &context};
        DeviceDescriptor descriptor{
          .deviceLostCallbackInfo = lost, .uncapturedErrorCallbackInfo = error};
#endif
        adapter.RequestDevice(
          &descriptor,
          [](
            WGPURequestDeviceStatus status,
            WGPUDevice cDevice,
            char const *message,
            void *userdata)
          {
            auto &userData = *static_cast<UserData *>(userdata);
            if ((RequestDeviceStatus)status != RequestDeviceStatus::Success) {
              delete &userData;
              throw message;
            }

            auto &window = userData.window;
            auto &context = userData.context;
            auto &callback = userData.callback;
            auto &device = context.device;

            device = Device::Acquire(cDevice);

            callback(::move(*window), ::move(context));
            delete &userData;
          },
          userdata);
      },
      userdata);
  }

private:
  void onDeviceLost(DeviceLostReason const &reason, string_view message) const
  {
    switch (reason) {
#ifndef __EMSCRIPTEN__
      case DeviceLostReason::InstanceDropped:
        // Expected during unwinding
        break;
      case DeviceLostReason::FailedCreation:
#endif
      case DeviceLostReason::Destroyed:
      case DeviceLostReason::Unknown:
        cout << format("Device: {}\n", message);
        break;
    }
  }

  void onError(ErrorType const &error, string_view message) const
  {
    cerr << format("Error: {}\n", message);
  }
};
