/**
 * Context.c++ - Diyou.Lib
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

  static inline Surface GetSurface(
    Instance const &instance,
    Window const &window);

  static inline void Receive(
    unique_ptr<Window> &window,
    function<void(unique_ptr<Window> &, unique_ptr<Context> &)> callback)
  {
    struct UserData
    {
      unique_ptr<Window> window;
      unique_ptr<Context> context;
      function<void(unique_ptr<Window> &, unique_ptr<Context> &)> callback;
    };

    auto *userdata = new UserData(
      ::move(window), ::move(make_unique<Context>()), ::move(callback));

    auto &context = *userdata->context;
    auto &instance = context.instance;
    auto &surface = context.surface;

    // InstanceDescriptor descriptor{.features = {.timedWaitAnyEnable = true}};

    instance = CreateInstance();

    surface = GetSurface(instance, *userdata->window);

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

        auto &context = *userData.context;
        auto &adapter = context.adapter;

        adapter = Adapter::Acquire(cAdapter);

#ifdef __EMSCRIPTEN__
        DeviceDescriptor descriptor;
        descriptor.deviceLostCallback =
          [](WGPUDeviceLostReason reason, char const *message, void *userData)
        {
          auto &context = *static_cast<Context *>(userData);
          context.onDeviceLost(static_cast<DeviceLostReason>(reason), message);
        },
        descriptor.deviceLostUserdata = &context;
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

        DeviceDescriptor descriptor;
        descriptor.deviceLostCallbackInfo = lost;
        descriptor.uncapturedErrorCallbackInfo = error;
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
            auto &device = context->device;

            device = Device::Acquire(cDevice);

            callback(window, context);
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

Surface
Context::GetSurface(Instance const &instance, Window const &window)
{
  return SurfaceFromWindow(instance, window.handle);
}
