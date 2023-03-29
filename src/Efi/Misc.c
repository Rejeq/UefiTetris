#include "Misc.h"

#if defined(TETRIS_EFI_PROVIDER_GNU_EFI)

#include <efilib.h>

#elif defined(TETRIS_EFI_PROVIDER_EDK)

#define uefi_call_wrapper(func, va_num, ...) func(__VA_ARGS__)

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/LoadedImage.h>

#else
#error "Efi provider not provided"
#endif

// Time stuff - stolen from posix-uefi library
static uint64_t YerasToSec(uint64_t year, int* is_leap) {
  int y;
  int cycles;
  int centuries;
  int leaps;
  int rem;

  if (year - 2ULL <= 136) {
    y = (int)year;
    leaps = (y - 68) >> 2;
    if (!((y - 68) & 3)) {
      leaps--;
      if (is_leap)
        *is_leap = 1;
    } else if (is_leap)
      *is_leap = 0;
    return 31536000ULL * (uint64_t)(y - 70) + 86400ULL * (uint64_t)leaps;
  }

  if (!is_leap)
    is_leap = &(int){0};
  cycles = (int)((year - 100) / 400);
  rem = (year - 100) % 400;
  if (rem < 0) {
    cycles--;
    rem += 400;
  }
  if (!rem) {
    *is_leap = 1;
    centuries = 0;
    leaps = 0;
  } else {
    if (rem >= 200) {
      if (rem >= 300) {
        centuries = 3;
        rem -= 300;
      } else {
        centuries = 2;
        rem -= 200;
      }
    } else {
      if (rem >= 100) {
        centuries = 1;
        rem -= 100;
      } else
        centuries = 0;
    }
    if (!rem) {
      *is_leap = 0;
      leaps = 0;
    } else {
      leaps = rem / 4;
      rem %= 4;
      *is_leap = !rem;
    }
  }

  leaps += 97 * cycles + 24 * centuries - *is_leap;

  return (uint64_t)(year - 100) * 31536000ULL + (uint64_t)leaps * 86400ULL +
         946684800ULL + 86400ULL;
}

time_t PackTime(const EFI_TIME* time) {
  static const uint64_t secs_through_month[] = {
      0,           31 * 86400,  59 * 86400,  90 * 86400,
      120 * 86400, 151 * 86400, 181 * 86400, 212 * 86400,
      243 * 86400, 273 * 86400, 304 * 86400, 334 * 86400};

  uint64_t out = 0;
  int is_leap = 0;
  int month = time->Month - 1;
  // + - workaround some buggy firmware
  uint64_t year = (uint64_t)time->Year + (time->Year > 2000 ? -1900 : 98);

  if (month >= 12 || month < 0) {
    uint64_t adj;
    adj = (uint64_t)month / 12;
    month %= 12;
    if (month < 0) {
      adj--;
      month += 12;
    }
    year += adj;
  }

  out = YerasToSec(year, &is_leap);
  out += secs_through_month[month];
  if (is_leap && month >= 2)
    out += 86400;
  out += 86400ULL * (uint64_t)(time->Day - 1);
  out += 3600ULL * (uint64_t)time->Hour;
  out += 60ULL * (uint64_t)time->Minute;
  out += (uint64_t)time->Second;
  return (time_t)out;
}

void Efi_MicroSleep(size_t mc) {
  uefi_call_wrapper(gBS->Stall, 1, mc);
}

size_t Efi_Strlen(const char16_t* str) {
  char16_t const* start = str;
  while (*str != L'\0')
    str++;
  return str - start;
}

time_t Efi_Time(time_t* arg) {
  EFI_STATUS status = EFI_SUCCESS;
  EFI_TIME time;

  status = gRT->GetTime(&time, NULL);
  if (EFI_ERROR(status))
    return 0;
  return PackTime(&time);
}

static int g_randNext = 0;

void Efi_Srand(unsigned seed) {
  g_randNext = seed;
}

size_t Efi_Rand(void) {
  g_randNext = g_randNext * 1103515245 + 12345;
  return (unsigned int)(g_randNext / 65536) % 32768;
}

Efi_LoadedImageProtocol* Efi_GetLoadedImage(EFI_HANDLE* handle) {
  Efi_LoadedImageProtocol* image = NULL;

  uefi_call_wrapper(gST->BootServices->HandleProtocol, 3, *handle,
                    &gEfiLoadedImageProtocolGuid, (void**)&image);
  return image;
}

void Efi_SetWatchdogTimer(size_t seconds, uint64_t code, size_t size, char16_t* data)
{
	uefi_call_wrapper(gBS->SetWatchdogTimer, 4, seconds, code, size, data);
}
