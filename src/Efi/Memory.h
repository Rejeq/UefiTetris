#ifndef TETRIS_EFI_MEMORY_H
#define TETRIS_EFI_MEMORY_H

#include "Stdint.h"

void* TETRISAPI Efi_Malloc(size_t size);
void* TETRISAPI Efi_Calloc(size_t num, size_t size);
// void* TETRISAPI Efi_Realloc(void* ptr, size_t new_size);
void TETRISAPI Efi_Free(void* ptr);

#ifdef TETRIS_EFI_DEBUG_MEMORY

// return the total number of alloc calls
int TETRISAPI Efi_DbgGetAllocateCount();

// return the total number of free calls
int TETRISAPI Efi_DbgGetFreeCount();

#endif  // TETRIS_EFI_DEBUG_MEMORY

void TETRISAPI Efi_Memset(void* dst, int ch, size_t count);
void TETRISAPI Efi_Memcpy(void* dst, const void* src, size_t count);

#endif  // TETRIS_EFI_MEMORY_H
