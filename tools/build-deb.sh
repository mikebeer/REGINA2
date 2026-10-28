#!/bin/sh
# Build a Debian/Ubuntu package of this Regina tree.
#
#   tools/build-deb.sh [OUTDIR]        (default: packages)
#
# The package is called regina-rexx-oo and replaces the distribution's
# regina-rexx / libregina3 packages (same commands, same library).
set -e
SRC=$(cd "$(dirname "$0")/.." && pwd)
OUT=${1:-$SRC/packages}
mkdir -p "$OUT"; OUT=$(cd "$OUT" && pwd)
. "$SRC/regina.ver"
PKGVER=${PKGVER:-$VER_DOT+oo1}
ARCH=$(dpkg --print-architecture)
WORK=$(mktemp -d "${TMPDIR:-/tmp}/regina-deb.XXXXXX")
trap 'rm -rf "$WORK"' EXIT

mkdir "$WORK/build" "$WORK/root"
cd "$WORK/build"
"$SRC/configure" --prefix=/usr --sysconfdir=/etc >configure.log 2>&1
make >make.log 2>&1              # serial: the Makefile races under -j
make install DESTDIR="$WORK/root" >install.log 2>&1

# the regression suites must pass before anything is packaged
"$SRC/tests/run-regression.sh" "$WORK/build/rexx"

# library symlinks (upstream "make install" only installs libregina.so.X.Y)
for so in "$WORK"/root/usr/lib/*/libregina.so.*.* "$WORK"/root/usr/lib/libregina.so.*.*; do
  [ -f "$so" ] || continue
  d=$(dirname "$so"); f=$(basename "$so")
  soname=$(objdump -p "$so" | sed -n 's/^ *SONAME *//p')
  [ -n "$soname" ] && [ "$soname" != "$f" ] && ln -sf "$f" "$d/$soname"
  ln -sf "${soname:-$f}" "$d/libregina.so"
done

mkdir -p "$WORK/root/DEBIAN" "$WORK/root/usr/share/doc/regina-rexx-oo"
cp "$SRC/COPYING-LIB" "$WORK/root/usr/share/doc/regina-rexx-oo/copyright"
cp "$SRC/README.OO.md" "$WORK/root/usr/share/doc/regina-rexx-oo/README.OO.md"
cp "$SRC/rgmqtt/README" "$WORK/root/usr/share/doc/regina-rexx-oo/README.rgmqtt"
mkdir -p "$WORK/root/usr/share/doc/regina-rexx-oo/examples"
cp "$SRC"/demo/oo_demo.rexx "$SRC"/demo/oo/* "$WORK/root/usr/share/doc/regina-rexx-oo/examples/"
mkdir -p "$WORK/root/usr/share/doc/regina-rexx-oo/examples/mqtt"
cp "$SRC"/demo/mqtt/* "$WORK/root/usr/share/doc/regina-rexx-oo/examples/mqtt/"
for m in "$SRC"/doc/ooRegina-Manual.*; do
  [ -f "$m" ] && cp "$m" "$WORK/root/usr/share/doc/regina-rexx-oo/"
done
[ -f "$WORK/root/etc/rxstack.conf" ] && echo /etc/rxstack.conf >"$WORK/root/DEBIAN/conffiles"
SIZE=$(du -sk "$WORK/root" | cut -f1)
DEPS=$(cd "$WORK/root" && dpkg-shlibdeps -O --ignore-missing-info \
        -l"$WORK/root/usr/lib" usr/bin/rexx usr/bin/regina 2>/dev/null \
        | sed -n 's/^shlibs:Depends=//p')
cat >"$WORK/root/DEBIAN/control" <<EOC
Package: regina-rexx-oo
Version: $PKGVER
Architecture: $ARCH
Maintainer: Mike Beer <mike@mindcoa.ch>
Installed-Size: $SIZE
Depends: ${DEPS:-libc6}
Conflicts: regina-rexx, libregina3
Replaces: regina-rexx, libregina3
Provides: regina-rexx, libregina3
Section: interpreters
Priority: optional
Homepage: https://github.com/mikebeer/REGINA2
Description: ooRegina - the Regina Rexx interpreter with optional OO syntax
 ooRegina is Regina $VER_DOT with an optional object-oriented extension:
 classes, methods, attributes and routines (:: directives), message
 operators ~ and ->, cascades ~~ and ->>, collections, DO OVER and DO WITH,
 USE ARG and stems passed by reference, and the notation of NetRexx and
 CREXX. Classic Rexx programs run unchanged; --no-oo switches the
 extension off.
 Includes RgMqtt, an MQTT 3.1.1 client function package.
EOC
printf '#!/bin/sh\nset -e\n[ "$1" = configure ] && ldconfig || true\n' >"$WORK/root/DEBIAN/postinst"
printf '#!/bin/sh\nset -e\n[ "$1" = remove ] && ldconfig || true\n' >"$WORK/root/DEBIAN/postrm"
chmod 755 "$WORK/root/DEBIAN/postinst" "$WORK/root/DEBIAN/postrm"

DEB="$OUT/regina-rexx-oo_${PKGVER}_${ARCH}.deb"
dpkg-deb --root-owner-group --build "$WORK/root" "$DEB" >/dev/null
echo "built $DEB"
