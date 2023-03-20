#!/usr/bin/python

import gdb
import re


def GetSectionAdress(input, sectionName):
    try:
        line = re.search(fr'.*\{sectionName}.*', input)
        line = line.group().strip()

        hex = re.search(r'0x[\da-fA-F]+', line)
        return int(hex.group(), 16)
    except:
        return None


def GetResultSections(targets, offsetStr):
    sections = ['.text', '.data']
    offset = int(offsetStr, 16)
    result = {}

    for section in sections:
        try:
            sectionOffset = GetSectionAdress(targets, section)
            result[section] = hex(sectionOffset + offset)
        except:
            print(f'Not found offset for "{section}" section')
            continue
    return result


def GenerateSectionArgs(sections):
    result = ''
    for name, offset in sections.items():
        result += offset if name == '.text' else f'-s {name} {offset}'
        result += ' '
    return result


class SetupEfi(gdb.Command):
    """
    Setup efi executable for debugging.
    SetupEfi [file.efi] [file.efi.debug] [image base adress]
    For details see: https://wiki.osdev.org/Debugging_UEFI_applications_with_GDB
    """

    def __init__(self):
        super(SetupEfi, self).__init__('SetupEfi', gdb.COMMAND_USER)

    def invoke(self, argument, from_tty):
        argv = gdb.string_to_argv(argument)
        efiFile = argv[0]
        efiDebugFile = argv[1]
        offset = argv[2]

        gdb.execute('file')
        gdb.execute(f'file {efiFile}')

        targets = gdb.execute('info files', to_string=True)
        print(targets)
        sections = GetResultSections(targets, offset)
        sectionArgs = GenerateSectionArgs(sections)

        gdb.execute('file')
        gdb.execute(
            f'add-symbol-file {efiDebugFile} {sectionArgs}', from_tty=True)

    def complete(self, text, word):
        return gdb.COMPLETE_FILENAME


SetupEfi()
