# macros.cmake - Diyou.Engine
# 
# Copyright (c) 2024 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(CACHE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.cache)

find_package(Git REQUIRED)
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

macro(DeclareDependency NAME URL TAG)
  set(PATCH ${ARGV3})
  message(STATUS "Adding Dependency: ${NAME}")

  FetchContent_Declare(${NAME}
    GIT_REPOSITORY    ${URL}
    GIT_TAG           ${TAG}
    GIT_SHALLOW       TRUE
    GIT_SUBMODULES    ""
    GIT_PROGRESS      TRUE
    EXCLUDE_FROM_ALL  TRUE
    BINARY_DIR        ${CMAKE_CURRENT_BINARY_DIR}/${NAME}
    PREFIX            ${CACHE_DIR}/${NAME}/prefix
    SOURCE_DIR        ${CACHE_DIR}/${NAME}/source
    SUBBUILD_DIR      ${CACHE_DIR}/${NAME}/subbuild
    USES_TERMINAL_DOWNLOAD TRUE
    PATCH_COMMAND ${PATCH}
  )
  set(FetchList "${FetchList};${NAME}")
endmacro()

macro(AddDependencies Dependencies)
  set(_LIST ${Dependencies} ${ARGN})
  set(FetchList "")
  foreach(dependency ${_LIST})
    include(${dependency})
  endforeach()
  FetchContent_MakeAvailable(${FetchList})
endmacro()

function(Configure FILE_IN FILE_OUT)
set(COPY_REMARK "\
# Do not edit!
# This file was auto-generated
# Source: ${FILE_IN}
")
configure_file(${FILE_IN} ${FILE_OUT} @ONLY NEWLINE_STYLE UNIX)
endfunction()