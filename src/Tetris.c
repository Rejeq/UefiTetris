#include "Tetris.h"

#include "Efi/Console.h"
#include "Efi/Memory.h"
#include "Efi/Misc.h"

#include "Log.h"

static void Tetris_ChangeNextActive(Tetris_Data *game) {
  if (game == NULL)
    return;

  // int active = 0;
  int active = Efi_Rand() % TETROMINO_COUNT;
  game->nextActiveTetro = &game->tetros[active];
}

int Tetris_Init(Tetris_Data *game, int width, int height) {
  if (game == NULL)
    return 1;

  LOG_DEBUG(L"Initilazing game...");
  // Efi_Srand(123);
  Efi_Srand(Efi_Time(NULL));

  game->level = 1;
  game->score = 0;
  game->speed = TETRIS_INIT_SPEED;
  game->ended = 0;

  Efi_GetConsoleSize(&game->totalCols, &game->totalRows);
  LOG_DEBUG(L"Console size: (%d, %d)", game->totalCols, game->totalRows);

  game->board = Board_Create(width, height);
  if (!BOARD_VALID(&game->board)) {
    LOG_ERROR(L"Unable to create main board");
    return 1;
  }
  LOG_DEBUG(L"Board size: (%d, %d)", game->board.width, game->board.height);

  if (game->board.width > game->totalCols ||
      game->board.height > game->totalRows) {
    LOG_CRITICAL(L"Unable to draw board, console too small");
    return 1;
  }

  // Centering the board
  game->boardPosX = (game->totalCols / 2) - (game->board.width / 2);
  game->boardPosY = (game->totalRows / 2) - (game->board.height / 2);
  LOG_DEBUG(L"Board position: (%d, %d)", game->boardPosX, game->boardPosY);

  Tetris_CreateI(&game->tetros[TETROMINO_I], 'I');
  Tetris_CreateJ(&game->tetros[TETROMINO_J], 'J');
  Tetris_CreateL(&game->tetros[TETROMINO_L], 'L');
  Tetris_CreateO(&game->tetros[TETROMINO_O], 'O');
  Tetris_CreateT(&game->tetros[TETROMINO_T], 'T');
  Tetris_CreateS(&game->tetros[TETROMINO_S], 'S');
  Tetris_CreateZ(&game->tetros[TETROMINO_Z], 'Z');

  game->renData = Ren_Create();
  if (game->renData == NULL) {
    LOG_ERROR(L"Unable to create efi renderer");
    return 1;
  }

  Ren_SetColor(game->renData, 'I', EFI_BLUE, EFI_BACKGROUND_BLUE);
  Ren_SetColor(game->renData, 'J', EFI_BROWN, EFI_BACKGROUND_BROWN);
  Ren_SetColor(game->renData, 'L', EFI_CYAN, EFI_BACKGROUND_CYAN);
  Ren_SetColor(game->renData, 'O', EFI_LIGHTGRAY, EFI_BACKGROUND_LIGHTGRAY);
  Ren_SetColor(game->renData, 'S', EFI_GREEN, EFI_BACKGROUND_GREEN);
  Ren_SetColor(game->renData, 'Z', EFI_RED, EFI_BACKGROUND_RED);
  Ren_SetColor(game->renData, 'T', EFI_MAGENTA, EFI_BACKGROUND_MAGENTA);

  game->holdTetro = NULL;
  game->prevHoldTetro = NULL;
  game->holdUsed = 0;

  Tetris_ChangeNextActive(game);
  game->currentActiveTetro = NULL;
  game->activeTetro = Board_Create(0, 0);
  game->activeX = 0;
  game->activeX = 0;
  game->activeDropY = 0;

  game->clearRowsSize = game->board.height;
  game->clearRowsData = Efi_Calloc(game->clearRowsSize, sizeof(int));
  if (game->clearRowsData == NULL) {
    LOG_ERROR(L"Unable to create clear rows buffer: bad alloc");
    return 1;
  }
  game->clearRowsCount = 0;
  game->clearRowsTotal = 0;

  game->showHelp = 1;
  game->showStats = 1;
  return 0;
}

void Tetris_Shutdown(Tetris_Data *game) {
  if (game == NULL)
    return;

  Ren_Clear();
  Ren_Restore(game->renData);

  Efi_Free(game->clearRowsData);
  for (int i = 0; i < TETROMINO_COUNT; i++) {
    Board_Destroy(&game->tetros[i]);
  }

  Ren_Destroy(game->renData);
  Board_Destroy(&game->board);
  Board_Destroy(&game->activeTetro);
}

int Tetris_SwapHold(Tetris_Data *game) {
  if (game == NULL)
    return 1;

  if (game->holdUsed == 1)
    return 1;
  game->holdUsed = 1;

  game->prevHoldTetro = game->holdTetro;
  game->holdTetro = game->currentActiveTetro;

  Tetris_SetActive(game, game->prevHoldTetro);
  Tetris_ResetActivePosition(game);
  return 0;
}

void Tetris_SetActive(Tetris_Data *game, Board *newActive) {
  if (game == NULL)
    return;

  game->currentActiveTetro = newActive;
  if (newActive == NULL) {
    Board_Destroy(&game->activeTetro);
    return;
  }

  Board_CloneTo(&game->activeTetro, newActive);
}

Collision Tetris_MoveActive(Tetris_Data *game, int x, int y) {
  if (game == NULL)
    return BOARD_COLLISION_NONE;

  Collision collision = BOARD_COLLISION_NONE;
  int newX = (x >= 0) ? x : game->activeX;
  int newY = (y >= 0) ? y : game->activeY;

  collision = Board_CheckCollision(&game->board, &game->activeTetro, newX, newY);
  if (collision == BOARD_COLLISION_NONE) {
    game->activeX = newX;
    game->activeY = newY;

    int minY =
        Board_GetMinDownPosition(&game->board, &game->activeTetro, newX, newY);
    game->activeDropY = (minY > 0) ? minY : 0;
  }

  return collision;
}

Collision Tetris_MoveRelativeActive(Tetris_Data *game, int relX,
                                          int relY) {
  if (game == NULL)
    return BOARD_COLLISION_NONE;

  return Tetris_MoveActive(game, game->activeX + relX, game->activeY + relY);
}

void Tetris_ResetActivePosition(Tetris_Data *game) {
  if (game == NULL)
    return;

  Collision collide = Tetris_MoveActive(game, game->board.width / 2, 0);
  if (collide != BOARD_COLLISION_NONE) {
    game->ended = 1;
  }
}

int Tetris_RotateActive(Tetris_Data *game) {
  if (game == NULL)
    return 1;

  Board rotated = Board_Clone(&game->activeTetro);

  Board_CwRotate(&rotated);
  Collision res =
      Board_CheckCollision(&game->board, &rotated, game->activeX, game->activeY);
  if (res == BOARD_COLLISION_NONE) {
    Board_Destroy(&game->activeTetro);
    game->activeTetro = rotated;
  }

  return 0;
}

void Tetris_InsertActive(Tetris_Data *game) {
  if (game == NULL)
    return;

  Board_Insert(&game->board, &game->activeTetro, game->activeX, game->activeY);
  Tetris_SetActive(game, NULL);
  Tetris_MoveActive(game, 0, 0);

  // It must be in increasing order
  for (int row = 0; row < game->board.height; row++) {
    if (Board_CheckRow(&game->board, row)) {
      if (game->clearRowsCount >= game->clearRowsSize) {
        LOG_ERROR(L"Unable to clear row, too much rows in buffer already");
        continue;
      }
      game->clearRowsData[game->clearRowsCount] = row;
      game->clearRowsCount += 1;
    }
  }

  LOG_DEBUG(L"ClearRowsCount: %d", game->clearRowsCount);
}

void Tetris_ClearRows(Tetris_Data *game) {
  if (game == NULL)
    return;

  for (int i = 0; i < game->clearRowsCount; i++) {
    for (int row = game->clearRowsData[i]; row > 0; row--) {
      Board_SetRowFrom(&game->board, row - 1, row);
    }
    game->clearRowsData[i] = 0;
  }

  int points = 0;
  switch (game->clearRowsCount) {
    case 1: points = TETRIS_SCORE_SINGLE; break;
    case 2: points = TETRIS_SCORE_DOUBLE; break;
    case 3: points = TETRIS_SCORE_TRIPLE; break;
    case 4: points = TETRIS_SCORE_TETRIS; break;
    default: break;
  }
  game->score += points * game->level;

  game->clearRowsTotal += game->clearRowsCount;
  while (game->clearRowsTotal >= game->level * TETRIS_ROWS_PER_LEVEL) {
    game->speed /= 800 - (game->level * 7);
    game->level += 1;
  }

  game->clearRowsCount = 0;
}

void Tetris_DropActive(Tetris_Data *game) {
  if (game == NULL)
    return;

  if (game->activeDropY <= 0) {
    LOG_WARNING(L"Unable to drop active: activeDropY is less than zero");
    return;
  }

  game->score += (game->activeDropY - game->activeY) * game->level;
  game->activeY = game->activeDropY;
}

void Tetris_Update(Tetris_Data *game) {
  if (game == NULL)
    return;

  if (BOARD_VALID(&game->activeTetro)) {
    Collision collide = Tetris_MoveRelativeActive(game, 0, 1);
    if (collide != BOARD_COLLISION_NONE) {
      Tetris_InsertActive(game);
    }
  } else {
    game->holdUsed = 0;
    Tetris_SetActive(game, game->nextActiveTetro);
    Tetris_ChangeNextActive(game);
    Tetris_ResetActivePosition(game);
  }

  if (game->clearRowsCount != 0)
    Tetris_ClearRows(game);
}
