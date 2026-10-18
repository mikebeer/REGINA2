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
| `samples` | the sample programs in `demo/oo` | reviewed output of the OO build |
| `packages` | `tests/packages/*.rexx`: external function packages (rgnet, RgMqtt) | reviewed output of the OO build |

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

## Windows

`tools/test-windows.sh [ZIP]` tests the Windows zip under Wine: the oo and
samples suites with `rexx.exe` and `regina.exe`, the MQTT tests and samples
with `regina.exe`, and, through `tools/wine-console.py`, programs run with a
console (typed input, an empty line, Ctrl-Z, `--stack`). Run it before
publishing a Windows package: problems with the console don't show when
input comes from a file or pipe.

## Switches for a test

If `NAME.switches` exists next to `NAME.rexx`, its content is passed to the
interpreter before the program, for example `--oo` or `--no-oo`.

## Packages

The `packages` suite runs `tests/packages/*.rexx` with the dynamic
`regina` found next to the given `rexx` (it is skipped, with a note, if
there is none, or no libregina, C compiler or python3). It compiles
`rgnet.c`, a small RxSock-style test package with its own function names,
and uses `librgmqtt.so` of the build (or compiles `rgmqtt/rgmqtt.c`). If
`NAME.server` exists, the Python server it names (`echoserver.py`,
`minibroker.py`) is started first and its port is passed as the
argument. The classic programs (`net_classic`, `mqtt_functions`) give the
same output with unmodified Regina 3.9.7.

## Samples

The `samples` suite runs the sample programs in `demo/oo` and compares
their output with `tests/expected/samples`, so the samples in the
packages and the manual keep working.
