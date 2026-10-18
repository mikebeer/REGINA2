#!/usr/bin/env python3
"""Runs a Windows program under Wine with a console (a pseudo terminal) and
types lines into it, the way a user at cmd.exe would.

   tools/wine-console.py SECONDS LINES -- wine prog.exe args...

LINES is separated by "|"; "^Z" stands for Ctrl-Z. Each line is typed when
the program has been quiet for a moment. Prints the output (terminal codes
removed) and "EXITED" or "HANGS" (still running after SECONDS)."""
import os
import pty
import re
import select
import sys
import termios
import time

tmo = float(sys.argv[1])
lines = [l.replace("^Z", "\x1a") for l in sys.argv[2].split("|")] if sys.argv[2] else []
cmd = sys.argv[sys.argv.index("--") + 1:]
pid, fd = pty.fork()
if pid == 0:
    os.environ.setdefault("WINEDEBUG", "-all")
    os.execvp(cmd[0], cmd)
attr = termios.tcgetattr(fd)
attr[0] &= ~termios.ICRNL                  # Enter is a CR for the Wine console
termios.tcsetattr(fd, termios.TCSANOW, attr)
out, start, last, exited = b"", time.time(), time.time(), False
while time.time() - start < tmo:
    r, _, _ = select.select([fd], [], [], 0.3)
    if r:
        try:
            d = os.read(fd, 4096)
        except OSError:
            d = b""
        if not d:
            exited = True
            break
        out += d
        last = time.time()
    elif lines and time.time() - last > 1.5:
        os.write(fd, (lines.pop(0) + "\r").encode())
        last = time.time()
if not exited:
    os.kill(pid, 9)
text = re.sub(rb"\x1b\[[0-9;?]*[a-zA-Z]", b"", out).decode("latin-1").replace("\r", "")
print(text.strip())
print("EXITED" if exited else "HANGS")
