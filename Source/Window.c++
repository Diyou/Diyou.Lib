/**
 * Window.c++ - Diyou.Lib
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found
 * in the LICENSE file in the root directory of this source
 * tree.
 */
module;
#include <SDL3/SDL.h>

#include <cassert>
#include <iostream>
#include <string>
#include <utility>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

export module Diyou:Window;

using namespace std;

constexpr char const *RuntimeResolver = "🪟";

export class Window
{
  friend struct Application;
  friend struct Context;

  SDL_Window *handle;

  /// \brief used for emscripten
  string querySelector;

protected:
  SDL_PropertiesID properties;

  virtual void Close() { SDL_DestroyWindow(handle); };

  virtual void OnMouseMotion(SDL_MouseMotionEvent const &event) {}
  virtual void OnMouseDown(SDL_MouseButtonEvent const &event){};
  virtual void OnMouseUp(SDL_MouseButtonEvent const &event){};

  virtual void OnKeyDown(SDL_KeyboardEvent const &event)
  {
    SDL_Scancode const &code = event.scancode;
    SDL_Keymod const &mod = event.mod;
    // Toggle fullscreen
    if (
      event.repeat == 0 && (mod & SDL_KMOD_ALT) > 0
      && (code == SDL_SCANCODE_KP_ENTER || code == SDL_SCANCODE_RETURN))
    {
      SDL_WindowFlags flags = SDL_GetWindowFlags(handle);
      bool isNotFullscreen = (flags & SDL_WINDOW_FULLSCREEN) == 0;
      SDL_SetWindowFullscreen(handle, static_cast<SDL_bool>(isNotFullscreen));
    }
  }
  virtual void OnKeyUp(SDL_KeyboardEvent const &event) {}

public:
  Window(string const &title, unsigned const width, unsigned const height)
  {
    assert(
      width <= numeric_limits<int>::max()
      && height <= numeric_limits<int>::max());

#ifdef __EMSCRIPTEN__
    int canvasCount = MAIN_THREAD_EM_ASM_INT({
      const container= document.querySelector('#canvasContainer');
      const canvas= document.createElement('canvas');
      const count = document.getElementsByTagName('canvas').length;

      canvas.oncontextmenu = e => e.preventDefault();
      canvas.id = `canvas${count}`;
      container.appendChild(canvas);
      return count;
    });
    querySelector = format("canvas#canvas{}", canvasCount);
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR, querySelector.c_str());
#endif
    handle = SDL_CreateWindow(
      title.c_str(),
      static_cast<int>(width),
      static_cast<int>(height),
      SDL_WINDOW_RESIZABLE);
    properties = SDL_GetWindowProperties(handle);

    SDL_SetProperty(properties, RuntimeResolver, this);
    SDL_SetProperty(properties, "🖼", querySelector.data());
  }

  [[nodiscard]]
  string_view GetTitle() const
  {
    return SDL_GetWindowTitle(handle);
  }

  void SetTitle(string const &title)
  {
    SDL_SetWindowTitle(handle, title.c_str());
  }

  void GetSize(unsigned &width, unsigned &height) const
  {
    SDL_GetWindowSize(
      handle,
      reinterpret_cast<int *>(&width),
      reinterpret_cast<int *>(&height));
  }

  void SetSize(unsigned const width, unsigned const height)
  {
    assert(
      width <= numeric_limits<int>::max()
      && height <= numeric_limits<int>::max());

    SDL_SetWindowSize(
      handle, static_cast<int>(width), static_cast<int>(height));
  }

  static inline Window &fromID(SDL_WindowID const &windowID)
  {
    auto *sdl_window = SDL_GetWindowFromID(windowID);
    auto *window = SDL_GetProperty(
      SDL_GetWindowProperties(sdl_window), RuntimeResolver, nullptr);
    assert(window);
    return *static_cast<Window *>(window);
  }

  virtual ~Window() { Close(); }

  Window(Window const &) = delete;

  Window(Window &&other) noexcept
  : handle(exchange(other.handle, nullptr))
  , properties(other.properties)
  , querySelector(::move(other.querySelector))
  {
    SDL_SetProperty(properties, RuntimeResolver, this);
  };
};
