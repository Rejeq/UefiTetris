#ifndef TETRIS_RENDERER_H
#define TETRIS_RENDERER_H

#include "Board.h"
#include "Efi/Stdint.h"

typedef struct Ren_Data Ren_Data;

Ren_Data* TETRISAPI Ren_Create();
void TETRISAPI Ren_Destroy(Ren_Data* data);

// Save current console state (position, attributes etc)
void TETRISAPI Ren_Save(Ren_Data* data);

// Save current console state (position, attributes etc)
void TETRISAPI Ren_SetColor(Ren_Data* data, char ch, int fg, int bg);

// Restore saved console state
void TETRISAPI Ren_Restore(const Ren_Data* data);

void TETRISAPI Ren_PutStr(int x, int y, int fg, int bg, const char16_t* fmt, ...);

int TETRISAPI Ren_Clear(void);

// Draws the board at the given position
// ActiveChars argument is the character that will be printed if board active at that point
// Pass '\0' to print the state of the point, which is stored in the board
// EmptyChars argument is the character that will be printed if board not active at those point
// Pass '\0' to just skip character
int TETRISAPI Ren_Draw(const Ren_Data* ren, const Board* board, int posX, int posY, char activeChar, char emptyChar, int checkBackground);
// int Ren_DrawChunk(const Board* board, int posX, int posY);

#endif  // TETRIS_RENDERER_H
