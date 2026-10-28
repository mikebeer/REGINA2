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

# Windows resources: version information (file version, product, description,
# copyright) for every binary, and the Regina icon and a manifest (normal user
# rights, Windows 10/11) for the executables. Unsigned executables without
# them are flagged by virus scanners far more often.
make_rc() {   # NAME DESCRIPTION exe|dll
  {
    echo '#include <winver.h>'
    if [ "$3" = exe ]; then
      echo "1 ICON \"$SRC/reginaw32.ico\""
      echo '1 24 "app.manifest"'
    fi
    cat <<EORC
VS_VERSION_INFO VERSIONINFO
 FILEVERSION $VER_MAJOR,$VER_MINOR,$VER_RELEASE,0
 PRODUCTVERSION $VER_MAJOR,$VER_MINOR,$VER_RELEASE,0
 FILEFLAGSMASK 0x3fL
 FILEFLAGS 0x0L
 FILEOS VOS_NT_WINDOWS32
 FILETYPE $( [ "$3" = exe ] && echo VFT_APP || echo VFT_DLL )
 FILESUBTYPE 0x0L
BEGIN
  BLOCK "StringFileInfo"
  BEGIN
    BLOCK "040904b0"
    BEGIN
      VALUE "CompanyName", "Regina Rexx OO (github.com/mikebeer/REGINA2)"
      VALUE "FileDescription", "$2"
      VALUE "FileVersion", "$VER_DOT.0"
      VALUE "InternalName", "$1"
      VALUE "LegalCopyright", "Regina: Anders Christensen, Mark Hessling and others. OO extension: Mike Beer. GNU LGPL."
      VALUE "OriginalFilename", "$1"
      VALUE "ProductName", "Regina Rexx Interpreter with OO extension"
      VALUE "ProductVersion", "$VER_DOT"
    END
  END
  BLOCK "VarFileInfo"
  BEGIN
    VALUE "Translation", 0x409, 1200
  END
END
EORC
  } >"$1.rc"
  $WINDRES -I"$SRC" -i "$1.rc" -o "$1.res.o"
}
write_manifest() {
  cat >app.manifest <<'EOM'
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
  <assemblyIdentity type="win32" name="Regina.Rexx.OO" version="1.0.0.0" processorArchitecture="amd64"/>
  <description>Regina Rexx Interpreter with OO extension</description>
  <trustInfo xmlns="urn:schemas-microsoft-com:asm.v3">
    <security><requestedPrivileges>
      <requestedExecutionLevel level="asInvoker" uiAccess="false"/>
    </requestedPrivileges></security>
  </trustInfo>
  <compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1">
    <application><supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}"/></application>
  </compatibility>
</assembly>
EOM
}

# __USE_MINGW_SETJMP_NON_SEH: longjmp (EXIT, errors, SIGNAL) only restores
# the registers, as in C. Otherwise 64-bit MinGW unwinds the stack with SEH,
# which hung the program at exit when the stack was 32 MB or more.
CFLAGS="-O2 -g -DNDEBUG -funsigned-char -DWIN32 -DWIN64 -DHAVE_PROTO -D__USE_MINGW_SETJMP_NON_SEH \
 -DREGINA_SINGLE_THREADED -DREGINA_BITS=64 -include stdint.h -include $OUT/version_defs.h \
 -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -w \
 -I$SRC -I$SRC/contrib"

SOURCES="funcs builtin error variable interprt debug dbgfuncs memory parsing
 files misc unxfuncs arxfuncs cmsfuncs os2funcs shell rexxext stack extstack
 tracing interp cmath convert strengs library strmath signals macros envir
 expr instore rexxbif yaccsrc lexsrc options os_win wrappers mt_notmt
 os_other mygetopt rexx nosaa oo dialect"

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
# the Python interpreter: its own flags (version_defs.h defines min/max)
$CC -c -O2 -g -DNDEBUG -D__USE_MINGW_SETJMP_NON_SEH -w -I"$SRC" -o pybridge.o "$SRC/pybridge.c"
OBJS="$OBJS pybridge.o"

write_manifest
make_rc rexx.exe "Regina Rexx interpreter (standalone)" exe
$CC -static -Wl,--stack,268435456 -o rexx.exe $OBJS rexx.exe.res.o -lwsock32 -ladvapi32 -lpthread
# unstripped copies for tools/symbolize-windows.sh (crash reports); not shipped
mkdir -p "$OUT/debug"; cp rexx.exe "$OUT/debug/"
x86_64-w64-mingw32-strip rexx.exe
echo "built $OUT/rexx.exe"

# regina.dll and regina.exe, which load external function packages
# (rgmqtt.dll), as the regina executable does on Linux. Multi-threaded
# (mt_win32), with the SAA interface (rexxsaa, client) instead of nosaa.
mkdir -p "$OUT/dll"; cd "$OUT/dll"
DLLFLAGS=$(echo "$CFLAGS" | sed 's/-DREGINA_SINGLE_THREADED//')
DLLFLAGS="$DLLFLAGS -DDYNAMIC -D__REGINA_DLL"
DOBJS=""
for f in $(echo "$SOURCES" | sed 's/\<nosaa\>//; s/\<mt_notmt\>//') mt_win32 client rexxsaa; do
  extra=""
  [ "$f" = yaccsrc ] && extra="-DYYMAXDEPTH=10000"
  [ "$f" = rexx ] && extra="-DRXLIB"       # __regina_faked_main, called by regina.exe
  $CC -c $DLLFLAGS $extra -o $f.o "$SRC/$f.c"
  DOBJS="$DOBJS $f.o"
done
{ echo "LIBRARY regina.dll"; echo "EXPORTS"; sed -n 's/^EXPORTS //p' "$SRC/regina_w32_mingw_dll.def"; } >regina.def
write_manifest
make_rc regina.dll "Regina Rexx interpreter library" dll
make_rc regina.exe "Regina Rexx interpreter (uses regina.dll)" exe
make_rc rgmqtt.dll "RgMqtt - MQTT client functions for Regina Rexx" dll
$CC -shared -static-libgcc -o regina.dll -Wl,--out-implib,libregina.a $DOBJS ../time64.o ../pybridge.o regina.dll.res.o \
    regina.def -Wl,-Bstatic -lpthread -Wl,-Bdynamic -lwsock32 -ladvapi32
# 256 MB of stack like rexx.exe; --stack=SIZE or REGINA_STACK asks for
# another size (the interpreter then runs on a thread of that size)
$CC -O2 -g -w -I"$SRC" -o regina.exe "$SRC/regina.c" regina.exe.res.o -L. -lregina -Wl,--stack,268435456
$CC -O2 -g -w -shared -static-libgcc -I"$SRC" -o rgmqtt.dll "$SRC/rgmqtt/rgmqtt.c" rgmqtt.dll.res.o \
    -Wl,--kill-at -L. -lregina -lws2_32
cp regina.dll regina.exe rgmqtt.dll "$OUT/debug/"
x86_64-w64-mingw32-strip regina.dll regina.exe rgmqtt.dll
cd "$OUT"
echo "built $OUT/dll/regina.exe, regina.dll, rgmqtt.dll"

# distribution zip: rexx.exe, message files, demos, licence
DIST="regina-rexx-oo-$VER_DOT-win64"
rm -rf "$DIST" "$DIST.zip"; mkdir -p "$DIST/demo"
cp rexx.exe dll/regina.exe dll/regina.dll dll/rgmqtt.dll "$DIST/"
cp "$SRC"/*.mtb "$DIST/"
cp "$SRC"/demo/*.rexx "$DIST/demo/"
mkdir -p "$DIST/demo/oo"; cp "$SRC"/demo/oo/* "$DIST/demo/oo/"
mkdir -p "$DIST/demo/mqtt"; cp "$SRC"/demo/mqtt/* "$DIST/demo/mqtt/"
cp "$SRC/rgmqtt/README" "$DIST/README.rgmqtt.txt"; sed -i 's/$/\r/' "$DIST/README.rgmqtt.txt"
# the Python library, found next to rexx.exe
cp -R "$SRC/python/pylib" "$DIST/pylib"
for m in "$SRC"/doc/Regina-OO-Manual.*; do [ -f "$m" ] && cp "$m" "$DIST/"; done
cp "$SRC/COPYING-LIB" "$DIST/COPYING-LIB.txt"
cp "$SRC/README.OO.md" "$DIST/README.OO.md"
cat > "$DIST/README.txt" <<EOR
Regina Rexx $VER_DOT with the optional OO extension - 64-bit Windows

rexx.exe is a standalone interpreter; it needs no DLL and no installation.
Run a program with:   rexx.exe myprog.rexx

regina.exe is the same interpreter built on regina.dll. It can load
external function packages (DLLs), such as rgmqtt.dll, the MQTT client
included here (see README.rgmqtt.txt and demo\mqtt):

  regina.exe demo\mqtt\15_public_broker.rexx      uses test.mosquitto.org
  regina.exe demo\mqtt\13_mqtt.rexx localhost 1883

Keep regina.exe, regina.dll and the package DLLs in the same folder.

The programs are not signed. If a virus scanner empties (0 bytes) or blocks
regina.exe or another file right after unpacking, add this folder as a scan
exclusion (with company-managed protection, the administrator does this in
the policy) and unpack the zip again.

Deeply nested methods end with error 11 (control stack full). rexx.exe and
regina.exe have a 256 MB stack; to use another size, start with
--stack=SIZE (for example  regina.exe --stack=512M prog.rexx) or set
REGINA_STACK=512M.

Classic Rexx programs run unchanged. Object-oriented programs can use
classes and methods (::class, ::method, ::attribute, ::routine,
::requires), messages (obj~method, obj -> method, cascades ~~ and ->>),
collections (Array, Directory, Table, Queue, Stem, ...), DO ... OVER,
DO WITH INDEX/ITEM, USE ARG and stems passed by reference.
Python modules (statistics, json, numpy, pandas, ...) are used with
.Python~import; the Python library is the folder pylib next to rexx.exe.

  rexx.exe --oo prog.rexx      OO keywords on in every program
  rexx.exe --no-oo prog.rexx   OO extension off (classic Regina 3.9.7)

Manual: Regina-OO-Manual.pdf / .docx.  Samples: demo\oo\*.rexx

Error messages in other languages: set REGINA_LANG (e.g. de) and
REGINA_LANG_DIR to the folder containing the *.mtb files.

Source: https://github.com/mikebeer/REGINA2
EOR
sed -i 's/$/\r/' "$DIST/README.txt"
(command -v zip >/dev/null && zip -qr "$DIST.zip" "$DIST") || python3 -c "import shutil,sys; shutil.make_archive(sys.argv[1], 'zip', '.', sys.argv[1])" "$DIST"
echo "built $OUT/$DIST.zip"
# the unstripped binaries of this build, for tools/symbolize-windows.sh
rm -f "$DIST-debug.zip"
(command -v zip >/dev/null && zip -qr9 "$DIST-debug.zip" debug) || python3 -c "import shutil,sys; shutil.make_archive(sys.argv[1], 'zip', '.', 'debug')" "$DIST-debug"
echo "built $OUT/$DIST-debug.zip"
