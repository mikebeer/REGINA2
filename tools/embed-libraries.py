#!/usr/bin/env python3
"""Copies lib/lisp.cls into oo.c as the built-in library (lisp_library).

   python3 tools/embed-libraries.py           update oo.c
   python3 tools/embed-libraries.py --check   exit 1 if oo.c is out of date
"""
import re
import sys
from pathlib import Path

root = Path(__file__).resolve().parent.parent
src = (root / "lib" / "lisp.cls").read_text()
body = "\n".join('   "' + l.replace("\\", "\\\\").replace('"', '\\"') + '\\n"'
                 for l in src.split("\n"))
oo = (root / "oo.c").read_text(encoding="latin-1")
pat = re.compile(r"(static const char lisp_library\[\] =\n)(.*?)(;\n)", re.S)
new = pat.sub(lambda m: m.group(1) + body + m.group(3), oo, count=1)
if "--check" in sys.argv:
    sys.exit(0 if new == oo else 1)
(root / "oo.c").write_text(new, encoding="latin-1")
print("oo.c: lisp_library updated" if new != oo else "oo.c: up to date")
