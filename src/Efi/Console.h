#ifndef TETRIS_EFI_CONSOLE_H
#define TETRIS_EFI_CONSOLE_H

#include "Stdint.h"

#if defined(TETRIS_EFI_PROVIDER_GNU_EFI)

typedef SIMPLE_TEXT_OUTPUT_MODE Efi_ConsoleMode;

#elif defined(TETRIS_EFI_PROVIDER_EDK)

#include <Protocol/SimpleTextOut.h>
#include <Protocol/SimpleTextIn.h>

typedef EFI_SIMPLE_TEXT_OUTPUT_MODE Efi_ConsoleMode;

#endif

void TETRISAPI Efi_SetConsoleAttribute(int32_t attr);
void TETRISAPI Efi_SetCursorPosition(int x, int y);
void TETRISAPI Efi_SetCursor(int state);

void TETRISAPI Efi_GetConsoleSize(size_t* columns, size_t* rows);
Efi_ConsoleMode* Efi_GetConsoleMode(void);

void TETRISAPI Efi_ClearScreen(void);

// Checks string can be printed or not.
// Return 1 on success, otherwise - 0
int TETRISAPI Efi_IsConsoleValidString(const char16_t* str);

void TETRISAPI Efi_VPrint(const char16_t* fmt, va_list args);
void TETRISAPI Efi_Print(const char16_t* fmt, ...);
void TETRISAPI Efi_Putc(int ch);

// Get char from input without waiting
int TETRISAPI Efi_Getchar(void);
// Get char from input with waiting key pressing
int TETRISAPI Efi_Getc(void);

void TETRISAPI Efi_WaitKey(char16_t key);
void TETRISAPI Efi_WaitAnyKey(void);
void TETRISAPI Efi_ResetInputKeys(void);

#endif  // TETRIS_EFI_CONSOLE_H
