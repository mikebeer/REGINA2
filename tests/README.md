# Regression tests for the OO extension

`run-regression.sh` builds nothing; point it at a `rexx` binary:

```sh
tests/run-regression.sh /path/to/build/rexx
```

| Suite | Programs | Expected output recorded from |
|---|---|---|
| `trip` | Regina's own trip suite in `../trip` | unmodified Regina 3.9.7 |
| `classic` | `tests/classic/*.rexx`: classic behaviour the OO syntax must not change (`~` as NOT, `-` and `>` near each other, brackets in strings, labels, PARSE templates, `USE`/`ARG` as plain symbols) | unmodified Regina 3.9.7 |
| `oo` | `tests/oo/*.rexx`: the new syntax (`~`, `->`, `~~`, `->>`, `[ ]`, `::` directives) | reviewed output of the OO build |

`trip` and `classic` must stay identical to 3.9.7. Never re-record them
from a modified build; a difference there means classic behaviour changed.
Re-record `oo` after an intended change and review the diff:

```sh
tests/run-regression.sh /path/to/build/rexx --record oo
```

## Building

Build serially (`make`, not `make -j`): the Makefile renames `extstack.o`
during the build and races under `-j`.

After editing `lexsrc.l` or `yaccsrc.y`, run `tools/regenerate-parser.sh`
(flex 2.6, bison 3) and commit the generated `lexsrc.c`, `yaccsrc.c` and
`yaccsrc.h` together with the sources.

## Packages

| Command | Result |
|---|---|
| `tools/build-deb.sh [OUTDIR]` | `regina-rexx-oo_<ver>_<arch>.deb` for Debian/Ubuntu. Runs the regression suites first and refuses to package if they fail. Replaces the distribution's `regina-rexx`/`libregina3`. |
| `tools/build-windows.sh [OUTDIR]` | Standalone 64-bit `rexx.exe` (no DLL) and `regina-rexx-oo-<ver>-win64.zip`, cross-compiled with MinGW-w64 (`apt install mingw-w64`). |

## Switches for a test

If `NAME.switches` exists next to `NAME.rexx`, its content is passed to the
interpreter before the program, for example `--oo` or `--no-oo`.
