#!/bin/sh
# Cross-compile a standalone macOS rexx (universal: Apple Silicon + Intel)
# with zig cc, on Linux or on a Mac.
#
#   tools/build-macos.sh [OUTDIR]            (default: build-macos)
#
# Needs zig (for example "pip install ziglang", then ZIG="python3 -m ziglang")
# and llvm-lipo or lipo to join the two architectures.
# Like the Windows build, it leaves out readline, ncurses, iconv and GCI.
#
# On a Mac you can also build the usual way: ./configure && make
set -e
SRC=$(cd "$(dirname "$0")/.." && pwd)
OUT=${1:-build-macos}
mkdir -p "$OUT"; OUT=$(cd "$OUT" && pwd)
ZIG=${ZIG:-zig}
if ! $ZIG version >/dev/null 2>&1; then
  if python3 -m ziglang version >/dev/null 2>&1; then ZIG="python3 -m ziglang"
  else echo "zig not found (set ZIG=...)"; exit 1; fi
fi
LIPO=${LIPO:-$(command -v lipo || command -v llvm-lipo || command -v llvm-lipo-18 || true)}
MACOS_MIN=${MACOS_MIN:-11.0}

. "$SRC/regina.ver"

cat > "$OUT/config.h" <<'EOC'
/* config.h for macOS (cross build, see tools/build-macos.sh) */
#define UNIX 1
#define STDC_HEADERS 1
#define TIME_WITH_SYS_TIME 1
#define HAVE_CRYPT 1
#define HAVE_ATOLL 1
#define HAVE_DIV 1
#define HAVE_FSEEKO 1
#define HAVE_FTELLO 1
#define HAVE_FTRUNCATE 1
#define HAVE_GETOPT_LONG 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_INET_NTOP 1
#define HAVE_GMTIME_R 1
#define HAVE_LOCALTIME_R 1
#define HAVE_LSTAT 1
#define HAVE_MEMCPY 1
#define HAVE_MMAP 1
#define HAVE_MEMMOVE 1
#define HAVE_POLL 1
#define HAVE_PUTENV 1
#define HAVE_MKSTEMP 1
#define HAVE_MBSTOWCS 1
#define HAVE_RANDOM 1
#define HAVE_RAISE 1
#define HAVE_READLINK 1
#define HAVE_REALPATH 1
#define HAVE_STRERROR 1
#define HAVE_SYSCONF 1
#define HAVE_USLEEP 1
#define HAVE_VSPRINTF 1
#define HAVE_SIGACTION 1
#define HAVE_SOCKLEN_T 1
#define HAVE_ALLOCA_H 1
#define HAVE_ASSERT_H 1
#define HAVE_CTYPE_H 1
#define HAVE_DLFCN_H 1
#define HAVE_ERRNO_H 1
#define HAVE_FCNTL_H 1
#define HAVE_GRP_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_LIMITS_H 1
#define HAVE_MEMORY_H 1
#define HAVE_SYS_MMAN_H 1
#define HAVE_STDBOOL_H 1
#define HAVE_SYS_STATVFS_H 1
#define HAVE_SYS_RESOURCE_H 1
#define HAVE_SYS_MOUNT_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_IOCTL_H 1
#define HAVE_MATH_H 1
#define HAVE_PATHS_H 1
#define HAVE_POLL_H 1
#define HAVE_SYS_POLL_H 1
#define HAVE_PWD_H 1
#define HAVE_SETJMP_H 1
#define HAVE_SIGNAL_H 1
#define HAVE_STDARG_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STDINT_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_TIME_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_WAIT_H 1
#define HAVE_SYS_SOCKET_H 1
#define HAVE_NETINET_IN_H 1
#define HAVE_SYS_SELECT_H 1
#define HAVE_SYS_UTSNAME_H 1
#define HAVE_NETDB_H 1
#define HAVE_ARPA_INET_H 1
#define HAVE_BOOL 1
#define HAVE_GETGRGID_R_RETURNS_INT_5_PARAMS 1
#define HAVE_GETPWUID_R_RETURNS_INT 1
#define HAVE_BUILTIN_RETURN_ADDRESS 1
#define HAVE_REGINA_ADDON_DIR "/usr/local/lib/regina-rexx/addons"
#define REGINA_SHARE_DIRECTORY "/usr/local/share/regina-rexx"
#define REGINA_BITS 64
EOC
cat >> "$OUT/config.h" <<EOV
#define REGINA_VERSION_DATE "$VER_DATE"
#define REGINA_VERSION_MAJOR "$VER_MAJOR"
#define REGINA_VERSION_MINOR "$VER_MINOR"
#define REGINA_VERSION_RELEASE "$VER_RELEASE"
#define REGINA_VERSION_SUPP "$VER_SUPP"
EOV

SOURCES="funcs builtin error variable interprt debug dbgfuncs memory parsing
 files misc unxfuncs cmsfuncs os2funcs shell rexxext stack tracing interp
 cmath convert strengs library strmath signals macros envir expr extstack
 yaccsrc lexsrc options os_unx rexxbif wrappers mt_notmt instore oo arxfuncs
 rexx nosaa"

DEFS="-DNDEBUG -funsigned-char -DHAVE_CONFIG_H"

build_arch() {   # $1 = zig target, $2 = output name
  dir="$OUT/$2"; rm -rf "$dir"; mkdir -p "$dir"
  objs=""
  for f in $SOURCES; do
    extra=""
    [ "$f" = yaccsrc ] && extra="-DYYMAXDEPTH=10000"
    $ZIG cc -target $1.$MACOS_MIN -O2 -w $DEFS $extra \
      -I"$OUT" -I"$SRC" -I"$SRC/contrib" -c -o "$dir/$f.o" "$SRC/$f.c"
    objs="$objs $dir/$f.o"
  done
  # a large main stack, as for Windows, for deeply recursive Rexx code
  $ZIG cc -target $1.$MACOS_MIN -o "$dir/rexx" $objs \
    -Wl,-stack_size,0x10000000
}

build_arch aarch64-macos arm64
build_arch x86_64-macos x86_64
if [ -n "$LIPO" ]; then
  $LIPO -create -output "$OUT/rexx" "$OUT/arm64/rexx" "$OUT/x86_64/rexx"
else
  echo "no lipo: shipping the two binaries separately"
  cp "$OUT/arm64/rexx" "$OUT/rexx-arm64"; cp "$OUT/x86_64/rexx" "$OUT/rexx-x86_64"
fi
echo "built $OUT/rexx"

# distribution archive: rexx, message files, demos, docs, licence
DIST="regina-rexx-oo-$VER_DOT-macos"
cd "$OUT"
rm -rf "$DIST" "$DIST.tar.gz"; mkdir -p "$DIST/demo"
cp rexx* "$DIST/" 2>/dev/null || true
rm -f "$DIST/rexx.o"
cp "$SRC"/*.mtb "$DIST/"
cp "$SRC"/demo/*.rexx "$DIST/demo/"
mkdir -p "$DIST/demo/oo"; cp "$SRC"/demo/oo/* "$DIST/demo/oo/"
for m in "$SRC"/doc/Regina-OO-Manual.*; do [ -f "$m" ] && cp "$m" "$DIST/"; done
cp "$SRC/README.OO.md" "$DIST/"
cp "$SRC/COPYING-LIB" "$DIST/COPYING-LIB.txt"
cat > "$DIST/INSTALL-macOS.txt" <<'EOI'
Regina Rexx 3.9.7 with the OO extension, for macOS 11 or later
(one universal binary for Apple Silicon and Intel Macs).

1. Unpack the archive, then in Terminal, in the unpacked folder:

     xattr -dr com.apple.quarantine .     # files from the internet are blocked
     ./rexx demo/oo_demo.rexx             # try it

   The binary is not notarized by Apple. Without the xattr command,
   macOS says it "cannot be opened"; you can also allow it once in
   System Settings > Privacy & Security.

2. To install it for all users:

     sudo mkdir -p /usr/local/bin /usr/local/share/regina-rexx
     sudo cp rexx /usr/local/bin/
     sudo cp *.mtb /usr/local/share/regina-rexx/

See README.OO.md for the OO extension, including the --oo and --no-oo
switches. Readline (line editing in interactive trace) is not included.
EOI
tar czf "$DIST.tar.gz" "$DIST"
echo "built $OUT/$DIST.tar.gz"
