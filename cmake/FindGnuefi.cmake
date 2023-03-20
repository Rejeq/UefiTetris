if (TETRIS_FETCHING AND NOT DEFINED GNU_EFI_DIR)
  set(GNU_EFI_URL "https://sourceforge.net/projects/gnu-efi/files/gnu-efi-3.0.15.tar.bz2/download")
  message(STATUS "Loading GNU_EFI from ${GNU_EFI_URL}")

  include(FetchContent)
  FetchContent_Declare(GNU_EFI
    URL ${GNU_EFI_URL}
    DOWNLOAD_EXTRACT_TIMESTAMP ON
  )

  FetchContent_GetProperties(GNU_EFI)
  if(NOT GNU_EFI_POPULATED)
    FetchContent_Populate(GNU_EFI)
    set(GNU_EFI_DIR "${gnu_efi_SOURCE_DIR}")
  endif()
endif()

if(NOT DEFINED GNU_EFI_DIR)
  message(FATAL_ERROR "Unable to find gnu-efi library, provide GNU_EFI_DIR or enable fetching")
endif()

message(STATUS "gnu-efi directory: ${GNU_EFI_DIR}/")
set(GNU_EFI_INC_DIR ${GNU_EFI_DIR}/inc)
set(GNU_EFI_LIB_DIR ${GNU_EFI_DIR}/lib)

if(NOT EXISTS ${GNU_EFI_LIB_DIR}/${TETRIS_ARCH})
  message(FATAL_ERROR "gnu-efi dosen't support the '${TETRIS_ARCH}' architecture")
endif()

file(GLOB GNU_EFI_SRC
  ${GNU_EFI_LIB_DIR}/*.c
  ${GNU_EFI_LIB_DIR}/runtime/*.c
  ${GNU_EFI_LIB_DIR}/${TETRIS_ARCH}/*.c
  ${GNU_EFI_LIB_DIR}/${TETRIS_ARCH}/*.S
)

add_library(gnuefi STATIC)

target_sources(gnuefi PRIVATE ${GNU_EFI_SRC})

target_include_directories(gnuefi PUBLIC
  ${GNU_EFI_INC_DIR}
  ${GNU_EFI_INC_DIR}/protocol
  ${GNU_EFI_INC_DIR}/${TETRIS_ARCH}
)

target_compile_options(gnuefi PUBLIC
  --target=${TETRIS_ARCH}-unknown-windows
  -fshort-wchar
  -mno-red-zone
  -ffreestanding
)


target_link_options(gnuefi PUBLIC
  --target=${TETRIS_ARCH}-unknown-windows
  -nostdlib
  --warn-common
  -Wl,-entry:efi_main
  -Wl,-subsystem:efi_application
  -fuse-ld=lld
)

target_compile_definitions(gnuefi PRIVATE GNU_EFI_USE_MS_ABI)

if ("${TETRIS_ARCH}" STREQUAL "x86_64")
  target_compile_definitions(gnuefi PRIVATE EFI_FUNCTION_WRAPPER)
endif()

message(STATUS "gnu-efi configured successfully")
