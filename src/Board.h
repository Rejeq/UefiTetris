#ifndef TETRIS_BOARD_H
#define TETRIS_BOARD_H

#include "Efi/Stdint.h"

#define BOARD_DATA_STATE(data, x, y, stride) ((data)[(y) * (stride) + (x)])
#define BOARD_STATE(board, x, y) \
  BOARD_DATA_STATE((board)->data, x, y, (board)->width)
#define BOARD_IS_ACTIVE(board, x, y) (BOARD_STATE(board, x, y) != 0)

#define BOARD_VALID(board) ((board)->width != 0 && (board)->height != 0)

typedef enum Collision {
  BOARD_COLLISION_NONE,
  BOARD_COLLISION_BOARD,   // On collisions inside the board
  BOARD_COLLISION_BOUNDS,  // On collisions outside the board
} Collision;

typedef char state_t;

typedef struct Board {
  state_t* data;
  int width;
  int height;
} Board;

Board TETRISAPI Board_Create(int width, int height);
// void Board_Init(Board* board, int width, int height);
void TETRISAPI Board_Destroy(Board* board);

// Deep copy from src to dst
// Dst will be realocated only when its size is smaller than src size
// Return 1 if error occured
// Return 0 on success
int TETRISAPI Board_CloneTo(Board* dst, const Board* src);

// Same as Board_CloneTo(&nullBoard, src)
Board TETRISAPI Board_Clone(const Board* src);

// Checks for collision in main and sub when it is placed at the (x, y) point.
// Returns true if the sub collides with main, or goes out of bounds
Collision TETRISAPI Board_CheckCollision(const Board* main, const Board* sub, int x,
                                     int y);

// Inserts the sub board into the main board at (x, y) point
int TETRISAPI Board_Insert(Board* main, const Board* sub, int x, int y);

// Return 1 if row fully not empty
// Return 0 if at least one cell is 0
int TETRISAPI Board_CheckRow(const Board* board, int y);

// Copies the specified row and places it to the row
// Return 1 if from or to not in board.height range
// Return 0 on success
int TETRISAPI Board_SetRowFrom(Board* board, int from, int to);

// Set the specified row to the state
// Return 1 if y not in board.height range
// Return 0 on success
int TETRISAPI Board_SetRow(Board* board, int y, state_t state);

// Rotate the board in clockwise direction
void TETRISAPI Board_CwRotate(Board* board);

// Rotate the board in counter-clockwise direction
void TETRISAPI Board_CcwRotate(Board* board);

// Return board.width * board.height
int TETRISAPI Board_GetSize(const Board* board);

// Return the minium y position of sub board in main board
// If error occured return -1
int TETRISAPI Board_GetMinDownPosition(const Board* main, const Board* sub, int x, int y);

#endif  // TETRIS_BOARD_H
