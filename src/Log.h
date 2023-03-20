#ifndef TETRIS_LOG_H
#define TETRIS_LOG_H

#include "Efi/Console.h"

typedef enum {
  LOG_LEVEL_CRITICAL,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
} Log_Level;

// Returns 1 if error happened
// Otherwise - 0
int TETRISAPI Log_Init(int startCol, int startRow, int endRow);
void TETRISAPI Log_Destroy();

// Do not call it directly, use the LOG_* macro instead
void TETRISAPI Log_InternalPrint(Log_Level level, const char16_t* fmt, ...);

#ifdef TETRIS_LOG_ENABLED

#define LOG_CRITICAL(fmt, ...) \
  Log_InternalPrint(LOG_LEVEL_CRITICAL, fmt L"\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) \
  Log_InternalPrint(LOG_LEVEL_ERROR, fmt L"\n", ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) \
  Log_InternalPrint(LOG_LEVEL_WARNING, fmt L"\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) \
  Log_InternalPrint(LOG_LEVEL_INFO, fmt L"\n", ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) \
  Log_InternalPrint(LOG_LEVEL_DEBUG, fmt L"\n", ##__VA_ARGS__)

#else  // TETRIS_LOG_ENABLED

// clang-format off
#define LOG_CRITICAL(fmt, ...) do {} while(0)
#define LOG_ERROR(fmt, ...) do {} while(0)
#define LOG_WARNING(fmt, ...) do {} while(0)
#define LOG_INFO(fmt, ...) do {} while(0)
#define LOG_DEBUG(fmt, ...) do {} while(0)
// clang-format on

#endif  // TETRIS_LOG_ENABLED

#endif  // TETRIS_LOG_H
