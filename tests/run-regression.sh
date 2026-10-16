#!/bin/sh
# Regression check: run the trip suite and tests/classic with a given
# rexx binary and compare against the outputs recorded from the
# unmodified Regina 3.9.7 build (tests/expected/).
#
#   tests/run-regression.sh /path/to/rexx            compare
#   tests/run-regression.sh /path/to/rexx --record   (re)record expected output
#
# Exit status 0 = every output identical.
set -u
REXX=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
MODE=${2:-compare}
HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$(cd "$HERE/.." && pwd)
WORK=$(mktemp -d "${TMPDIR:-/tmp}/regina-regress.XXXXXX")
trap 'rm -rf "$WORK"' EXIT

cp -R "$SRC/trip" "$WORK/trip"
cp -R "$HERE/classic" "$WORK/classic"
( cd "$WORK/trip" && for p in rc true std filter; do cc -o $p $p.c 2>/dev/null; done )
PATH="$WORK/trip:$PATH"; export PATH

normalize() {
  sed -e "s#$WORK#@WORK@#g" \
      -e 's#^[0-9.]* seconds used$#@SECONDS@ seconds used#'
}

run_dir() {   # $1 = suite name
  mkdir -p "$WORK/out/$1"
  cd "$WORK/$1"
  for f in *.rexx; do
    timeout 60 "$REXX" "./$f" </dev/null >"$WORK/raw" 2>&1
    echo "rc=$?" >>"$WORK/raw"
    normalize <"$WORK/raw" >"$WORK/out/$1/${f%.rexx}.out"
  done
}
run_dir trip
run_dir classic

if [ "$MODE" = "--record" ]; then
  rm -rf "$HERE/expected"; mkdir -p "$HERE/expected"
  cp -R "$WORK/out/trip" "$WORK/out/classic" "$HERE/expected/"
  echo "recorded expected output in $HERE/expected"
  exit 0
fi

fail=0; total=0
for suite in trip classic; do
  for exp in "$HERE/expected/$suite"/*.out; do
    total=$((total+1))
    got="$WORK/out/$suite/$(basename "$exp")"
    if ! cmp -s "$exp" "$got"; then
      fail=$((fail+1)); echo "FAIL $suite/$(basename "$exp" .out)"
      diff "$exp" "$got" | head -20
    fi
  done
done
echo "$((total-fail))/$total identical to Regina 3.9.7 baseline"
[ $fail -eq 0 ]
