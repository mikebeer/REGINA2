#!/bin/bash
# Tests the Windows zip under Wine (wine64, python3).
#
#   tools/test-windows.sh [ZIP]     (default: dist/regina-rexx-oo-*-win64.zip)
#
# - the oo and samples suites with rexx.exe and with regina.exe;
# - the MQTT tests and samples with regina.exe and rgmqtt.dll, against
#   tests/packages/minibroker.py;
# - with a console (tools/wine-console.py): programs end after EXIT, after
#   an empty input line and with --stack, and rexx reads a program typed in
#   up to Ctrl-Z. Pipes alone don't show console problems.
# Exit status 0 = everything passed.
set -u
SRC=$(cd "$(dirname "$0")/.." && pwd)
ZIP=${1:-$(ls "$SRC"/dist/regina-rexx-oo-*-win64.zip | head -1)}
WORK=$(mktemp -d "${TMPDIR:-/tmp}/regina-wintest.XXXXXX")
trap 'rm -rf "$WORK"' EXIT
export WINEDEBUG=-all
(cd "$WORK" && unzip -q "$ZIP")
D=$(ls -d "$WORK"/regina-rexx-oo-*-win64)
export PYLIB=$(winepath -w "$SRC/python/pylib" 2>/dev/null)
status=0

suite() {   # exe dir expected-dir tag
  local exe=$1 dir=$2 exp=$3 tag=$4 pass=0 total=0
  cd "$dir"
  local W; W=$(winepath -w "$PWD" 2>/dev/null | sed 's/\\/\\\\/g')
  for f in *.rexx; do
    b=${f%.rexx}
    [ -f "$b.session" ] && continue
    [ -f "$exp/$b.out" ] || continue
    total=$((total+1)); sw=""; port=""; pid=""
    [ -f "$b.switches" ] && sw=$(cat "$b.switches")
    if [ -f "$b.server" ]; then
      rm -f "$WORK/port"
      python3 "$SRC/tests/packages/$(cat "$b.server")" "$WORK/port" & pid=$!
      i=0; while [ ! -s "$WORK/port" ] && [ $i -lt 100 ]; do sleep 0.1; i=$((i+1)); done
      port=$(cat "$WORK/port")
    fi
    timeout 120 wine "$D/$exe" $sw "./$f" ${port:+127.0.0.1 $port} </dev/null >"$b.raw" 2>&1; rc=$?
    [ -n "$pid" ] && { kill $pid 2>/dev/null; wait $pid 2>/dev/null; }
    tr -d '\r' <"$b.raw" | sed -e "s#$W\\\\#@WORK@/$tag/#g" \
        -e 's#^[0-9.]* seconds used$#@SECONDS@ seconds used#' >"$b.got"
    [ -n "$port" ] && sed -i "s/\([^0-9]\)$port\([^0-9]\|\$\)/\1@PORT@\2/g" "$b.got"
    echo "rc=$rc" >>"$b.got"
    if cmp -s "$b.got" "$exp/$b.out"; then pass=$((pass+1))
    else echo "FAIL $exe $tag/$b"; diff "$b.got" "$exp/$b.out" | head -6; fi
  done
  echo "$exe $tag: $pass/$total identical"
  [ $pass -eq $total ] || status=1
}

mkdir -p "$WORK/t"
for exe in rexx.exe regina.exe; do
  rm -rf "$WORK/t/oo" "$WORK/t/samples"
  cp -R "$SRC/tests/oo" "$WORK/t/oo"; cp -R "$SRC/demo/oo" "$WORK/t/samples"
  suite $exe "$WORK/t/oo" "$SRC/tests/expected/oo" oo
  suite $exe "$WORK/t/samples" "$SRC/tests/expected/samples" samples
done
mkdir -p "$WORK/t/packages"
cp "$SRC"/tests/packages/mqtt_*.rexx "$SRC"/tests/packages/mqtt_*.server "$SRC"/demo/mqtt/*.rexx "$WORK/t/packages/"
for f in "$SRC"/demo/mqtt/*.rexx; do echo minibroker.py >"$WORK/t/packages/$(basename "${f%.rexx}").server"; done
suite regina.exe "$WORK/t/packages" "$SRC/tests/expected/packages" packages

# with a console
printf "say 'hi'\nexit 0\n" >"$WORK/t/exit.rexx"
console() {   # name lines expected-text command...
  local name=$1 lines=$2 want=$3; shift 3
  out=$(cd "$D/demo" && python3 "$SRC/tools/wine-console.py" 40 "$lines" -- "$@")
  if echo "$out" | tail -1 | grep -q EXITED && echo "$out" | grep -q "$want"; then
    echo "console $name: ok"
  else
    echo "console $name: FAILED"; echo "$out" | tail -5; status=1
  fi
}
for exe in rexx.exe regina.exe; do
  console "$exe exit" "" "hi" wine "$D/$exe" "$WORK/t/exit.rexx"
  console "$exe --stack=512M" "" "hi" wine "$D/$exe" --stack=512M "$WORK/t/exit.rexx"
  console "$exe animal (empty answer)" "yes|no|" "save" wine "$D/$exe" animal.rexx
  console "$exe program from the console" 'say "abc"|^Z' "abc" wine "$D/$exe"
done
exit $status
