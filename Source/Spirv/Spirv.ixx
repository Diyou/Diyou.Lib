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
#include <cstdint>
#include <string>
#include <vector>

export module Diyou.Spirv;

export namespace SPIRV {
std::string
ToWGSL(std::vector<uint32_t> const &spirv);
}

export namespace GLSL {
std::vector<uint32_t>
ToSPIRV(std::vector<std::string> const &glsl);
}
