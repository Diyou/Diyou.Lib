/**
 * Spirv.ixx - Diyou.Lib
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
module;
#include <string>
#include <vector>

export module Diyou.Spirv;

export namespace Spirv {
std::string
ToWGSL(std::vector<unsigned> const &spirv);
}
