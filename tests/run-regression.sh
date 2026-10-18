#!/bin/sh
# Regression check for the Regina OO work.
#
# Suites:
#   trip     the trip suite shipped with Regina (../trip)
#   classic  classic programs whose behaviour the OO syntax must not change
#   oo       the new OO syntax
#   samples  the sample programs of demo/oo
#   packages external function packages (tests/packages: the rgnet test
#            package and RgMqtt) run by the dynamic `regina` next to REXX;
#            skipped when there is no `regina`, libregina, cc or python3
#
# trip and classic are compared against output recorded from the unmodified
# Regina 3.9.7 build; oo against reviewed output of the OO build. All
# expected output lives in tests/expected/<suite>/.
#
#   tests/run-regression.sh /path/to/rexx                  compare all suites
#   tests/run-regression.sh /path/to/rexx --record SUITE   re-record one suite
#
# Exit status 0 = every output identical.
set -u
REXX=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
MODE=${2:-compare}
RECORD_SUITE=${3:-}
HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$(cd "$HERE/.." && pwd)
WORK=$(mktemp -d "${TMPDIR:-/tmp}/regina-regress.XXXXXX")
trap 'rm -rf "$WORK"' EXIT
SUITES="trip classic oo samples"

# the built-in lisp library in oo.c must match lib/lisp.cls
if ! python3 "$SRC/tools/embed-libraries.py" --check; then
  echo "oo.c is out of date: run tools/embed-libraries.py"; exit 1
fi

cp -R "$SRC/trip" "$WORK/trip"
cp -R "$HERE/classic" "$WORK/classic"
cp -R "$HERE/oo" "$WORK/oo"
cp -R "$SRC/demo/oo" "$WORK/samples"   # the sample programs of demo/oo
( cd "$WORK/trip" && for p in rc true std filter; do cc -o $p $p.c 2>/dev/null; done )
PATH="$WORK/trip:$PATH"; export PATH
# the Python library of the source tree (installed copies are in share/pylib)
PYLIB="$SRC/python/pylib"; export PYLIB

normalize() {
  sed -e "s#$WORK#@WORK@#g" \
      -e 's#^[0-9.]* seconds used$#@SECONDS@ seconds used#'
}

run_suite() {   # $1 = suite name
  mkdir -p "$WORK/out/$1"
  cd "$WORK/$1"
  for f in *.rexx; do
    # optional interpreter switches for this test in NAME.switches
    sw=""
    [ -f "${f%.rexx}.switches" ] && sw=$(cat "${f%.rexx}.switches")
    if [ -f "${f%.rexx}.session" ]; then
      # a workspace session: NAME.session is typed in, NAME.rexx is ignored
      timeout 60 "$REXX" $sw --ws <"${f%.rexx}.session" >"$WORK/raw" 2>&1
    else
      timeout 60 "$REXX" $sw "./$f" </dev/null >"$WORK/raw" 2>&1
    fi
    echo "rc=$?" >>"$WORK/raw"
    normalize <"$WORK/raw" >"$WORK/out/$1/${f%.rexx}.out"
  done
}
for suite in $SUITES; do run_suite $suite; done

# --- external function packages ------------------------------------------
BINDIR=$(dirname "$REXX")
skip_packages=""
if [ ! -x "$BINDIR/regina" ]; then skip_packages="no dynamic regina next to $REXX"
elif ! ls "$BINDIR"/libregina.* >/dev/null 2>&1; then skip_packages="no libregina in $BINDIR"
elif ! command -v cc >/dev/null 2>&1; then skip_packages="no C compiler"
elif ! command -v python3 >/dev/null 2>&1; then skip_packages="no python3"
fi
if [ -z "$skip_packages" ]; then
  cp -R "$HERE/packages" "$WORK/packages"
  mkdir -p "$WORK/lib" "$WORK/out/packages"
  case $(uname -s) in Darwin) SHFLAGS="-dynamiclib -undefined dynamic_lookup"; SO=dylib ;;
                      *) SHFLAGS="-shared -fPIC"; SO=so ;; esac
  # the test package; RgMqtt as built by make, or from its source
  cc $SHFLAGS -I"$SRC" -I"$BINDIR" -o "$WORK/lib/librgnet.$SO" "$HERE/packages/rgnet.c" \
     -L"$BINDIR" -lregina >"$WORK/cc.log" 2>&1 || skip_packages="rgnet does not compile"
  if [ -f "$BINDIR/librgmqtt.$SO" ]; then cp "$BINDIR/librgmqtt.$SO" "$WORK/lib/"
  else cc $SHFLAGS -I"$SRC" -I"$BINDIR" -o "$WORK/lib/librgmqtt.$SO" "$SRC/rgmqtt/rgmqtt.c" \
          -L"$BINDIR" -lregina >>"$WORK/cc.log" 2>&1 || skip_packages="rgmqtt does not compile"
  fi
fi
if [ -z "$skip_packages" ]; then
  LD_LIBRARY_PATH="$WORK/lib:$BINDIR${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"; export LD_LIBRARY_PATH
  DYLD_LIBRARY_PATH="$WORK/lib:$BINDIR"; export DYLD_LIBRARY_PATH
  cd "$WORK/packages"
  for f in *.rexx; do
    sw=""; port=""; pid=""
    [ -f "${f%.rexx}.switches" ] && sw=$(cat "${f%.rexx}.switches")
    if [ -f "${f%.rexx}.server" ]; then
      # NAME.server names a Python server that writes its port to a file
      rm -f "$WORK/port"
      python3 "./$(cat "${f%.rexx}.server")" "$WORK/port" &
      pid=$!
      i=0; while [ ! -s "$WORK/port" ] && [ $i -lt 100 ]; do sleep 0.1; i=$((i+1)); done
      port=$(cat "$WORK/port" 2>/dev/null)
    fi
    timeout 60 "$BINDIR/regina" $sw "./$f" $port </dev/null >"$WORK/raw" 2>&1
    echo "rc=$?" >>"$WORK/raw"
    [ -n "$pid" ] && { kill $pid 2>/dev/null; wait $pid 2>/dev/null; }
    normalize <"$WORK/raw" >"$WORK/out/packages/${f%.rexx}.out"
  done
  SUITES="$SUITES packages"
fi

if [ "$MODE" = "--record" ]; then
  case " $SUITES " in
    *" $RECORD_SUITE "*) ;;
    *) echo "usage: $0 REXX --record {trip|classic|oo|samples|packages}"; exit 2 ;;
  esac
  rm -rf "$HERE/expected/$RECORD_SUITE"; mkdir -p "$HERE/expected"
  cp -R "$WORK/out/$RECORD_SUITE" "$HERE/expected/"
  echo "recorded expected output for suite $RECORD_SUITE"
  exit 0
fi

status=0
for suite in $SUITES; do
  fail=0; total=0
  for exp in "$HERE/expected/$suite"/*.out; do
    [ -f "$exp" ] || continue
    total=$((total+1))
    got="$WORK/out/$suite/$(basename "$exp")"
    if ! cmp -s "$exp" "$got"; then
      fail=$((fail+1)); echo "FAIL $suite/$(basename "$exp" .out)"
      diff "$exp" "$got" | head -20
    fi
  done
  # a test program without recorded output is a failure, too
  for got in "$WORK/out/$suite"/*.out; do
    [ -f "$got" ] || continue
    [ -f "$HERE/expected/$suite/$(basename "$got")" ] && continue
    fail=$((fail+1)); total=$((total+1))
    echo "FAIL $suite/$(basename "$got" .out): no expected output recorded"
  done
  echo "$suite: $((total-fail))/$total identical"
  [ $fail -eq 0 ] || status=1
done
[ -n "$skip_packages" ] && echo "packages: skipped ($skip_packages)"
exit $status
