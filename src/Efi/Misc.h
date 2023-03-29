#ifndef TETRIS_EFI_MISC_H
#define TETRIS_EFI_MISC_H

#include "Stdint.h"

#if defined(TETRIS_EFI_PROVIDER_EDK)
#include <Protocol/LoadedImage.h>
#endif

typedef EFI_LOADED_IMAGE_PROTOCOL Efi_LoadedImageProtocol;

void TETRISAPI Efi_MicroSleep(size_t mc);
size_t TETRISAPI Efi_Strlen(const char16_t* str);

time_t TETRISAPI Efi_Time(time_t* arg);
void TETRISAPI Efi_Srand(unsigned seed);
size_t TETRISAPI Efi_Rand(void);

Efi_LoadedImageProtocol* TETRISAPI Efi_GetLoadedImage(EFI_HANDLE *handle);
void TETRISAPI Efi_SetWatchdogTimer(size_t seconds, uint64_t code, size_t size, char16_t* data);

#endif  // TETRIS_EFI_MISC_H
