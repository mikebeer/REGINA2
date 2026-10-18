#!/bin/sh
# Regression check for the Regina OO work.
#
# Suites:
#   trip     the trip suite shipped with Regina (../trip)
#   classic  classic programs whose behaviour the OO syntax must not change
#   oo       the new OO syntax
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

if [ "$MODE" = "--record" ]; then
  case " $SUITES " in
    *" $RECORD_SUITE "*) ;;
    *) echo "usage: $0 REXX --record {trip|classic|oo|samples}"; exit 2 ;;
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
    total=$((total+1))
    got="$WORK/out/$suite/$(basename "$exp")"
    if ! cmp -s "$exp" "$got"; then
      fail=$((fail+1)); echo "FAIL $suite/$(basename "$exp" .out)"
      diff "$exp" "$got" | head -20
    fi
  done
  # a test program without recorded output is a failure, too
  for got in "$WORK/out/$suite"/*.out; do
    [ -f "$HERE/expected/$suite/$(basename "$got")" ] && continue
    fail=$((fail+1)); total=$((total+1))
    echo "FAIL $suite/$(basename "$got" .out): no expected output recorded"
  done
  echo "$suite: $((total-fail))/$total identical"
  [ $fail -eq 0 ] || status=1
done
exit $status
