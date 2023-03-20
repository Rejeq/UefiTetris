#! /bin/bash

function display_help() {
  echo "Usage: $0 [options] -o [Path to the OVMF] [Path to the EFI image (*.iso or *.img)]"
  echo "Options:"
  echo "  -a, --arch     Run qemu in the specified architecture [default: x86_64]"
  echo "  -d, --debug    Run QEMU in debug mode [default: false]"
  echo "  -h, --help     Display this help message"
  echo "  -n  --no-image Disables image check when using -hda or similar options in extra flags"
  echo "  -o, --ovmf     Set the path to the OVMF binaries"
  echo "  --             Set extra flags to pass to the qemu"
}

ImagePath=""
Arch=""
IsDebug=false
NoImage=false
OvmfPath=""
Extra_Flags=""

while [ $# -gt 0 ]; do
  case "$1" in
    -h|--help)
      display_help
      exit 0
      ;;
    -a|--arch=*)
      Arch="${1#*=}"
      ;;
    -o|--ovmf=*)
      OvmfPath="${1#*=}"
      ;;
    -d|--debug=*)
      IsDebug="${1#*=}"
      ;;
    -n|--no-image)
      NoImage=true
      ;;
    --)
      Extra_Flags+="${@:2}"
      break
      ;;
    *)
      if [ -n "$ImagePath" ]; then
        echo "Error: Boot file already provided: '$ImagePath'"
        echo "Invalid argument: $1"
        exit 1
      fi
      ImagePath=$1
      ;;
  esac
  shift
done


Flags=""
Flags+="-net none "
# Flags+="-machine q35 -m 1024 -cpu qemu64 "

if [ "$NoImage" = false ]; then
  if [ -z "$ImagePath" ]; then
    echo "Error: Boot file not specified"
    exit 1
  fi

  BootExt="${ImagePath##*.}"
  case $BootExt in
    "iso") Flags+="-cdrom $ImagePath" ;;
    "img") Flags+="-drive file=$ImagePath,if=ide " ;;
    *)
      echo "Error: Undefined boot extension: $ImagePath"
      exit 1
  esac
fi

if [ -z "$Arch" ]; then
  echo "Arch not specified. It will be treated as 'x86_64'"
  Arch="x86_64"
fi

if [ -z "$OvmfPath" ]; then
  OvmfPath="/usr/share/qemu/ovmf-x86_64.bin"
fi
Flags+="-pflash $OvmfPath "

if [ "$IsDebug" = true ]; then
  Flags+="-debugcon file:debug.log -global isa-debugcon.iobase=0x402 -gdb tcp::1234 "
fi

Flags+=$Extra_Flags

echo "Qemu flags: $Flags"
qemu-system-$Arch $Flags
