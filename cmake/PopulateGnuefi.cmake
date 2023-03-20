find_package(Gnuefi REQUIRED)

add_executable(UefiTetris ${TETRIS_SOURCE})
set_target_properties(UefiTetris PROPERTIES SUFFIX ".efi")

message(STATUS "Tetris definitions: ${TETRIS_DEFINITIONS}")
target_compile_definitions(UefiTetris PRIVATE ${TETRIS_DEFINITIONS})

target_link_libraries(UefiTetris PRIVATE
  gnuefi
)

add_custom_command(TARGET UefiTetris POST_BUILD
  COMMENT "Making FAT image"
  COMMAND sh ${TETRIS_SCRIPTS_DIR}/MakeFat.sh $<TARGET_FILE:UefiTetris>
)
