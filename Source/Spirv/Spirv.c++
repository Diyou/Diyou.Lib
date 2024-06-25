/**
 * Spirv.c++ - Diyou.Lib
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

#include <iostream>

#include <tint/tint.h>
export module Diyou.Spirv:Spirv;

using namespace std;

namespace SPIRV {
string
ToWGSL(vector<uint32_t> const &spirv)
{
  tint::spirv::reader::Options reader;
  auto program = tint::spirv::reader::Read(spirv, reader);

  if (program.Diagnostics().ContainsErrors()) {
    cout << program.Diagnostics().Str() << "\n";
  }

  tint::wgsl::writer::Options writer;
  auto result = tint::wgsl::writer::Generate(program, writer);
  return result->wgsl;
}
}
