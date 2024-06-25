/**
 * GLSL.c++ - Diyou.Lib
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
module;
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <SPIRV/GlslangToSpv.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>

export module Diyou.Spirv:GLSL;

using namespace std;
using namespace glslang;

namespace GLSL {
export vector<uint32_t>
ToSPIRV(vector<string> const &glsl)
{
  InitializeProcess();
  TShader parser(EShLanguage::EShLangFragment);

  parser.setStrings((char **)glsl.data(), (int)glsl.size());
  parser.setEnvInput(
    EShSourceGlsl, EShLanguage::EShLangFragment, EShClientOpenGL, 100);

  parser.setEnvClient(EShClientOpenGL, EShTargetOpenGL_450);
  parser.setEnvTarget(EShTargetSpv, EShTargetSpv_1_0);

  TBuiltInResource const *res = GetDefaultResources();
  parser.parse(res, 100, false, EShMessages::EShMsgDefault);

  cout << parser.getInfoLog() << "\n";

  TProgram program;
  program.addShader(&parser);
  program.link(EShMessages::EShMsgDefault);

  cout << program.getInfoLog() << "\n";

  TIntermediate &intermediate =
    *program.getIntermediate(EShLanguage::EShLangFragment);
  vector<uint32_t> spirv;
  GlslangToSpv(intermediate, spirv);

  return spirv;
}
}
