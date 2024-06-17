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

function(DeclareDependency NAME URL TAG)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs PATCH SUBMODULES)
  cmake_parse_arguments(FUNC_DDEP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  Message(STATUS "Adding Dependency: ${NAME}")

  FetchContent_Declare(${NAME}
    GIT_REPOSITORY    ${URL}
    GIT_TAG           ${TAG}
    GIT_SHALLOW       TRUE
    GIT_SUBMODULES    "${FUNC_DDEP_SUBMODULES}"
    GIT_PROGRESS      TRUE
    UPDATE_DISCONNECTED TRUE
    EXCLUDE_FROM_ALL  TRUE
    BINARY_DIR        ${NAME}
    PREFIX            ${CACHE_DIR}/.prefix/${NAME}
    SOURCE_DIR        ${CACHE_DIR}/${NAME}
    SUBBUILD_DIR      ${CACHE_DIR}/.prefix/${NAME}/sub
    PATCH_COMMAND     ${FUNC_DDEP_PATCH}
  )
  set(_LIST ${FetchList})
  list(APPEND _LIST ${NAME})
  set(FetchList ${_LIST} PARENT_SCOPE)
endfunction()

function(AddDependencies Dependencies)
  set(_LIST ${Dependencies} ${ARGN})
  set(FetchList PARENT_SCOPE)

  foreach(dependency ${_LIST})
    include(${dependency})
  endforeach()

  FetchContent_MakeAvailable(${FetchList})
endfunction()

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

function(AppendPath)
  set(options)
  set(oneValueArgs PATCH)
  set(multiValueArgs AFTER BEFORE)

  cmake_parse_arguments(FUNC_APATH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(_PATH ${FUNC_APATH_BEFORE} $ENV{PATH} ${FUNC_APATH_AFTER})
  cmake_path(CONVERT "${_PATH}" TO_NATIVE_PATH_LIST _PATH NORMALIZE)
  set(ENV{PATH} "${_PATH}") 
endfunction()

function(WriteIfChanged DESTINATION TEXT)
set(Changed FALSE)
if(EXISTS ${DESTINATION})
  file(STRINGS ${DESTINATION} compare_string NEWLINE_CONSUME)
  string(REPLACE "\\;" "\;" compare_string ${compare_string})
  string(COMPARE EQUAL
    "${compare_string}"
    "${TEXT}"
    Changed
  )
endif()
if(NOT ${Changed})
  file(WRITE ${DESTINATION} "${TEXT}")
endif()
endfunction()

function(Configure FILE_IN FILE_OUT)
set(COPY_REMARK "\
# Do not edit!
# This file was auto-generated
# Source: ${FILE_IN}
")
configure_file(${FILE_IN} ${FILE_OUT} @ONLY NEWLINE_STYLE UNIX)
endfunction()