# CMakeLists.txt - Diyou.Lib
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_library(${PROJECT_NAME}.Spirv STATIC)
target_compile_features(${PROJECT_NAME}.Spirv
PUBLIC
    cxx_std_23
)
target_sources(${PROJECT_NAME}.Spirv
PUBLIC
    FILE_SET module_files
    TYPE CXX_MODULES
    BASE_DIRS
        ${CMAKE_CURRENT_LIST_DIR}
    FILES
        Spirv.ixx
        Spirv.c++
        GLSL.c++
)

add_subdirectory(${CACHE_DIR}/dawn/third_party/glslang/src glslang)

target_link_libraries(${PROJECT_NAME}.Spirv
PRIVATE
    libtint
    glslang::glslang
    glslang::SPIRV
    glslang::glslang-default-resource-limits
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    ${PROJECT_NAME}.Spirv
)

if(EMSCRIPTEN)
target_link_options(${PROJECT_NAME}.Spirv
PUBLIC
    -sSTACK_SIZE=1MB
)
set_target_properties(tint_lang_core_constant
PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION FALSE
)
get_target_property(tint_libs libtint LINK_LIBRARIES)
foreach(tint_lib ${tint_libs})
    target_compile_options(${tint_lib} PRIVATE -Oz)
endforeach()

endif()