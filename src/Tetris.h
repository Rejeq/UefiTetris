#ifndef TETRIS_H
#define TETRIS_H

#include "Efi/Stdint.h"

#include "Board.h"
#include "Renderer.h"
#include "Tetromino.h"

// Initial speed, in milliseconds
#define TETRIS_INIT_SPEED (500)

// Scoring: score is increased by the product of the current level and a factor
// corresponding to the number of rows cleared.
#define TETRIS_SCORE_SINGLE (100)
#define TETRIS_SCORE_DOUBLE (300)
#define TETRIS_SCORE_TRIPLE (500)
#define TETRIS_SCORE_TETRIS (800)
// Amount to increase the score for a soft drop
#define TETRIS_SCORE_SOFT_DROP (1)
// Factor by which to multiply the number of rows dropped to increase the score
// for a hard drop
#define TETRIS_SCORE_HARD_DROP (2)
// Number of rows that need to be cleared to increase level
#define TETRIS_ROWS_PER_LEVEL (10)

typedef struct Tetris {
  int level;
  int score;
  int speed;
  int ended;

  size_t totalRows;
  size_t totalCols;

  Board board;
  int boardPosX;
  int boardPosY;

  Ren_Data *renData;

  Board tetros[TETROMINO_COUNT];

  Board *holdTetro;
  Board *prevHoldTetro;
  int holdUsed;

  Board *nextActiveTetro;
  Board *currentActiveTetro;  // Same as activeTetro but ignoring rotates.
  Board activeTetro;
  int activeX;
  int activeY;
  int activeDropY;

  int *clearRowsData;
  int clearRowsSize;   // Allocated size
  int clearRowsCount;  // Already in use count
  int clearRowsTotal;  // Total number of cleared rows

  int showHelp;
  int showStats;
} Tetris_Data;

int TETRISAPI Tetris_Init(Tetris_Data *game, int width, int height);
void TETRISAPI Tetris_Shutdown(Tetris_Data *game);

// Retains active tetromino in the hold buffer
// If the buffer is already occupied, similarly with replacing activeTetromino
// by hold buffer return 1 - on failure, if the hold buffer is used, and
// activeTetromino is not dropped return 0 - on success
int TETRISAPI Tetris_SwapHold(Tetris_Data *game);

// Set active board, its not inserted in main board.
void TETRISAPI Tetris_SetActive(Tetris_Data *game, Board *active);

// Moves the active board to the point.
// If collision detected, do nothing.
// Pass the argument < 0 if you don't want to change the axis position
Collision TETRISAPI Tetris_MoveActive(Tetris_Data *game, int x, int y);

// Relative move the active board.
// If collision detected, do nothing
Collision TETRISAPI Tetris_MoveRelativeActive(Tetris_Data *game, int relX,
                                                    int relY);

// Sets position for active tetro to initial position
void TETRISAPI Tetris_ResetActivePosition(Tetris_Data *game);

// Rotates active board in the clockwise direction
// Return 1 if unable to rotate
// Return 0 on success
int TETRISAPI Tetris_RotateActive(Tetris_Data *game);

// Inserts the active board into the main board
// Resets active board
// And deletes full rows if exist
void TETRISAPI Tetris_InsertActive(Tetris_Data *game);

void TETRISAPI Tetris_DropActive(Tetris_Data *game);

// void TETRISAPI Tetris_Pause(Tetris_Data *game);
// void TETRISAPI Tetris_Exit(Tetris_Data *game);

void TETRISAPI Tetris_Update(Tetris_Data *game);

#endif  // TETRIS_H
