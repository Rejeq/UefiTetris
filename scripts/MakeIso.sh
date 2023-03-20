#! /bin/bash

FatPath=$1
FatName=$(basename -- "$FatPath")
IsoPath=$FatName.iso

echo Creating ISO
xorriso -as mkisofs -R -f -e $FatName -no-emul-boot -o $IsoPath $FatPath
