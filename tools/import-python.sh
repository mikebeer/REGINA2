#!/bin/sh
# Copies the Python interpreter and library of mikebeer/python into python/.
#
#   tools/import-python.sh /path/to/python-checkout
#
# See python/README.md.
set -e
UP=$(cd "${1:?usage: $0 /path/to/mikebeer/python}" && pwd)
SRC=$(cd "$(dirname "$0")/.." && pwd)
[ -f "$UP/c/py_all.c" ] && [ -d "$UP/pylib" ] || { echo "$UP is no mikebeer/python checkout"; exit 1; }
cp "$UP/c/py_all.c" "$SRC/python/py_all.c"
rm -rf "$SRC/python/pylib"
cp -R "$UP/pylib" "$SRC/python/pylib"
find "$SRC/python/pylib" -name __pycache__ -prune -exec rm -rf {} +
find "$SRC/python/pylib" -name '*.pyc' -exec rm -f {} +
# globals()[name] = value is not kept by the interpreter: use exec
sed -i.bak "s/^    globals()\[_nm\] = _wrap(_nm)\$/    exec(_nm + ' = _wrap(_nm)')   # globals()[...] = is not kept by py/" \
  "$SRC/python/pylib/matplotlib/pyplot.py"
sed -i.bak "s/^    globals()\[_n\] = getattr(HTTPStatus, _n)\$/    exec(_n + ' = getattr(HTTPStatus, _n)')   # globals()[...] = is not kept by py/" \
  "$SRC/python/pylib/http/client.py"
rm -f "$SRC/python/pylib/matplotlib/pyplot.py.bak" "$SRC/python/pylib/http/client.py.bak"
( cd "$UP" && git log -1 --format='%H %s' 2>/dev/null || echo unknown ) > "$SRC/python/UPSTREAM"
echo "imported $(cat "$SRC/python/UPSTREAM")"
