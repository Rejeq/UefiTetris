#include "Log.h"

#include "Efi/Console.h"
#include "Efi/Memory.h"
#include "Efi/Misc.h"

typedef struct Log_Data {
  int startCol;
  int startRow;
  int endRow;
  int currRow;
} Log_Data;

static Log_Data* gs_logData = NULL;

int Log_Init(int startCol, int startRow, int endRow) {
  if (gs_logData != NULL) {
    LOG_ERROR(L"Trying to initiate logging when its already been initiated");
    return 1;
  }

  gs_logData = Efi_Malloc(sizeof(Log_Data));
  if (gs_logData == NULL)
    return 1;

  gs_logData->startCol = startCol;
  gs_logData->startRow = startRow;
  gs_logData->endRow = endRow;
  gs_logData->currRow = startRow;

  return 0;
}

void Log_Destroy() {
  if (gs_logData == NULL)
    return;

  Efi_Free(gs_logData);
}

void Log_InternalPrint(Log_Level level, const char16_t* fmt, ...) {
  int startRow = 0;
  int endRow = 0;
  int startCol = 0;
  int currRow = 0;

  if (gs_logData != NULL) {
    startRow = gs_logData->startRow;
    endRow = gs_logData->endRow;
    startCol = gs_logData->startCol;
    currRow = gs_logData->currRow;
  }

  // Trying to prevent an infinite number of recursive calls.
  // It can happen by calling this funtion in Efi_Print and similiar, that
  // used within.
  static volatile int alreadyInUse = 0;

  if (alreadyInUse > 2)
    return;

  alreadyInUse += 1;

  int attrib = 0;
  const char16_t* lvlStr = NULL;

  switch (level) {
    case LOG_LEVEL_CRITICAL:
      attrib = EFI_LIGHTRED;
      lvlStr = L"Critical:";
      break;
    case LOG_LEVEL_ERROR:
      attrib = EFI_RED;
      lvlStr = L"Error:";
      break;
    case LOG_LEVEL_WARNING:
      attrib = EFI_MAGENTA;
      lvlStr = L"Warning:";
      break;
    case LOG_LEVEL_INFO:
      attrib = EFI_LIGHTGRAY;
      lvlStr = L"Info:";
      break;
    case LOG_LEVEL_DEBUG:
      attrib = EFI_BLUE;
      lvlStr = L"Debug:";
      break;
  };

  int currCol = startCol;
  Efi_SetCursorPosition(currCol, currRow);
  Efi_SetConsoleAttribute(attrib);
  Efi_Print(lvlStr);

  currCol += Efi_Strlen(lvlStr) + 1;
  Efi_SetCursorPosition(currCol, currRow);
  Efi_SetConsoleAttribute(EFI_LIGHTGRAY);
  va_list args;
  va_start(args, fmt);
  Efi_VPrint(fmt, args);
  va_end(args);

  currRow += 1;

  Efi_SetCursorPosition(startCol, currRow);
  Efi_SetConsoleAttribute(EFI_BACKGROUND_LIGHTGRAY);
  Efi_Print(L" ");

  Efi_SetConsoleAttribute(0);

  if (currRow >= endRow) {
    currRow = startRow;
  }

  if (gs_logData != NULL) {
    gs_logData->currRow = currRow;
  }

  alreadyInUse -= 1;
}
