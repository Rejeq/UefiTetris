#include "Efi/Memory.h"
#include "Efi/Misc.h"

#include "Board.h"
#include "Log.h"
#include "Renderer.h"
#include "Tetris.h"
#include "Ui.h"

#define GAME_WIDTH 10
#define GAME_HEIGHT 20

// Clang with efi_application subsystem don't link to msvcrt, but use msvc ABI,
// so _fltused symbol should be manually provided
// Details:
// https://github.com/rust-lang/rust/issues/62785/#issuecomment-530926991
#ifdef __clang__
int _fltused = 0;
#endif  // __clang__

// Needed for debuging
void TETRISAPI PrintImageBase(EFI_HANDLE *handle) {
  if (handle == NULL)
    return;

  Efi_LoadedImageProtocol *image = NULL;

  image = Efi_GetLoadedImage(handle);
  if (image == NULL) {
    LOG_ERROR(L"Unable to determine image base ptr");
    // LOG_ERROR(
    //     L"Unable to determine image base ptr, cannot handle "
    //     L"LoadedImageProtocol: %r",
    //     status);
    return;
  }

  LOG_DEBUG(L"Image base: 0x%lx", image->ImageBase);
}

// Return 1 - if want exit
int TETRISAPI RunTetris() {
  LOG_INFO(L"Starting tetris");

  Tetris_Data game;
  if (Tetris_Init(&game, GAME_WIDTH, GAME_HEIGHT)) {
    LOG_CRITICAL(L"Unable to init game. Shutdowning");
    Efi_WaitAnyKey();
    Tetris_Shutdown(&game);
    return 0;
  }

#ifdef TETRIS_LOG_ENABLED
  Efi_WaitAnyKey();
#endif

  Ren_Clear();
  Ui_PrintLogo(game.totalCols, game.totalRows);

  Efi_WaitAnyKey();

  Ren_Clear();

  Ui_DrawAll(&game);
  // Efi_WaitAnyKey();

  while (!game.ended) {
    Tetris_Update(&game);

    Ui_DrawBoard(&game);
    Ui_DrawStats(&game);

    Ui_HandleInput(&game);
    Efi_MicroSleep(game.speed * 1000);
  }

  Ren_Clear();
  Ui_DrawGameOver(&game);
  int wantExit = Ui_HandleGameOverInput(&game);

  Tetris_Shutdown(&game);

#ifdef TETRIS_EFI_DEBUG_MEMORY
  LOG_INFO(L"Total malloc: %d", Efi_DbgGetAllocateCount());
  LOG_INFO(L"Total free: %d", Efi_DbgGetFreeCount());
  Efi_WaitAnyKey();
#endif

  return wantExit;
}

EFI_STATUS TETRISAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systab) {
#ifdef TETRIS_EFI_PROVIDER_GNU_EFI
  // setup ST, BT and RT global variables
  InitializeLib(image, systab);
#endif

#ifdef TETRIS_LOG_ENABLED
  Efi_ClearScreen();
  if (Log_Init(0, 0, 20)) {
    Efi_Print(
        L"Unable to init log system. Logging will be disabled\n"
        L"Press any key to continue...\n");
    Efi_WaitAnyKey();
  }
#endif

  PrintImageBase(&image);
  LOG_DEBUG(L"Entry point (efi_main address): 0x%08x\r\n", (char16_t*)efi_main);

  int wantExit = 0;
  do {
    wantExit = RunTetris();
  } while (!wantExit);

#ifdef TETRIS_LOG_ENABLED
  Log_Destroy();
#endif

  return EFI_SUCCESS;
}
