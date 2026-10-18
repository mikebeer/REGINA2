#!/bin/sh
# Turns the lines of a Windows crash report ("at regina.dll+0x1234",
# "called from rexx.exe+0x5678") into function names and source lines.
#
#   tools/symbolize-windows.sh DEBUGDIR < report.txt
#
# DEBUGDIR holds the unstripped binaries of that build (OUTDIR/debug of
# tools/build-windows.sh); they must come from the same build as the zip.
DBG=${1:?usage: $0 DEBUGDIR < report}
OBJDUMP=${OBJDUMP:-x86_64-w64-mingw32-objdump}
ADDR2LINE=${ADDR2LINE:-x86_64-w64-mingw32-addr2line}
grep -o '[A-Za-z0-9_.-]*\.\(exe\|dll\)+0x[0-9a-fA-F]*' | while IFS=+ read -r mod off; do
  f="$DBG/$mod"
  if [ ! -f "$f" ]; then echo "$mod+$off: (no debug copy)"; continue; fi
  base=$($OBJDUMP -p "$f" | awk '/^ImageBase/ {print $2}')
  addr=$(printf '0x%x' $((0x$base + off - 1)))   # -1: inside the call, not after it
  echo "$mod+$off: $($ADDR2LINE -f -i -C -e "$f" "$addr" | paste -sd' ' -)"
done
