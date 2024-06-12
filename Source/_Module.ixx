/**
 * _Module.ixx - Diyou.Engine
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
module;
#ifndef __EMSCRIPTEN__
#include <bits/unicode.h>
#endif

export module Diyou;

export import :Runtime;
export import :Window;
export import :Context;
export import :AbstractRenderer;
