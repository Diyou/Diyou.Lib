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
set(multiValueArgs WGSL)
cmake_parse_arguments(F_ADD_SHADERS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

set(INTERMEDIATE_DIR ${CMAKE_CURRENT_BINARY_DIR}/intermediate/${TARGET})

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
string(APPEND SHADER_MODULES "\t\tstatic char const* ${FILE_NAME};\n")

list(APPEND SHADER_SOURCES ${INTERMEDIATE_DIR}/${FILE_NAME}.cpp)
set_target_properties(${TARGET}
PROPERTIES
    LINK_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/${WGSL_SHADER};${INTERMEDIATE_DIR}/${FILE_NAME}.cpp"
)

endforeach()

set(SHADER_MODULE "\
export module Shaders;
export namespace Shaders
{
    struct WGSL{
${SHADER_MODULES}\
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
