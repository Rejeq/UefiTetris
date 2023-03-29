#include "Renderer.h"

#include "Efi/Console.h"
#include "Efi/Memory.h"
#include "Log.h"

typedef struct Ren_ColorData {
  int fg;
  int bg;
} Ren_ColorData;

#define REN_COLOR_SIZE 256
struct Ren_Data {
  Efi_ConsoleMode snap;  // Saved init console state
  Ren_ColorData colors[REN_COLOR_SIZE];
};

#define REN_GET_COLOR(ren, key) (ren)->colors[(key) % REN_COLOR_SIZE]

Ren_Data* Ren_Create() {
  Ren_Data* data = (Ren_Data*)Efi_Calloc(1, sizeof(Ren_Data));
  if (data == NULL) {
    LOG_ERROR(L"Unable to create rener data: bad alloc");
    return NULL;
  }

  Ren_Save(data);
  Efi_SetCursor(0);
  return data;
}

void Ren_Destroy(Ren_Data* data) {
  if (data == NULL)
    return;

  Efi_Free(data);
}

void Ren_Save(Ren_Data* data) {
  if (data == NULL)
    return;

  Efi_Memcpy(&data->snap, Efi_GetConsoleMode(), sizeof(Efi_ConsoleMode));
}

void Ren_SetColor(Ren_Data* data, state_t state, int fg, int bg) {
  if (data == NULL)
    return;

  REN_GET_COLOR(data, state).fg = fg;
  REN_GET_COLOR(data, state).bg = bg;
}

void Ren_Restore(const Ren_Data* data) {
  if (data == NULL)
    return;

  Efi_SetConsoleAttribute(data->snap.Attribute);
  Efi_SetCursorPosition(data->snap.CursorRow, data->snap.CursorColumn);
  Efi_SetCursor(data->snap.CursorVisible);
}

void Ren_PutStr(int x, int y, int fg, int bg, const char16_t* fmt, ...) {
  va_list args;

  Efi_SetCursorPosition(x, y);
  Efi_SetConsoleAttribute(fg | bg);

  va_start(args, fmt);
  Efi_VPrint(fmt, args);
  va_end(args);

  Efi_SetConsoleAttribute(0);
}

int Ren_Clear(void) {
  Efi_SetConsoleAttribute(0);
  Efi_ClearScreen();
  return 0;
}

int Ren_Draw(const Ren_Data* ren, const Board* board, int posX, int posY,
             char activeChar, char emptyChar, int checkBackground) {
  if (board == NULL)
    return 1;
  if (!BOARD_VALID(board))
    return 1;

  for (int y = 0; y < board->height; ++y) {
    Efi_SetCursorPosition(posX, posY + y);

    for (int x = 0; x < board->width; ++x) {
      char ch = emptyChar;
      int color = EFI_WHITE | EFI_BACKGROUND_BLACK;

      if (BOARD_IS_ACTIVE(board, x, y)) {
        ch = (activeChar == '\0') ? BOARD_STATE(board, x, y) : activeChar;
        if (ren != NULL) {
          Ren_ColorData col = REN_GET_COLOR(ren, BOARD_STATE(board, x, y));
          int res = col.fg;
          if (checkBackground)
            res |= col.bg;

          if (res != 0)
            color = res;
        }
      }

      if (ch == '\0') {
        Efi_SetCursorPosition(posX + x + 1, posY + y);
        continue;
      }

      Efi_SetConsoleAttribute(color);
      Efi_Putc(ch);
    }
  }
  return 0;
}
