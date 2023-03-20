#ifndef TETRIS_UI_H
#define TETRIS_UI_H

#include "Efi/Stdint.h"

#include "Tetris.h"

#define TETRIS_KEY_REFRESH 'u'
#define TETRIS_KEY_EXIT 'q'
#define TETRIS_KEY_PAUSE 'p'

#define TETRIS_KEY_LEFT 'a'
#define TETRIS_KEY_RIGHT 'd'
#define TETRIS_KEY_ROTATE 'w'
#define TETRIS_KEY_DOWN 's'
#define TETRIS_KEY_HOLD 'h'

void Ui_PrintLogo(int maxX, int maxY);

// Draws all exlude Ui_GameOver
void Ui_DrawAll(const Tetris_Data* game);
void Ui_DrawHelp(const Tetris_Data* game);
void Ui_DrawStats(const Tetris_Data* game);
void Ui_DrawBoardBorders(const Tetris_Data* game);
void Ui_DrawBoard(const Tetris_Data* game);
void Ui_DrawGameOver(const Tetris_Data* game);
void Ui_DrawNextTetromino(const Tetris_Data* game);
void Ui_DrawHoldTetromino(const Tetris_Data* game);

int Ui_HandleGameOverInput(Tetris_Data *game);
int Ui_HandleInput(Tetris_Data *game);

#endif  // TETRIS_UI_H
