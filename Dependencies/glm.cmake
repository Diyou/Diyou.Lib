# glm.cmake - Diyou.Engine
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(GLM_ENABLE_CXX_20 ON CACHE BOOL "Build glm with c++20" FORCE)
DeclareDependency(glm https://github.com/g-truc/glm 1.0.1)