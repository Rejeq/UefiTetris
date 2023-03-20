#include "Board.h"

#include "Efi/Memory.h"
#include "Log.h"

Board Board_Create(int width, int height) {
  Board board;

  if (width == 0 || height == 0) {
    board.data = NULL;
  } else {
    board.data = (state_t*)Efi_Calloc(width * height, sizeof(state_t));
    if (board.data == NULL) {
      LOG_WARNING(L"Unable to create data for board: bad meomry allocation");
      return board;
    }
  }

  board.width = width;
  board.height = height;

  return board;
}

void Board_Destroy(Board* board) {
  if (board == NULL)
    return;

  board->width = 0;
  board->height = 0;
  Efi_Free(board->data);
  board->data = NULL;
}

int Board_CloneTo(Board* dst, const Board* src) {
  if (dst == NULL || src == NULL)
    return 1;

  int srcSize = Board_GetSize(src);
  state_t* tmpData = Efi_Malloc(srcSize * sizeof(state_t));
  if (tmpData == NULL) {
    LOG_WARNING(L"Unable to clone board: bad alloc");
    return 1;
  }

  Efi_Free(dst->data);
  dst->data = tmpData;

  Efi_Memcpy(dst->data, src->data, srcSize * sizeof(state_t));
  dst->width = src->width;
  dst->height = src->height;

  return 0;
}

Board Board_Clone(const Board* src) {
  Board tmp = Board_Create(0, 0);
  Board_CloneTo(&tmp, src);
  return tmp;
}

Collision Board_CheckCollision(const Board* main, const Board* sub, int x,
                               int y) {
  if (main == NULL || sub == NULL)
    return BOARD_COLLISION_NONE;

  for (int subY = 0; subY < sub->height; subY++) {
    for (int subX = 0; subX < sub->width; subX++) {
      if (!BOARD_IS_ACTIVE(sub, subX, subY))
        continue;

      int newX = subX + x;
      if (newX < 0 || newX >= main->width)
        return BOARD_COLLISION_BOUNDS;

      int newY = subY + y;
      if (newY < 0 || newY >= main->height)
        return BOARD_COLLISION_BOUNDS;

      if (BOARD_IS_ACTIVE(main, newX, newY))
        return BOARD_COLLISION_BOARD;
    }
  }
  return BOARD_COLLISION_NONE;
}

int Board_Insert(Board* main, const Board* sub, int x, int y) {
  if (main == NULL || sub == NULL)
    return 0;

  for (int subY = 0; subY < sub->height; subY++) {
    for (int subX = 0; subX < sub->width; subX++) {
      if (!BOARD_IS_ACTIVE(sub, subX, subY))
        continue;
      int newX = subX + x;
      int newY = subY + y;
      BOARD_STATE(main, newX, newY) = BOARD_STATE(sub, subX, subY);
    }
  }
  return 0;
}

int Board_GetMinDownPosition(const Board* main, const Board* sub, int x,
                             int y) {
  if (main == NULL || sub == NULL)
    return -1;

  for (; y < main->height; y++) {
    Collision res = Board_CheckCollision(main, sub, x, y);
    if (res != BOARD_COLLISION_NONE)
      return y - 1;
  }
  return main->height - 1;
}

int Board_CheckRow(const Board* board, int y) {
  if (board == NULL)
    return 0;

  if (y >= board->height)
    return 0;

  for (int x = 0; x < board->width; x++) {
    if (!BOARD_IS_ACTIVE(board, x, y))
      return 0;
  }
  return 1;
}

int Board_SetRowFrom(Board* board, int from, int to) {
  if (board == NULL)
    return 1;
  if (from < 0 || from >= board->height || to < 0 || to >= board->height)
    return 1;
  if (from == to)
    return 0;

  Efi_Memcpy(board->data + (to * board->width),
             board->data + (from * board->width),
             board->width * sizeof(state_t));
  return 0;
}

int Board_SetRow(Board* board, int y, state_t state) {
  if (board == NULL)
    return 1;
  if (y < 0 || y >= board->height)
    return 1;

  Efi_Memset(board->data + (y * board->width), state,
             board->width * sizeof(state_t));
  return 0;
}

void Board_CwRotate(Board* board) {
  if (board == NULL)
    return;

  int width = board->width;
  int height = board->height;

  state_t* data = board->data;
  state_t* rotated = Efi_Malloc(width * height * sizeof(state_t));
  if (rotated == NULL) {
    LOG_ERROR(L"Unable to roatate board: bad alloc");
    return;
  }

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      BOARD_DATA_STATE(rotated, y, x, height) =
          BOARD_DATA_STATE(data, x, height - y - 1, width);

      // Here is rotate in Ccw direction
      // rotated[x * width + y] = data[y * height + (height - x -1)];
    }
  }


  Efi_Free(data);
  board->data = rotated;

  int tmp = board->width;
  board->width = board->height;
  board->height = tmp;
}

int Board_GetSize(const Board* board) {
  if (board == NULL)
    return 0;
  if (board->data == NULL)
    return 0;
  return board->width * board->height;
}
