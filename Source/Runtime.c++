/**
 * Runtime.c++ - Diyou.Lib
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
module;
#include <cassert>
#include <memory>
#include <vector>

export module Diyou:Runtime;
import :ILoop;

using namespace std;

export template<class T>
struct Runtime : public virtual ILoop
{
  void Close()
  {
    ILoop::Stop();

    auto position = find_if(
      Instances.begin(),
      Instances.end(),
      [this](auto &renderer) { return renderer.get() == this; });

    assert(position != Instances.end());

    Instances.erase(position);
  }
  static inline vector<unique_ptr<T>> Instances;
};
