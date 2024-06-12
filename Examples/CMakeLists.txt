# CMakeLists.txt - Diyou.Engine
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_custom_target(Examples)

add_library(Shell.html INTERFACE)
target_link_options(Shell.html INTERFACE 
    --shell-file=${CMAKE_CURRENT_LIST_DIR}/Shell.html
)
set_target_properties(Shell.html
PROPERTIES
    INTERFACE_LINK_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/Shell.html
)

add_executable(Triangle Triangle.c++)
target_link_libraries(Triangle
PRIVATE
    Diyou::Lib
    $<$<BOOL:${EMSCRIPTEN}>:Shell.html>
)

add_executable(Cube Cube.c++)
target_link_libraries(Cube
PRIVATE
    Diyou::Lib
    $<$<BOOL:${EMSCRIPTEN}>:Shell.html>
)

add_dependencies(Examples
    Triangle
    Cube
)

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX .html)
    file(INSTALL webgpu-notext.svg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()