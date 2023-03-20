#ifndef TETRIS_EFI_STDINT_H
#define TETRIS_EFI_STDINT_H

#if defined(TETRIS_EFI_PROVIDER_GNU_EFI)

#include <efi.h>
#include <efistdarg.h>

#elif defined(TETRIS_EFI_PROVIDER_EDK)

#include <Uefi.h>
#define uefi_call_wrapper(func, va_num, ...) func(__VA_ARGS__)


#define va_list VA_LIST
# define va_start(v,l)	VA_START(v,l)
# define va_end(v)	VA_END(v)
# define va_arg(v,l)	VA_ARG(v,l)
# define va_copy(d,s)	VA_COPY(d,s)
#else
#error "Efi provider not provided"
#endif


#define EFI_CHAR_BIT 8

typedef UINT8 uint8_t;
typedef UINT16 uint16_t;
typedef UINT32 uint32_t;
typedef UINT64 uint64_t;
typedef INT8 int8_t;
typedef INT16 int16_t;
typedef INT32 int32_t;
typedef INT64 int64_t;
typedef CHAR16 char16_t;
typedef UINTN size_t;

typedef size_t time_t;

#endif  // TETRIS_EFI_STDINT_H
