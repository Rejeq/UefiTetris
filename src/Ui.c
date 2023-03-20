#include "Ui.h"

#include "Efi/Console.h"
#include "Efi/Misc.h"

#include "Log.h"
#include "Renderer.h"

static void LogoGlyphPrinter(int x, int y, int fg, int bg,
                             const char16_t *glyph) {
  // if detected twice, then break the loop
  while (*glyph != L'\0') {
    Ren_PutStr(x, y, fg, bg, glyph);
    y++;

    while (*glyph != L'\0')
      glyph++;
    glyph++;  // Skip '\0' char
  }
}

static void ReplaceLogoGlyph(char16_t *glyph, char16_t rep, char16_t with) {
  if (glyph == NULL)
    return;

  int isEOL = 0;  // End Of Line
  while (!isEOL || *glyph != L'\0') {
    if (*glyph == rep)
      *glyph = with;

    isEOL = (*glyph == L'\0');
    glyph++;
  }
}

static void CreateLogoGlyph(char16_t ch, char16_t **glyph, int *width) {
  static int hasSquare = 2;  // 2 - is undefined;
  if (hasSquare == 2) {
    hasSquare = Efi_IsConsoleValidString(L"█");

    if (hasSquare == 0) {
      LOG_WARNING(L"Unable to draw unicode logo, switching to ascii");
    }
  }
  switch (ch) {
    case L'T':
      *width = 8;
      *glyph =
          L"████████ \0"
          L"   ██    \0"
          L"   ██    \0"
          L"   ██    \0"
          L"   ██    \0";
      break;
    case L'E':
      *width = 7;
      *glyph =
          L"███████ \0"
          L"██      \0"
          L"█████   \0"
          L"██      \0"
          L"███████ \0";
      break;
    case L'R':
      *width = 7;
      *glyph =
          L"██████  \0"
          L"██   ██ \0"
          L"██████  \0"
          L"██   ██ \0"
          L"██   ██ \0";
      break;
    case L'I':
      *width = 3;
      *glyph =
          L"██ \0"
          L"██ \0"
          L"██ \0"
          L"██ \0"
          L"██ \0";
      break;
    case L'S':
      *width = 7;
      *glyph =
          L"███████ \0"
          L"██      \0"
          L"███████ \0"
          L"     ██ \0"
          L"███████ \0";
      break;
    default: LOG_WARNING(L"Trying to create undefined glyph: '%c'", ch);
  }

  if (hasSquare == 0)
    ReplaceLogoGlyph(*glyph, L'█', ch);
}

void Ui_PrintLogo(int maxX, int maxY) {
  const int glyphsCount = 6;

  int widthT = 0;
  char16_t *glyphT = NULL;
  CreateLogoGlyph('T', &glyphT, &widthT);

  int widthE = 0;
  char16_t *glyphE = NULL;
  CreateLogoGlyph('E', &glyphE, &widthE);

  int widthR = 0;
  char16_t *glyphR = NULL;
  CreateLogoGlyph('R', &glyphR, &widthR);

  int widthI = 0;
  char16_t *glyphI = NULL;
  CreateLogoGlyph('I', &glyphI, &widthI);

  int widthS = 0;
  char16_t *glyphS = NULL;
  CreateLogoGlyph('S', &glyphS, &widthS);

  int widthSpace = 3;

  int widthTotal = (widthT * 2) + widthE + widthR + widthI + widthS +
                   (glyphsCount * widthSpace);
  int heightTotal = 5;

  if (widthTotal > maxX || heightTotal > maxY) {
    LOG_DEBUG(
        "Unable to print logo: too small screen, switching to the simple "
        "chars. Size of the logo: (%d, %d)",
        widthTotal, heightTotal);

    widthT = 1;
    glyphT = L"T\0";

    widthE = 1;
    glyphE = L"E\0";

    widthR = 1;
    glyphR = L"R\0";

    widthI = 1;
    glyphI = L"I\0";

    widthS = 1;
    glyphS = L"S\0";

    widthSpace = 1;
    widthTotal = glyphsCount;
    heightTotal = 2;  // The correct value is 1, but it will be divided by 2 in
                      // future, which gives 1 / 2 = 0, and we need to get = 1
  }

  int centerX = maxX / 2;
  int centerY = maxY / 2;

  int currX = centerX - (widthTotal / 2);
  int currY = centerY - (heightTotal / 2);

  const int bgColor = EFI_BACKGROUND_BLACK;
  LogoGlyphPrinter(currX, currY, EFI_RED, bgColor, glyphT);
  currX += widthSpace + widthT;
  LogoGlyphPrinter(currX, currY, EFI_BLUE, bgColor, glyphE);
  currX += widthSpace + widthE;
  LogoGlyphPrinter(currX, currY, EFI_BROWN, bgColor, glyphT);
  currX += widthSpace + widthT;
  LogoGlyphPrinter(currX, currY, EFI_GREEN, bgColor, glyphR);
  currX += widthSpace + widthR;
  LogoGlyphPrinter(currX, currY, EFI_CYAN, bgColor, glyphI);
  currX += widthSpace + widthI;
  LogoGlyphPrinter(currX, currY, EFI_MAGENTA, bgColor, glyphS);

  const char16_t *pressKeyStr = L"Press any key to continue...";
  currX = centerX - (Efi_Strlen(pressKeyStr) / 2);
  currY += heightTotal + 1;
  Ren_PutStr(currX, currY, EFI_LIGHTGRAY, bgColor, pressKeyStr);
}

void Ui_DrawAll(const Tetris_Data *game) {
  if (game == NULL)
    return;

  Ui_DrawHelp(game);
  Ui_DrawStats(game);
  Ui_DrawBoardBorders(game);
  Ui_DrawBoard(game);
}

void Ui_DrawHelp(const Tetris_Data *game) {
  if (game == NULL)
    return;
  if (game->showHelp == 0)
    return;

  const int totalRows = 10;

  int headerX = game->boardPosX - (game->board.width * 2);
  int currY = game->boardPosY + (game->board.height - totalRows) / 2;

  int leftX = headerX - 5;
  int keyLeftX = leftX + 12;

  // regular text
  int regFg = EFI_LIGHTGRAY;
  int regBg = EFI_BACKGROUND_BLACK;

  int keyFg = EFI_MAGENTA;
  int keyBg = EFI_BACKGROUND_BLACK;

  Ren_PutStr(headerX, currY, EFI_WHITE, regBg, L"Help");
  currY += 2;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Move Left:");
  Ren_PutStr(keyLeftX, currY, keyFg, keyBg, L"A");
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Move Right:");
  Ren_PutStr(keyLeftX, currY, keyFg, keyBg, L"D");
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Rotate:");
  Ren_PutStr(keyLeftX, currY, keyFg, keyBg, L"W");
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Drop:");
  Ren_PutStr(keyLeftX, currY, keyFg, keyBg, L"S");
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Hold:");
  Ren_PutStr(keyLeftX, currY, keyFg, keyBg, L"H");
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Pause:");
  Ren_PutStr(keyLeftX, currY, keyFg, keyBg, L"P");
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Redraw:");
  Ren_PutStr(keyLeftX, currY, keyFg, keyBg, L"U");
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Exit:");
  Ren_PutStr(keyLeftX, currY, keyFg, keyBg, L"Q");
  currY += 1;
}

void Ui_DrawStats(const Tetris_Data *game) {
  if (game == NULL)
    return;
  if (game->showStats == 0)
    return;

  const int totalRows = 5;

  int headerX = game->boardPosX + (game->board.width * 2);
  int currY = game->boardPosY + (game->board.height - totalRows) / 2;

  int leftX = headerX - 5;
  int dataLeftX = leftX + 12;

  // regular text
  int regFg = EFI_LIGHTGRAY;
  int regBg = EFI_BACKGROUND_BLACK;

  int dataFg = EFI_MAGENTA;
  int dataBg = EFI_BACKGROUND_BLACK;

  Ren_PutStr(headerX, currY, EFI_WHITE, regBg, L"Stats");
  currY += 2;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Level:");
  Ren_PutStr(dataLeftX, currY, dataFg, dataBg, L"%d", game->level);
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Score:");
  Ren_PutStr(dataLeftX, currY, dataFg, dataBg, L"%d", game->score);
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Cleared:");
  Ren_PutStr(dataLeftX, currY, dataFg, dataBg, L"%d", game->clearRowsTotal);
}

void Ui_DrawBoardBorders(const Tetris_Data *game) {
  if (game == NULL)
    return;

  int fg = EFI_LIGHTGRAY;
  int bg = EFI_BACKGROUND_BLACK;

  char16_t *chVert = L"│";
  char16_t *chHori = L"─";
  char16_t *chLeftBottom = L"└";
  char16_t *chRightBottom = L"┘";

  int hasUni = Efi_IsConsoleValidString(L"│─└┘");
  if (hasUni == 0) {
    void Ui_RefreshBoardBorders(const Tetris_Data *game);
    LOG_WARNING(L"Unable to print unicode border, using spaces");

    fg = EFI_LIGHTGRAY;
    bg = EFI_BACKGROUND_LIGHTGRAY;

    chVert = L" ";
    chHori = L" ";
    chLeftBottom = L" ";
    chRightBottom = L" ";
  }

  int leftCorner = game->boardPosX - 1;
  int rightCorner = game->boardPosX + game->board.width;
  int topCorner = game->boardPosY - 1;
  int bottomCorner = game->boardPosY + game->board.height;

  for (int x = leftCorner; x < rightCorner; x++) {
    // Ren_PutStr(x, topCorner, fg, bg, chHori);
    Ren_PutStr(x, bottomCorner, fg, bg, chHori);
  }

  for (int y = topCorner; y < bottomCorner; y++) {
    Ren_PutStr(leftCorner, y, fg, bg, chVert);
    Ren_PutStr(rightCorner, y, fg, bg, chVert);
  }

  Ren_PutStr(leftCorner, bottomCorner, fg, bg, chLeftBottom);
  Ren_PutStr(rightCorner, bottomCorner, fg, bg, chRightBottom);
}

void Ui_DrawBoard(const Tetris_Data *game) {
  if (game == NULL)
    return;

  int offsetX = game->boardPosX;
  int offsetY = game->boardPosY;
  Ren_Draw(game->renData, &game->board, offsetX, offsetY, '\0', ' ', 1);
  if (BOARD_VALID(&game->activeTetro)) {
    // Draw ghost tetromino
    if (game->activeDropY > 0) {
      Ren_Draw(game->renData, &game->activeTetro, offsetX + game->activeX,
               offsetY + game->activeDropY, '.', '\0', 0);
    }

    Ren_Draw(game->renData, &game->activeTetro, offsetX + game->activeX,
             offsetY + game->activeY, '\0', '\0', 1);
  }

  Ui_DrawNextTetromino(game);
  Ui_DrawHoldTetromino(game);
}

void Ui_DrawGameOver(const Tetris_Data *game) {
  if (game == NULL)
    return;

  const int totalRows = 6;

  int headerX = game->totalCols / 2 - 10;
  int currY = (game->totalRows / 2) - (totalRows / 2);

  int leftX = headerX - 5;
  int dataLeftX = leftX + 12;

  // regular text
  int regFg = EFI_LIGHTGRAY;
  int regBg = EFI_BACKGROUND_BLACK;

  int dataFg = EFI_MAGENTA;
  int dataBg = EFI_BACKGROUND_BLACK;

  Ren_PutStr(headerX, currY, EFI_WHITE, regBg, L"Game Over");
  currY += 2;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Level:");
  Ren_PutStr(dataLeftX, currY, dataFg, dataBg, L"%d", game->level);
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Score:");
  Ren_PutStr(dataLeftX, currY, dataFg, dataBg, L"%d", game->score);
  currY += 1;

  Ren_PutStr(leftX, currY, regFg, regBg, L"Cleared:");
  Ren_PutStr(dataLeftX, currY, dataFg, dataBg, L"%d", game->clearRowsTotal);
  currY += 1;

  leftX -= 13;
  Ren_PutStr(leftX, currY, regFg, regBg,
             L"Press any key to continue or q to quit...");
}

void Ui_DrawNextTetromino(const Tetris_Data *game) {
  if (game == NULL)
    return;
  if (game->nextActiveTetro == NULL)
    return;

  const Board *prev = game->currentActiveTetro;
  const Board *curr = game->nextActiveTetro;
  int currX = game->boardPosX + (game->board.width * 2);
  int currY = game->boardPosY + 2;

  // Clear previous next tetromino and draw current next tetromino
  Ren_Draw(game->renData, prev, currX, currY, ' ', '\0', 0);
  Ren_Draw(game->renData, curr, currX, currY, '\0', '\0', 1);
}

void Ui_DrawHoldTetromino(const Tetris_Data *game) {
  if (game == NULL)
    return;
  if (game->holdTetro == NULL)
    return;

  const Board *prev = game->prevHoldTetro;
  if (prev != NULL) {
    int prevX = game->boardPosX + 1;
    int prevY = game->boardPosY - prev->height - 2;
    Ren_Draw(game->renData, prev, prevX, prevY, ' ', '\0', 0);
  }

  const Board *curr = game->holdTetro;
  int currX = game->boardPosX + 1;
  int currY = game->boardPosY - curr->height - 2;

  Ren_Draw(game->renData, curr, currX, currY, '\0', '\0', 1);
}

int Ui_HandleGameOverInput(Tetris_Data *game) {
  if (game == NULL)
    return 0;

  int key = Efi_Getc();
  if (key == 0)
    return 1;

  if (key == TETRIS_KEY_EXIT)
    return 1;
  return 0;
}

int Ui_HandleInput(Tetris_Data *game) {
  if (game == NULL)
    return 0;

  int key = Efi_Getchar();
  Efi_ResetInputKeys();

  if (key == 0)
    return 0;
  LOG_INFO(L"Input key: %c (%d)", key, key);

  switch (key) {
    case TETRIS_KEY_REFRESH:
      Ui_DrawAll(game);
      break;
      break;
    case TETRIS_KEY_EXIT: game->ended = 1; break;
  }

  if (!BOARD_VALID(&game->activeTetro))
    return 0;

  switch (key) {
    case TETRIS_KEY_LEFT: Tetris_MoveRelativeActive(game, -1, 0); break;
    case TETRIS_KEY_RIGHT: Tetris_MoveRelativeActive(game, 1, 0); break;
    case TETRIS_KEY_DOWN: Tetris_DropActive(game); break;
    case TETRIS_KEY_ROTATE: Tetris_RotateActive(game); break;
    case TETRIS_KEY_HOLD: Tetris_SwapHold(game); break;
  }

  return 0;
}
