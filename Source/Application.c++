/**
 * Application.c++ - Diyou.Lib
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
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <filesystem>
#include <string>

export module Diyou:Application;
import :Window;

using namespace std;

export struct Application
{
  /// Absolute Path to the executable
  string const path;
  /// Filename of the executable
  string const name;

private:
  Application(int argc, char **argv)
  : path(filesystem::path(argv[0]).parent_path().string())
  , name(filesystem::path(argv[0]).filename().string())
  {
    // Attempt to reduce stutter on X11
    // \see https://forums.developer.nvidia.com/t/185015
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "1");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
      // Fixes high cpu load with nvidia proprietary drivers
      // Requires disabled compositor or fullscreen
      SDL_setenv("__GL_YIELD", "USLEEP", 1);
    }
  }
  int ProcessEvent(SDL_Event const &event)
  {
    switch (event.type) {
      case SDL_EVENT_MOUSE_MOTION: {
        Window::fromID(event.button.windowID).OnMouseMotion(event.motion);
      } break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        Window::fromID(event.button.windowID).OnMouseDown(event.button);
      } break;

      case SDL_EVENT_MOUSE_BUTTON_UP: {
        // windowID = 0 when releasing outside of canvas
        // causes Assertion to fail
        if (event.button.windowID > 0) {
          Window::fromID(event.button.windowID).OnMouseUp(event.button);
        }
      } break;

      case SDL_EVENT_KEY_DOWN: {
        Window::fromID(event.key.windowID).OnKeyDown(event.key);
      } break;

      case SDL_EVENT_KEY_UP: {
        Window::fromID(event.key.windowID).OnKeyUp(event.key);
      } break;

      case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
        Window::fromID(event.window.windowID).Close();
      } break;

      case SDL_EVENT_QUIT: {
        return SDL_APP_SUCCESS;
      } break;

      default:
        break;
    }
    return SDL_APP_CONTINUE;
  }

  virtual ~Application() { SDL_Quit(); }

  friend int SDL_AppInit(void **, int, char **);
  friend int SDL_AppEvent(void *, SDL_Event const *);
  friend int SDL_AppIterate(void *);
  friend void SDL_AppQuit(void *);
};

extern "C"
{
  /*  Consuming binaries need to implement
      void Init(Application const &){...}
      As a main function
   */
  void Init(Application const &);

  int SDL_AppInit(void **appstate, int argc, char **argv)
  {
    *appstate = new Application(argc, argv);
    auto &app = *static_cast<Application *>(*appstate);
    Init(app);
    return SDL_APP_CONTINUE;
  }

  int SDL_AppEvent(void *appstate, SDL_Event const *event)
  {
    auto &app = *static_cast<Application *>(appstate);
    return app.ProcessEvent(*event);
  }

  int SDL_AppIterate(void *appstate)
  {
    return SDL_APP_CONTINUE;
  }

  void SDL_AppQuit(void *appstate)
  {
    delete static_cast<Application *>(appstate);
  }
}
