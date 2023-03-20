#include "Memory.h"

#if defined(TETRIS_EFI_PROVIDER_GNU_EFI)

#include <efilib.h>

#elif defined(TETRIS_EFI_PROVIDER_EDK)

#define uefi_call_wrapper(func, va_num, ...) func(__VA_ARGS__)

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#else
#error "Efi provider not provided"
#endif

#ifdef TETRIS_EFI_DEBUG_MEMORY

static int s_allocCount = 0;
static int s_freeCount = 0;

#endif  // TETRIS_EFI_DEBUG_MEMORY

void* Efi_Malloc(size_t size) {
  EFI_STATUS status = EFI_SUCCESS;
  void* out = NULL;

  status =
      uefi_call_wrapper(gBS->AllocatePool, 3, EfiBootServicesData, size, &out);
  if (EFI_ERROR(status))
    return NULL;

#ifdef TETRIS_EFI_DEBUG_MEMORY
  if (out != NULL)
    s_allocCount += 1;
#endif
  return out;
  // return AllocatePool(size);
}

void* Efi_Calloc(size_t num, size_t size) {
  void* out = Efi_Malloc(num * size);
  if (out == NULL)
    return NULL;

  Efi_Memset(out, 0, num * size);
  return out;
}

void Efi_Free(void* ptr) {
  EFI_STATUS status = EFI_SUCCESS;

  status = uefi_call_wrapper(gBS->FreePool, 1, ptr);
  if (EFI_ERROR(status)) {
    return;
  }
#ifdef TETRIS_EFI_DEBUG_MEMORY
  if (ptr != NULL)
    s_freeCount += 1;
#endif
}

#ifdef TETRIS_EFI_DEBUG_MEMORY
int Efi_DbgGetAllocateCount() {
  return s_allocCount;
}

int Efi_DbgGetFreeCount() {
  return s_freeCount;
}
#endif  // TETRIS_EFI_DEBUG_MEMORY

void Efi_Memset(void* dst, int ch, size_t count) {
  uefi_call_wrapper(gBS->SetMem, 3, dst, count, ch);
}

void Efi_Memcpy(void* dst, const void* src, size_t count) {
  uefi_call_wrapper(gBS->CopyMem, 3, dst, (void*)src, count);
}
