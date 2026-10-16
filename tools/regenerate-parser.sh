#!/bin/sh
# Regenerate lexsrc.c and yaccsrc.c/.h from lexsrc.l and yaccsrc.y.
# Uses a current flex (2.6.x, default skeleton) and bison (3.x).
# contrib/flex.skl only works with flex <= 2.5.4a and is not used here.
set -e
cd "$(dirname "$0")/.."
flex -P__regina -8 -olexsrc.c lexsrc.l
bison -p__regina -oyaccsrc.c -d yaccsrc.y
echo "regenerated lexsrc.c yaccsrc.c yaccsrc.h"
