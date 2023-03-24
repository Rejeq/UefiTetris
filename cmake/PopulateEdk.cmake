function(ArchToEdk arch edkArch)
  if("${arch}" STREQUAL "x86_64")
    set(${edkArch} "X64" PARENT_SCOPE)
  elseif("${arch}" STREQUAL "i386" PARENT_SCOPE)
    set(${edkArch} "IA32" PARENT_SCOPE)
  elseif("${arch}" STREQUAL "aarch64" PARENT_SCOPE)
    set(${edkArch} "AArch64" PARENT_SCOPE)
  elseif("${arch}" STREQUAL "arm" PARENT_SCOPE)
    set(${edkArch} "ARM" PARENT_SCOPE)
  else()
    message(SEND_ERROR "Arch '${arch}' is not suppprted by edk")
  endif()
endfunction()

function(CompilerToEdk compiler edkCompiler)
  if("${compiler}" STREQUAL "Clang")
    set(${edkCompiler} "CLANG38" PARENT_SCOPE)
  elseif("${compiler}" STREQUAL "GNU")
    set(${edkCompiler} "GCC5" PARENT_SCOPE)
  elseif("${compiler}" STREQUAL "MSVC")
    if("${MSVC_TOOLSET_VERSION}" STREQUAL "90")
      set(${edkCompiler} "VS2008" PARENT_SCOPE)
    elseif("${MSVC_TOOLSET_VERSION}" STREQUAL "100")
      set(${edkCompiler} "VS2010" PARENT_SCOPE)
    elseif("${MSVC_TOOLSET_VERSION}" STREQUAL "110")
      set(${edkCompiler} "VS2012" PARENT_SCOPE)
    elseif("${MSVC_TOOLSET_VERSION}" STREQUAL "120")
      set(${edkCompiler} "VS2013" PARENT_SCOPE)
    elseif("${MSVC_TOOLSET_VERSION}" STREQUAL "140")
      set(${edkCompiler} "VS2015" PARENT_SCOPE)
    elseif("${MSVC_TOOLSET_VERSION}" STREQUAL "141")
      set(${edkCompiler} "VS2017" PARENT_SCOPE)
    else() # "${MSVC_TOOLSET_VERSION}" STREQUAL "142")
      set(${edkCompiler} "VS2019" PARENT_SCOPE)
    endif()
  else()
    message(SEND_ERROR "Compiler '${compiler}' is not supported by edk")
  endif()
endfunction()

function(BuildTypeToEdk buildType edkBuildType)
  string(TOUPPER ${buildType} buildType)
  set(${edkBuildType} "${buildType}" PARENT_SCOPE)
endfunction()

if(NOT TETRIS_EDK_REPO)
  message(FATAL_ERROR "Please provide a TETRIS_EDK_REPO variable")
endif()

option(EDK_SHORTCUT_DIR "Path to the to the EDK_REPO symlink. Must be a full path" "${TETRIS_BIN_DIR}/edk")
option(EDK_THREAD_COUNT "The number of concurrent threads at the build time")

if (NOT DEFINED ${EDK_THREAD_COUNT})
  set(EDK_THREAD_COUNT 0)
endif()

set(EDK_REPO "${TETRIS_EDK_REPO}") # Path to the EDK2 source tree
set(EDK_BUILD_DIR "${TETRIS_BIN_DIR}") # Path to the EDK2 build folder
set(EDK_WORKSPACE_DIR "${EDK_REPO}") # Path to the EDK2 workspace
                                     # where the Conf folder is located.
                                     # The same as the WORKSPACE environment variable

set(EDK_ARCH "")
ArchToEdk(${TETRIS_ARCH} EDK_ARCH)

set(EDK_CHAIN "")
CompilerToEdk(${CMAKE_CXX_COMPILER_ID} EDK_CHAIN)

set(EDK_BUILD_TYPE "")
BuildTypeToEdk(${CMAKE_BUILD_TYPE} EDK_BUILD_TYPE)

set(EDK_CONFIG_DIR "${TETRIS_DIR}/TetrisPkg" CACHE PATH "Path to the config folder that will be in EDK_PKG_DIR")
set(EDK_PKG_DIR "${EDK_REPO}/TetrisPkg" CACHE PATH "Path to the result TetrisPkg")
set(EDK_COMPILER_FLAGS "" CACHE STRING "Flags that passed to the compiler")


message(STATUS "Setup edk links...")
message(STATUS "Edk source tree: ${EDK_REPO}")

if(NOT EXISTS ${EDK_SHORTCUT_DIR} AND NOT "${EDK_SHORTCUT_DIR}")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${EDK_REPO} ${EDK_SHORTCUT_DIR}
  )
endif()

make_directory(${EDK_PKG_DIR})
execute_process(
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${TETRIS_SOURCE_DIR} ${EDK_PKG_DIR}/src
)

if("${EDK_BUILD_TYPE}" STREQUAL "DEBUG")
  if("${EDK_CHAIN}" MATCHES "VS") # if msvc compiler
    list(APPEND EDK_COMPILER_FLAGS "/Od")
  else()
    list(APPEND EDK_COMPILER_FLAGS "-O0")
  endif()
endif()


message(STATUS "Configure TetrisPkg...")
message(STATUS "Edk compiler flags: ${EDK_COMPILER_FLAGS}")
message(STATUS "Tetris definitions: ${TETRIS_DEFINITIONS}")

# TODO: Maybe exist a better approach to format list in the edk config files
string (REPLACE ";" "\n\t" EDK_SOURCE_SEPARATED "${TETRIS_SOURCE}")
string (REPLACE ";" " " EDK_COMPILER_FLAGS_SEPARATED "${EDK_COMPILER_FLAGS}")
string (REPLACE ";" " -D" EDK_DEFINITIONS_SEPARATED_GCC "${TETRIS_DEFINITIONS}")
string (REPLACE ";" " \\D" EDK_DEFINITIONS_SEPARATED_MSVC "${TETRIS_DEFINITIONS}")

configure_file(${EDK_CONFIG_DIR}/target.txt.in ${EDK_WORKSPACE_DIR}/Conf/target.txt)
configure_file(${EDK_CONFIG_DIR}/TetrisPkg.dsc.in ${EDK_PKG_DIR}/TetrisPkg.dsc)
configure_file(${EDK_CONFIG_DIR}/Tetris.inf.in ${EDK_PKG_DIR}/Tetris.inf)
