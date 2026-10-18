#!/usr/bin/env python3
"""Copies lib/lisp.cls and lib/session.rexx into oo.c (lisp_library and
   session_program).

   python3 tools/embed-libraries.py           update oo.c
   python3 tools/embed-libraries.py --check   exit 1 if oo.c is out of date
"""
import re
import sys
from pathlib import Path

root = Path(__file__).resolve().parent.parent
oo = (root / "oo.c").read_text(encoding="latin-1")
new = oo
for fname, cname in (("lisp.cls", "lisp_library"), ("session.rexx", "session_program")):
    src = (root / "lib" / fname).read_text()
    body = "\n".join('   "' + l.replace("\\", "\\\\").replace('"', '\\"') + '\\n"'
                     for l in src.split("\n"))
    pat = re.compile(r"(static const char " + cname + r"\[\] =\n)(.*?)(;\n)", re.S)
    new = pat.sub(lambda m: m.group(1) + body + m.group(3), new, count=1)
if "--check" in sys.argv:
    sys.exit(0 if new == oo else 1)
(root / "oo.c").write_text(new, encoding="latin-1")
print("oo.c: libraries updated" if new != oo else "oo.c: up to date")
