# UefiTetris

## Table of Contents
- [About](#about)
- [Building](#building)
- [Launching](#launching)
- [Debugging](#debugging)
- [Troubleshooting](#troubleshooting)
- [Related](#related)

## About
A tetris game, that can be launched in UEFI.

**Features:**
- Scoring
- Levels
- Tetromino preview
- Tetromino hold
- Ghost tetromino
- Rotation system and wall kicks

## Building
This project can be builded with severel efi providers:
- [gnu-efi](https://sourceforge.net/projects/gnu-efi/)
- [EDK2](https://github.com/tianocore/edk2)

Building some different depending of the provider
First make some common stuff:

```shell
git clone https://github.com/Rejeq/UefiTetris
cd UefiTetris
mkdir build
cd build
```

##### Gnu-efi

```shell
...
cmake -DTETRIS_ARCH="x86_64" -DTETRIS_EFI_PROVIDER="gnu-efi" ..
cmake --build . -j 2
```
The gnu-efi library will be automatically loaded, but if you have problems, you can install it manually by setting TETRIS_FETCHING=OFF and providing GNU_EFI_DIR path to the efi directory

##### EDK2
**If you use clang, you must also install the llvmGold extension**

```shell
...
cmake -DTETRIS_ARCH="x86_64" -DTETRIS_EFI_PROVIDER="edk" -DTETRIS_EDK_REPO="<path to edk repo" ..
cd edk
source edksetup.sh BaseTools
cd ..
build
```

If the build process was successful, the binaries should be in: `./<build_mode><compiler>/<arch>/TetrisApp.efi`

## Launching

#### Qemu
First of all, we need to get OVMF
This can be obtained by several methods:
1. Using the package manager
	In Ubuntu run: `sudo apt ovmf`
	In other distros must be similar
2. [Manual building](https://github.com/tianocore/tianocore.github.io/wiki/How-to-build-OVMF)
	If you have troubles, you can take the commands from nightly build github repository
1. Download [nightly build](https://retrage.github.io/edk2-nightly/).

Next we need to create FAT image (**mtools required**). For gnu-efi it will be created automatically, so you can skip this point
```shell
bash scripts/MakeFat.sh [Efi program]
bash scripts/RunQemu.sh [FAT image] --arch="x86_64" --ovmf=[path to ovmf]
```

> Or if you unable to create FAT image you can use -hda flag
> ```shell
> mkdir -p boot/EFI/BOOT
> cp [Efi program] boot/EFI/BOOT/BOOTX64.EFI
> bash scripts/RunQemu.sh --arch="x86_64" --ovmf=[path to ovmf] --no-image -- -hda fat:rw:boot
> ```

#### VirtualBox
Create .iso file by running: (**xorriso required**)
```shell
bash ../scripts/MakeIso.sh [FAT image]
```
Create  machine as usual, and set `Enable EFI` flag

> Not forget to set correct architecture
> Also you can set minimum ram and disk space

## Debugging
Go to build dir and reconfigure cmake:
```shell
cmake -DCMAKE_BUILD_Type=Debug -DTETRIS_LOGGING=ON ..
```

Rebuild project
Run tetris, and you will see in the console "ImageBase: 0x--------"
Meomrize the hex value and start gdb session:

```shell
gdb --command=../scripts/GdbSetupEfi.py'
(gdb) SetupEfi [Efi program] [Efi debug program] [ImageBase value]
(gdb) b efi_main
(gdb) target remote :1234
```

In other terminal you need to run qemu

```shell
bash ../scripts/RunQemu.sh [FAT image] --debug=true
```

## Troubleshooting
#### Black screen in VirtualBox
If you only see a black screen, you may have forgotten to set the Efi flag during the machine creation phase.

#### Always returning to the shell
Try executing the program manually with:
 ```sh
$ fs0:  # go to the boot filesystem
$ EFI\BOOT\BOOTX64.efi
```
If nothing appears after that and you just go back to the shell, it could be due to the different architecture of the machine and the program

## Related
- [Tetris guideline](https://tetris.wiki/Tetris_Guideline)
- [tetrEFIs](https://github.com/tsani/tetrefis) - A similar project with graphics output
- [About UEFI](https://wiki.osdev.org/UEFI)
- [Efi application debugging](https://wiki.osdev.org/Debugging_UEFI_applications_with_GDB)
