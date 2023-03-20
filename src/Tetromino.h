#ifndef TETRIS_TETROMINO_H
#define TETRIS_TETROMINO_H

#define TETROMINO_COUNT 7
#define TETROMINO_I 0
#define TETROMINO_J 1
#define TETROMINO_L 2
#define TETROMINO_O 3
#define TETROMINO_T 4
#define TETROMINO_S 5
#define TETROMINO_Z 6

#include "Board.h"

void TETRISAPI Tetris_CreateI(Board *tetroI, state_t state);
void TETRISAPI Tetris_CreateJ(Board *tetroJ, state_t state);
void TETRISAPI Tetris_CreateL(Board *tetroL, state_t state);
void TETRISAPI Tetris_CreateO(Board *tetroO, state_t state);
void TETRISAPI Tetris_CreateT(Board *tetroT, state_t state);
void TETRISAPI Tetris_CreateS(Board *tetroS, state_t state);
void TETRISAPI Tetris_CreateZ(Board *tetroZ, state_t state);

#endif // TETRIS_TETROMINO_H
