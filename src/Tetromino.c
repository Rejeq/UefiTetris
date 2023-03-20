#include "Tetromino.h"

#include "Log.h"

void Tetris_CreateI(Board *tetroI, state_t state) {
  if (tetroI == NULL)
    return;

  Board* tetro = tetroI;
  *tetro = Board_Create(1, 4);
  if (!BOARD_VALID(tetro)) {
    LOG_ERROR(L"Unable to create I tetromino");
    return;
  }

  BOARD_STATE(tetro, 0, 0) = state;
  BOARD_STATE(tetro, 0, 1) = state;
  BOARD_STATE(tetro, 0, 2) = state;
  BOARD_STATE(tetro, 0, 3) = state;
}

void Tetris_CreateJ(Board *tetroJ, state_t state) {
  if (tetroJ == NULL)
    return;

  Board *tetro = tetroJ;
  *tetro = Board_Create(2, 4);
  if (!BOARD_VALID(tetro)) {
    LOG_ERROR(L"Unable to create J tetromino");
    return;
  }

  BOARD_STATE(tetro, 1, 0) = state;
  BOARD_STATE(tetro, 1, 1) = state;
  BOARD_STATE(tetro, 1, 2) = state;
  BOARD_STATE(tetro, 1, 3) = state;
  BOARD_STATE(tetro, 0, 3) = state;
}

void Tetris_CreateL(Board *tetroL, state_t state) {
  if (tetroL == NULL)
    return;

  Board *tetro = tetroL;
  *tetro = Board_Create(2, 4);
  if (!BOARD_VALID(tetro)) {
    LOG_ERROR(L"Unable to create L tetromino");
    return;
  }

  BOARD_STATE(tetro, 0, 0) = state;
  BOARD_STATE(tetro, 0, 1) = state;
  BOARD_STATE(tetro, 0, 2) = state;
  BOARD_STATE(tetro, 0, 3) = state;
  BOARD_STATE(tetro, 1, 3) = state;
}

void Tetris_CreateO(Board *tetroO, state_t state) {
  if (tetroO == NULL)
    return;

  Board *tetro = tetroO;
  *tetro = Board_Create(2, 2);
  if (!BOARD_VALID(tetro)) {
    LOG_ERROR(L"Unable to create O tetromino");
    return;
  }

  BOARD_STATE(tetro, 0, 0) = state;
  BOARD_STATE(tetro, 0, 1) = state;
  BOARD_STATE(tetro, 1, 0) = state;
  BOARD_STATE(tetro, 1, 1) = state;
}

void Tetris_CreateT(Board *tetroT, state_t state) {
  if (tetroT == NULL)
    return;

  Board *tetro = tetroT;
  *tetro = Board_Create(3, 2);
  if (!BOARD_VALID(tetro)) {
    LOG_ERROR(L"Unable to create T tetromino");
    return;
  }

  BOARD_STATE(tetro, 0, 0) = state;
  BOARD_STATE(tetro, 1, 0) = state;
  BOARD_STATE(tetro, 2, 0) = state;
  BOARD_STATE(tetro, 1, 1) = state;
}

void Tetris_CreateS(Board *tetroS, state_t state) {
  if (tetroS == NULL)
    return;

  Board *tetro = tetroS;
  *tetro = Board_Create(3, 2);
  if (!BOARD_VALID(tetro)) {
    LOG_ERROR(L"Unable to create S tetromino");
    return;
  }

  BOARD_STATE(tetro, 2, 0) = state;
  BOARD_STATE(tetro, 1, 0) = state;
  BOARD_STATE(tetro, 1, 1) = state;
  BOARD_STATE(tetro, 0, 1) = state;
}

void Tetris_CreateZ(Board *tetroZ, state_t state) {
  if (tetroZ == NULL)
    return;

  Board *tetro = tetroZ;
  *tetro = Board_Create(3, 2);
  if (!BOARD_VALID(tetro)) {
    LOG_ERROR(L"Unable to create Z tetromino");
    return;
  }

  BOARD_STATE(tetro, 0, 0) = state;
  BOARD_STATE(tetro, 1, 0) = state;
  BOARD_STATE(tetro, 1, 1) = state;
  BOARD_STATE(tetro, 2, 1) = state;
}

