#! /bin/bash

set -e

BinPath=$1
FatPath=$1.img
FatName=$(basename -- "$FatPath")

dd if=/dev/zero of=$FatPath bs=512 count=1440
mformat -i $FatPath -f 1440 ::

# Create /EFI/BOOT folder
mmd -i $FatPath ::/EFI
mmd -i $FatPath ::/EFI/BOOT

# Copy binary to /EFI/BOOT/
mcopy -i $FatPath $BinPath ::/EFI/BOOT/BOOTX64.EFI
