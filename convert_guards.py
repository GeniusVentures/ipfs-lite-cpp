#!/usr/bin/env python3
"""
convert_guards.py — Convert C/C++ header guards to #pragma once.

Usage:
    python3 convert_guards.py [path ...] [options]

Arguments:
    path            Files or directories to process (default: current directory)

Options:
    -r, --recursive     Recurse into subdirectories (default: True)
    --dry-run           Show what would change without modifying files
    --extensions        Comma-separated list of extensions to process
                        (default: h,hpp,hxx,h++)
    -v, --verbose       Print details for every file processed
"""

import argparse
import re
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Patterns
# ---------------------------------------------------------------------------

# Matches:  #ifndef SOME_GUARD_H_
IFNDEF_RE = re.compile(r'^\s*#\s*ifndef\s+(\w+)\s*$')
# Matches:  #define SOME_GUARD_H_
DEFINE_RE = re.compile(r'^\s*#\s*define\s+(\w+)\s*$')
# Matches:  #endif  /  #endif // ...  /  #endif /* ... */
ENDIF_RE  = re.compile(r'^\s*#\s*endif\b.*$')


def find_guard(lines: list[str]):
    """
    Return (ifndef_idx, define_idx, endif_idx, guard_name) if the file uses a
    traditional include guard, otherwise return None.

    Rules:
    - Skip leading blank lines and block/line comments.
    - The first real preprocessor directive must be #ifndef <NAME>.
    - The very next non-blank line must be #define <NAME> (same name).
    - The last #endif in the file is treated as the closing guard.
    """
    ifndef_idx = define_idx = endif_idx = None
    guard_name = None

    # --- locate #ifndef ---
    for i, line in enumerate(lines):
        stripped = line.strip()
        # skip blank lines and comment lines
        if not stripped or stripped.startswith('//') or stripped.startswith('/*') or stripped.startswith('*'):
            continue
        m = IFNDEF_RE.match(line)
        if m:
            ifndef_idx = i
            guard_name = m.group(1)
            break
        else:
            # First real content is not #ifndef → not a guard
            return None

    if ifndef_idx is None:
        return None

    # --- locate matching #define immediately after ---
    for i in range(ifndef_idx + 1, len(lines)):
        stripped = lines[i].strip()
        if not stripped:
            continue
        m = DEFINE_RE.match(lines[i])
        if m and m.group(1) == guard_name:
            define_idx = i
            break
        else:
            return None  # something else between #ifndef and #define

    if define_idx is None:
        return None

    # --- locate the LAST #endif in the file ---
    for i in range(len(lines) - 1, define_idx, -1):
        if ENDIF_RE.match(lines[i]):
            endif_idx = i
            break

    if endif_idx is None:
        return None

    return ifndef_idx, define_idx, endif_idx, guard_name


def convert(lines: list[str]):
    """
    Convert header-guard lines to #pragma once.
    Returns (new_lines, changed: bool, info: str).
    """
    result = find_guard(lines)
    if result is None:
        return lines, False, "no guard found"

    ifndef_idx, define_idx, endif_idx, guard_name = result

    new_lines = list(lines)

    # Replace #ifndef … / #define … with a single #pragma once
    new_lines[ifndef_idx] = '#pragma once\n'
    new_lines[define_idx] = ''          # remove #define line

    # Remove the closing #endif (and any trailing comment on that line)
    new_lines[endif_idx] = ''

    # Drop consecutive blank lines that may now appear at the top or bottom
    new_lines = _clean_blanks(new_lines, ifndef_idx)

    return new_lines, True, guard_name


def _clean_blanks(lines: list[str], pragma_idx: int) -> list[str]:
    """Remove extra blank lines immediately after #pragma once and at EOF."""
    # After #pragma once: collapse multiple blanks to one
    i = pragma_idx + 1
    while i < len(lines) - 1 and lines[i].strip() == '' and lines[i + 1].strip() == '':
        lines.pop(i)

    # Strip trailing blank lines at end of file
    while lines and lines[-1].strip() == '':
        lines.pop()

    # Ensure single newline at EOF
    if lines:
        lines[-1] = lines[-1].rstrip('\n') + '\n'

    return lines


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

DEFAULT_EXTENSIONS = {'h', 'hpp', 'hxx', 'h++'}


def collect_files(paths: list[Path], extensions: set[str], recursive: bool) -> list[Path]:
    files = []
    for p in paths:
        if p.is_file():
            if p.suffix.lstrip('.').lower() in extensions:
                files.append(p)
        elif p.is_dir():
            glob = '**/*' if recursive else '*'
            for ext in extensions:
                files.extend(p.glob(f'{glob}.{ext}'))
    return sorted(set(files))


def process_file(path: Path, dry_run: bool, verbose: bool) -> str:
    """Process a single file. Returns a status string."""
    try:
        original = path.read_text(encoding='utf-8', errors='replace')
    except OSError as e:
        return f'  ERROR reading: {e}'

    lines = original.splitlines(keepends=True)
    new_lines, changed, info = convert(lines)

    if not changed:
        if verbose:
            return f'  skipped  ({info})'
        return None  # silent skip

    new_text = ''.join(new_lines)

    if dry_run:
        return f'  would convert  guard={info}'

    try:
        path.write_text(new_text, encoding='utf-8')
        return f'  converted  guard={info}'
    except OSError as e:
        return f'  ERROR writing: {e}'


def main():
    parser = argparse.ArgumentParser(
        description='Convert C/C++ header guards to #pragma once.',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument('paths', nargs='*', default=['.'],
                        help='Files or directories to process (default: current dir)')
    parser.add_argument('--dry-run', action='store_true',
                        help='Preview changes without writing files')
    parser.add_argument('--no-recursive', dest='recursive', action='store_false',
                        help='Do not recurse into subdirectories')
    parser.add_argument('--extensions', default=','.join(sorted(DEFAULT_EXTENSIONS)),
                        help='Comma-separated extensions to process '
                             f'(default: {",".join(sorted(DEFAULT_EXTENSIONS))})')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Print a line for every file, including skipped ones')
    parser.set_defaults(recursive=True)
    args = parser.parse_args()

    extensions = {e.strip().lstrip('.').lower() for e in args.extensions.split(',')}
    paths = [Path(p) for p in args.paths]
    files = collect_files(paths, extensions, args.recursive)

    if not files:
        print('No header files found.')
        sys.exit(0)

    if args.dry_run:
        print(f'DRY RUN — no files will be modified.\n')

    converted = skipped = errors = 0

    for f in files:
        status = process_file(f, dry_run=args.dry_run, verbose=args.verbose)
        if status is None:
            skipped += 1
            continue
        print(f'{f}')
        print(status)
        if 'ERROR' in status:
            errors += 1
        elif 'skip' in status:
            skipped += 1
        else:
            converted += 1

    verb = 'Would convert' if args.dry_run else 'Converted'
    print(f'\n{verb} {converted} file(s). Skipped {skipped}. Errors {errors}.')
    sys.exit(1 if errors else 0)


if __name__ == '__main__':
    main()
