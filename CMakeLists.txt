cmake_minimum_required(VERSION 3.16)

project(UefiTetris)

option(TETRIS_EFI_PROVIDER "Select efi provider, can be 'gnu-efi' or 'edk'" "")
option(TETRIS_ARCH
  "Target architecture. The Efi provider may not support some of these, can be: x86_64, ia32, ia64, arm, aarch64, riscv64, ebc, ipf"
  "")
option(TETRIS_FETCHING "Enable fetching" ON)
option(TETRIS_EDK_REPO "Path to the EDK source tree. If defined and TETRIS_EFI_PROVIDER is not specified, it will be overriden as edk" "")
option(TETRIS_LOGGING "Enable logging" OFF)
option(TETRIS_DEBUG_MEMORY "Enable counting of malloc/free" OFF)

set(TETRIS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TETRIS_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(TETRIS_SCRIPTS_DIR ${TETRIS_DIR}/scripts)
set(TETRIS_SOURCE_DIR ${TETRIS_DIR}/src)

list(APPEND CMAKE_MODULE_PATH "${TETRIS_DIR}/cmake")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if (NOT TETRIS_ARCH)
  message(FATAL_ERROR "Please provide a TETRIS_ARCH variable")
endif()

set(EFI_SOURCE
  "src/Efi/Console.h"
  "src/Efi/Console.c"
  "src/Efi/Memory.h"
  "src/Efi/Memory.c"
  "src/Efi/Misc.h"
  "src/Efi/Misc.c"
  "src/Efi/Stdint.h"
)

set(TETRIS_SOURCE
  ${EFI_SOURCE}
  "src/Main.c"
  "src/Tetris.c"
  "src/Tetris.h"
  "src/Tetromino.c"
  "src/Tetromino.h"
  "src/Ui.c"
  "src/Ui.h"
  "src/Board.c"
  "src/Board.h"
  "src/Renderer.h"
  "src/RendererConsole.c"
  "src/Log.h"
  "src/Log.c"
)

set(TETRIS_DEFINITIONS "")
list(APPEND TETRIS_DEFINITIONS "TETRISAPI=EFIAPI")

if (TETRIS_LOGGING)
  list(APPEND TETRIS_DEFINITIONS "TETRIS_LOG_ENABLED")
  message(STATUS "Logging enabled")
endif()

if(TETRIS_DEBUG_MEMORY)
  list(APPEND TETRIS_DEFINITIONS "TETRIS_EFI_DEBUG_MEMORY")
  message(STATUS "Memory debugging enabled")
endif()


if(NOT TETRIS_EFI_PROVIDER AND TETRIS_EDK_REPO)
  set(TETRIS_EFI_PROVIDER "edk")
endif()

string(TOLOWER "${TETRIS_EFI_PROVIDER}" TETRIS_EFI_PROVIDER)
message(STATUS "Building wtih efi provider: '${TETRIS_EFI_PROVIDER}'")

if(TETRIS_EFI_PROVIDER STREQUAL "gnu-efi")
  list(APPEND TETRIS_DEFINITIONS "TETRIS_EFI_PROVIDER_GNU_EFI")
  include(cmake/PopulateGnuefi.cmake)
elseif(TETRIS_EFI_PROVIDER STREQUAL "edk")
  list(APPEND TETRIS_DEFINITIONS "TETRIS_EFI_PROVIDER_EDK")
  include(cmake/PopulateEdk.cmake)
else()
  message(FATAL_ERROR "Please provide a correct TETRIS_EFI_PROVIDER variable")
endif()
