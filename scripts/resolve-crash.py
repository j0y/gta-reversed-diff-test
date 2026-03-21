#!/usr/bin/env python3
"""Resolve a crash address to a function name using an MSVC .map file.

Usage:
    # With absolute crash address and ASI base:
    resolve-crash.py --map gta_reversed.map --base 0x75CC0000 --addr 0x76369018

    # With offset from ASI base:
    resolve-crash.py --map gta_reversed.map --offset 0x6A9018

    # Parse crash from Wine stderr (pipe wine output):
    wine gta_sa.exe 2>&1 | resolve-crash.py --map gta_reversed.map --wine-stderr
"""
import argparse
import re
import sys


def parse_map_file(map_path):
    """Parse MSVC .map file and return sorted list of (rva, name) tuples."""
    symbols = []
    in_publics = False

    with open(map_path, 'r', errors='replace') as f:
        for line in f:
            # Look for the "Publics by Value" section
            if 'Publics by Value' in line or 'Address' in line and 'Publics' in line:
                in_publics = True
                continue

            if in_publics:
                # Blank line or new section ends publics
                if line.strip() == '' and symbols:
                    # Could be a separator; keep going
                    continue
                if line.startswith(' entry point') or line.startswith(' Static symbols'):
                    break

                # MSVC map format: " SSSS:OOOOOOOO  name  AAAAAAAA  module"
                # Section:Offset   Name   AbsoluteAddr   Object
                m = re.match(
                    r'\s+([0-9a-fA-F]+):([0-9a-fA-F]+)\s+(\S+)\s+([0-9a-fA-F]+)',
                    line
                )
                if m:
                    section = int(m.group(1), 16)
                    offset = int(m.group(2), 16)
                    name = m.group(3)
                    abs_addr = int(m.group(4), 16)
                    # We use the absolute address from the map (preferred base + offset)
                    symbols.append((abs_addr, name, section, offset))

    symbols.sort(key=lambda x: x[0])
    return symbols


def parse_sections(map_path):
    """Parse section info from the map file header."""
    sections = {}
    with open(map_path, 'r', errors='replace') as f:
        for line in f:
            # " 0001:00000000 00f7b000H .text"
            m = re.match(
                r'\s+([0-9a-fA-F]+):([0-9a-fA-F]+)\s+([0-9a-fA-F]+)H\s+(\S+)',
                line
            )
            if m:
                sec_num = int(m.group(1), 16)
                sec_start = int(m.group(2), 16)
                sec_size = int(m.group(3), 16)
                sec_name = m.group(4)
                sections[sec_num] = (sec_name, sec_start, sec_size)
    return sections


def get_preferred_base(map_path):
    """Extract preferred load address from map file."""
    with open(map_path, 'r', errors='replace') as f:
        for line in f:
            m = re.match(r'\s*Preferred load address is\s+([0-9a-fA-F]+)', line)
            if m:
                return int(m.group(1), 16)
    return 0x10000000  # MSVC default for DLLs


def resolve(symbols, preferred_base, target_rva):
    """Find the symbol at or before target_rva."""
    target_abs = preferred_base + target_rva

    best = None
    for abs_addr, name, section, offset in symbols:
        if abs_addr <= target_abs:
            best = (abs_addr, name, section, offset)
        else:
            break

    if best is None:
        return None

    abs_addr, name, section, offset = best
    delta = target_abs - abs_addr
    return name, delta, section, offset


def demangle_msvc(name):
    """Basic MSVC name demangling (just strips leading ? and underscores)."""
    # Full demangling would need undname.exe or a library.
    # This is a best-effort for readability.
    if name.startswith('?'):
        # Try to extract class::method from ?method@class@@...
        parts = name[1:].split('@')
        if len(parts) >= 2 and parts[1] != '':
            method = parts[0]
            cls = parts[1]
            if cls.startswith('?'):
                cls = cls[1:]
            return f'{cls}::{method}'
    if name.startswith('_'):
        return name[1:]
    return name


def parse_wine_crash(line):
    """Extract crash address from Wine stderr line.

    Patterns:
        "page fault on read access to XXXXXXXX at address YYYYYYYY"
        "code=c0000005 ... addr=YYYYYYYY ip=YYYYYYYY"
    """
    # Pattern 1: page fault message
    m = re.search(r'at address ([0-9a-fA-F]+)', line)
    if m:
        return int(m.group(1), 16)

    # Pattern 2: seh trace
    m = re.search(r'ip=([0-9a-fA-F]+)', line)
    if m:
        return int(m.group(1), 16)

    return None


def parse_wine_base(lines):
    """Extract ASI base address from Wine module trace.

    Pattern: 'Loaded module L"...gta_reversed.asi" at XXXXXXXX'
             'mapping PE file L"...gta_reversed.asi" at 0xXXXXXXXX-...'
    """
    for line in lines:
        m = re.search(
            r'(?:Loaded module|mapping PE file).*gta_reversed\.asi.*?(?:at |at 0x)([0-9a-fA-F]+)',
            line
        )
        if m:
            return int(m.group(1), 16)
    return None


def main():
    parser = argparse.ArgumentParser(description='Resolve crash address to function name')
    parser.add_argument('--map', required=True, help='Path to MSVC .map file')
    parser.add_argument('--addr', help='Absolute crash address (hex)')
    parser.add_argument('--base', help='ASI base address at runtime (hex)')
    parser.add_argument('--offset', help='Offset from ASI base (hex)')
    parser.add_argument('--wine-stderr', action='store_true',
                        help='Parse Wine stderr from stdin for crash info')
    args = parser.parse_args()

    print(f'Loading map file: {args.map}', file=sys.stderr)
    symbols = parse_map_file(args.map)
    preferred_base = get_preferred_base(args.map)
    sections = parse_sections(args.map)

    print(f'Preferred base: 0x{preferred_base:08X}', file=sys.stderr)
    print(f'Symbols loaded: {len(symbols)}', file=sys.stderr)
    print(f'Sections: {", ".join(f"{v[0]}" for v in sections.values())}', file=sys.stderr)

    if not symbols:
        print('ERROR: No symbols found in map file. Is it a valid MSVC .map file?',
              file=sys.stderr)
        sys.exit(1)

    if args.wine_stderr:
        lines = sys.stdin.readlines()
        crash_addr = None
        base_addr = None

        for line in lines:
            if crash_addr is None:
                crash_addr = parse_wine_crash(line)
            if base_addr is None and 'gta_reversed' in line:
                base_addr = parse_wine_base([line])

        if crash_addr is None:
            print('No crash address found in Wine output', file=sys.stderr)
            # Still print the output for reference
            for line in lines[-20:]:
                print(line.rstrip())
            sys.exit(1)

        if base_addr is not None:
            rva = crash_addr - base_addr
            print(f'\nCrash at 0x{crash_addr:08X}, ASI base 0x{base_addr:08X}, '
                  f'offset 0x{rva:08X}')
        else:
            # Assume crash might be relative to preferred base
            rva = crash_addr - preferred_base
            if 0 <= rva < 0x2000000:  # reasonable range
                print(f'\nCrash at 0x{crash_addr:08X}, '
                      f'offset 0x{rva:08X} (assumed preferred base)')
            else:
                print(f'\nCrash at 0x{crash_addr:08X} — '
                      f'NOT in ASI address range (preferred base 0x{preferred_base:08X})')
                sys.exit(1)

        target_rva = rva

    elif args.offset:
        target_rva = int(args.offset, 16)
        print(f'Offset: 0x{target_rva:08X}')

    elif args.addr and args.base:
        addr = int(args.addr, 16)
        base = int(args.base, 16)
        target_rva = addr - base
        print(f'Address: 0x{addr:08X}, Base: 0x{base:08X}, Offset: 0x{target_rva:08X}')

    elif args.addr:
        # Try using preferred base
        addr = int(args.addr, 16)
        target_rva = addr - preferred_base
        print(f'Address: 0x{addr:08X}, '
              f'Offset: 0x{target_rva:08X} (from preferred base)')

    else:
        parser.error('Provide --offset, --addr, or --wine-stderr')

    result = resolve(symbols, preferred_base, target_rva)
    if result:
        name, delta, section, offset = result
        demangled = demangle_msvc(name)
        print(f'\n  Function: {demangled}')
        print(f'  Mangled:  {name}')
        print(f'  At:       section {section}:0x{offset:08X} + 0x{delta:X}')
        if delta > 0x10000:
            print(f'  WARNING: Large offset from symbol — '
                  f'might be in a different (unnamed) function')
    else:
        print(f'\n  No symbol found for offset 0x{target_rva:08X}')


if __name__ == '__main__':
    main()
