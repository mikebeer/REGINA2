#!/usr/bin/env python3
"""Types a program into REXX on a terminal and ends it with Ctrl-D, as a
user does after starting "rexx" without a program. Prints "ok" if the
program ran and REXX ended, else what happened.

   tests/console-check.py /path/to/rexx [switches...]"""
import os
import pty
import select
import sys
import time

pid, fd = pty.fork()
if pid == 0:
    os.execv(sys.argv[1], sys.argv[1:])
out, done = b"", False
time.sleep(0.3)
for line in (b'say "typed" 6*7\n', b"exit 0\n"):
    os.write(fd, line)
    time.sleep(0.2)
os.write(fd, b"\x04")
end = time.time() + 10
while time.time() < end:
    r, _, _ = select.select([fd], [], [], 0.1)
    if r:
        try:
            d = os.read(fd, 4096)
        except OSError:
            d = b""
        if not d:
            done = True
            break
        out += d
if not done:
    os.kill(pid, 9)
ok = done and b"typed 42" in out
print("ok" if ok else "FAILED: " + ("ended" if done else "hangs") + ", output "
      + repr(out.decode("latin-1").replace("\r", "")))
sys.exit(0 if ok else 1)
