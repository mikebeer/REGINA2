#!/bin/sh
# Cross-compile a standalone 64-bit Windows rexx.exe with MinGW-w64.
#
#   tools/build-windows.sh [OUTDIR]          (default: build-win64)
#
# Needs x86_64-w64-mingw32-gcc (Debian/Ubuntu package mingw-w64).
# The result does not need regina.dll.
set -e
SRC=$(cd "$(dirname "$0")/.." && pwd)
OUT=${1:-build-win64}
mkdir -p "$OUT"; OUT=$(cd "$OUT" && pwd)
CC=${CC:-x86_64-w64-mingw32-gcc}
WINDRES=${WINDRES:-x86_64-w64-mingw32-windres}

. "$SRC/regina.ver"

mkdir -p "$OUT"
cat > "$OUT/version_defs.h" <<EOV
#define REGINA_VERSION_DATE "$VER_DATE"
#define REGINA_VERSION_MAJOR "$VER_MAJOR"
#define REGINA_VERSION_MINOR "$VER_MINOR"
#define REGINA_VERSION_RELEASE "$VER_RELEASE"
#define REGINA_VERSION_SUPP "$VER_SUPP"
#ifndef min
# define min(a,b) ((a) < (b) ? (a) : (b))
#endif
EOV

CFLAGS="-O2 -DNDEBUG -funsigned-char -DWIN32 -DWIN64 -DHAVE_PROTO \
 -DREGINA_SINGLE_THREADED -DREGINA_BITS=64 -include stdint.h -include $OUT/version_defs.h \
 -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -w \
 -I$SRC -I$SRC/contrib"

SOURCES="funcs builtin error variable interprt debug dbgfuncs memory parsing
 files misc unxfuncs arxfuncs cmsfuncs os2funcs shell rexxext stack extstack
 tracing interp cmath convert strengs library strmath signals macros envir
 expr instore rexxbif yaccsrc lexsrc options os_win wrappers mt_notmt
 os_other mygetopt rexx nosaa oo"

cd "$OUT"
OBJS=""
for f in $SOURCES; do
  extra=""
  [ "$f" = yaccsrc ] && extra="-DYYMAXDEPTH=10000"
  $CC -c $CFLAGS $extra -o $f.o "$SRC/$f.c"
  OBJS="$OBJS $f.o"
done
$CC -c $CFLAGS -o time64.o "$SRC/contrib/time64.c"
OBJS="$OBJS time64.o"

$CC -static -Wl,--stack,268435456 -o rexx.exe $OBJS -lwsock32 -ladvapi32 -lpthread
x86_64-w64-mingw32-strip rexx.exe
echo "built $OUT/rexx.exe"

# distribution zip: rexx.exe, message files, demos, licence
DIST="regina-rexx-oo-$VER_DOT-win64"
rm -rf "$DIST" "$DIST.zip"; mkdir -p "$DIST/demo"
cp rexx.exe "$DIST/"
cp "$SRC"/*.mtb "$DIST/"
cp "$SRC"/demo/*.rexx "$DIST/demo/"
cp "$SRC/COPYING-LIB" "$DIST/COPYING-LIB.txt"
cp "$SRC/README.OO.md" "$DIST/README.OO.md"
cat > "$DIST/README.txt" <<EOR
Regina Rexx $VER_DOT with optional OO syntax - 64-bit Windows

rexx.exe is a standalone interpreter; it needs no DLL and no installation.
Run a program with:   rexx.exe myprog.rexx

Classic Rexx programs run unchanged. New, optional syntax:
  obj~method(args)   obj -> method(args)     message send
  obj~~m             obj ->> m(1) ->> n      cascade
  a[i]  a[i, j]                              array reference
  ::class, ::method, ::routine, ...          directives
The object system is not implemented yet: OO terms raise error 91.

Error messages in other languages: set REGINA_LANG (e.g. de) and
REGINA_LANG_DIR to the folder containing the *.mtb files.

Source: https://github.com/mikebeer/REGINA2
EOR
sed -i 's/$/\r/' "$DIST/README.txt"
(command -v zip >/dev/null && zip -qr "$DIST.zip" "$DIST") || python3 -c "import shutil,sys; shutil.make_archive(sys.argv[1], 'zip', '.', sys.argv[1])" "$DIST"
echo "built $OUT/$DIST.zip"
