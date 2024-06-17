# ShaderMacros.cmake - Diyou.Lib
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

function(AddShaders TARGET)
set(options)
set(oneValueArgs)
set(multiValueArgs WGSL GLSL HLSL)
cmake_parse_arguments(F_ADD_SHADERS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

set(INTERMEDIATE_DIR ${CMAKE_CURRENT_BINARY_DIR}/intermediate/${TARGET})

# WGSL
foreach(WGSL_SHADER ${F_ADD_SHADERS_WGSL})
get_filename_component(FILE_NAME ${WGSL_SHADER} NAME_WLE)
file(STRINGS ${WGSL_SHADER} FILE_CONTENT NEWLINE_CONSUME)
string(REPLACE "\\;" "\;" FILE_CONTENT ${FILE_CONTENT})

set(SHADER_SOURCE "\
export module Shaders:WGSL_${FILE_NAME};
import Shaders;
char const* Shaders::WGSL::${FILE_NAME} =  R\"(\n${FILE_CONTENT}\n)\";\n"
)

WriteIfChanged(${INTERMEDIATE_DIR}/${FILE_NAME}.cpp "${SHADER_SOURCE}")
string(APPEND WGSL_SHADERS "\t\tstatic char const* ${FILE_NAME};\n")

list(APPEND SHADER_SOURCES ${INTERMEDIATE_DIR}/${FILE_NAME}.cpp)
set_target_properties(${TARGET}
PROPERTIES
    LINK_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/${WGSL_SHADER};${INTERMEDIATE_DIR}/${FILE_NAME}.cpp"
)
endforeach()

# GLSL
foreach(GLSL_SHADER ${F_ADD_SHADERS_GLSL})
get_filename_component(FILE_NAME ${GLSL_SHADER} NAME)
string(REPLACE "." "_" FILE_NAME ${FILE_NAME})
file(STRINGS ${GLSL_SHADER} FILE_CONTENT NEWLINE_CONSUME)
string(REPLACE "\\;" "\;" FILE_CONTENT ${FILE_CONTENT})

execute_process(COMMAND
glslc --target-spv=spv1.0 -mfmt=num -o - ${CMAKE_CURRENT_LIST_DIR}/${GLSL_SHADER}
OUTPUT_VARIABLE GLSL_SHADER_HEX
)
set(SHADER_SOURCE "\
module;
#include <vector>
export module Shaders:GLSL_${FILE_NAME};
import Shaders;
std::vector<unsigned> Shaders::GLSL::${FILE_NAME} {
${GLSL_SHADER_HEX}\
};
")

WriteIfChanged(${INTERMEDIATE_DIR}/${FILE_NAME}.glsl.cpp "${SHADER_SOURCE}")
string(APPEND GLSL_SHADERS "\t\tstatic std::vector<unsigned> ${FILE_NAME};\n")

list(APPEND SHADER_SOURCES ${INTERMEDIATE_DIR}/${FILE_NAME}.glsl.cpp)
set_target_properties(${TARGET}
PROPERTIES
    LINK_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/${GLSL_SHADER};${INTERMEDIATE_DIR}/${FILE_NAME}.glsl.cpp"
)
endforeach()

set(SHADER_MODULE "\
module;
#include <vector>
export module Shaders;
export namespace Shaders
{
    struct WGSL{
${WGSL_SHADERS}\
    };
    struct GLSL{
${GLSL_SHADERS}\
    };
}"
)

WriteIfChanged(${INTERMEDIATE_DIR}/Shaders.ixx "${SHADER_MODULE}")

string(TOLOWER ${TARGET} lower_target)
target_sources(${TARGET}
PUBLIC
    FILE_SET ${lower_target}_shader_module_files
    TYPE CXX_MODULES
    BASE_DIRS
        ${INTERMEDIATE_DIR}
    FILES
        ${INTERMEDIATE_DIR}/Shaders.ixx
        ${SHADER_SOURCES}  
)
endfunction()
