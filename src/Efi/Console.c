#include "Console.h"

#if defined(TETRIS_EFI_PROVIDER_GNU_EFI)

#include <efi.h>
#include <efilib.h>

#elif defined(TETRIS_EFI_PROVIDER_EDK)

#define uefi_call_wrapper(func, va_num, ...) func(__VA_ARGS__)

#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include "Memory.h"

#else
#error "Efi provider not provided"
#endif

void Efi_SetConsoleAttribute(int32_t attr) {
  uefi_call_wrapper(gST->ConOut->SetAttribute, 2, gST->ConOut, attr);
}

void Efi_SetCursorPosition(int x, int y) {
  uefi_call_wrapper(gST->ConOut->SetCursorPosition, 3, gST->ConOut, x, y);
}

void Efi_SetCursor(int state) {
  uefi_call_wrapper(gST->ConOut->EnableCursor, 2, gST->ConOut, state);
}

int Efi_IsConsoleValidString(const char16_t* str) {
  EFI_STATUS status = EFI_SUCCESS;

  status = uefi_call_wrapper(gST->ConOut->TestString, 2, gST->ConOut,
                             (char16_t*)str);
  if (status == EFI_SUCCESS)
    return 1;
  return 0;
}

void Efi_GetConsoleSize(size_t* columns, size_t* rows) {
  EFI_STATUS status = EFI_SUCCESS;

  status = uefi_call_wrapper(gST->ConOut->QueryMode, 4, gST->ConOut,
                             gST->ConOut->Mode->Mode, columns, rows);
  if (EFI_ERROR(status)) {
    // TODO: Add error handler
  }
}

Efi_ConsoleMode* Efi_GetConsoleMode(void) {
  return gST->ConOut->Mode;
}

void Efi_ClearScreen(void) {
  uefi_call_wrapper(gST->ConOut->ClearScreen, 1, gST->ConOut);
}

void Efi_VPrint(const char16_t* fmt, va_list args) {
#if defined(TETRIS_EFI_PROVIDER_GNU_EFI)
  VPrint(fmt, args);
#elif defined(TETRIS_EFI_PROVIDER_EDK)
  char16_t* buff = NULL;
  size_t proccessed = 0;
  size_t buffSize = 0;

  buffSize = (PcdGet32(PcdUefiLibMaxPrintBufferSize) + 1) * sizeof(char16_t);

  buff = (char16_t*)Efi_Malloc(buffSize);
  if (buff == NULL) {
    return;
  }

  proccessed = UnicodeVSPrint(buff, buffSize, fmt, args);
  if (proccessed > 0) {
    uefi_call_wrapper(gST->ConOut->OutputString, 2, gST->ConOut, buff);
  }

  Efi_Free(buff);

#endif
}

void Efi_Print(const char16_t* fmt, ...) {
  va_list args;

  va_start(args, fmt);
  Efi_VPrint(fmt, args);
  va_end(args);
}

void Efi_Putc(int ch) {
  char16_t str[2] = {ch, L'\0'};
  Efi_Print(str);
}

int Efi_Getchar(void) {
  EFI_STATUS status = EFI_SUCCESS;
  EFI_INPUT_KEY key;

  status = uefi_call_wrapper(gST->ConIn->ReadKeyStroke, 2, gST->ConIn, &key);
  if (status != EFI_SUCCESS)
    return 0;

  if (key.ScanCode != 0)
    return key.ScanCode;
  if (key.UnicodeChar != 0)
    return (int)key.UnicodeChar;
  return 0;
}

int Efi_Getc(void) {
  EFI_STATUS status = EFI_SUCCESS;
  UINTN index = 0;

  status = uefi_call_wrapper(gST->BootServices->WaitForEvent, 3, 1,
                             &gST->ConIn->WaitForKey, &index);
  if (EFI_ERROR(status))
    return 0;

  return Efi_Getchar();
}

void Efi_WaitAnyKey(void) {
  EFI_STATUS status = EFI_NOT_READY;
  UINTN index = 0;
  EFI_INPUT_KEY key;

  status = uefi_call_wrapper(gST->BootServices->WaitForEvent, 3, 1,
                             &gST->ConIn->WaitForKey, &index);
  if (EFI_ERROR(status))
    return;
  status = uefi_call_wrapper(gST->ConIn->ReadKeyStroke, 2, gST->ConIn, &key);
  return;
}

void Efi_ResetInputKeys(void) {
  EFI_STATUS status =
      uefi_call_wrapper(gST->ConIn->Reset, 2, gST->ConIn, FALSE);
  if (EFI_ERROR(status))
    return;
  return;
}
