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
    BINARY_DIR        ${NAME}
    PREFIX            ${CACHE_DIR}/.prefix/${NAME}
    SOURCE_DIR        ${CACHE_DIR}/${NAME}
    SUBBUILD_DIR      .cache/${NAME}
    USES_TERMINAL_DOWNLOAD TRUE
    PATCH_COMMAND ${PATCH}
  )
  list(APPEND FetchList ${NAME})
endmacro()

macro(AddDependencies Dependencies)
  set(_LIST ${Dependencies} ${ARGN})
  set(FetchList "")
  foreach(dependency ${_LIST})
    include(${dependency})
  endforeach()
  FetchContent_MakeAvailable(${FetchList})
endmacro()

function(GitTag DIR OUT)
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${DIR}
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  if("${VERSION}" STREQUAL "")
  execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
    WORKING_DIRECTORY ${DIR}
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  endif()
  set(${OUT} ${VERSION} PARENT_SCOPE)
endfunction()

function(Configure FILE_IN FILE_OUT)
set(COPY_REMARK "\
# Do not edit!
# This file was auto-generated
# Source: ${FILE_IN}
")
configure_file(${FILE_IN} ${FILE_OUT} @ONLY NEWLINE_STYLE UNIX)
endfunction()