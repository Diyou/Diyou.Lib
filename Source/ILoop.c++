/**
 * ILoop.c++ - Diyou.Lib
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

#include <thread>

export module Diyou:ILoop;

import :Context;

using namespace std;

export class ILoop : public virtual Context
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
            auto &_this = *static_cast<ILoop *>(userData);
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
        auto &_this = *(ILoop *)userData;
        _this.Draw();
        return _this.rendering;
      },
      this);
#endif
#else
    loop = thread(&ILoop::Loop, this);
#endif
  }
};
