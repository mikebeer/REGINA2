#!/usr/bin/env python3
"""A one-shot TCP echo server for the rgnet test: python3 echoserver.py PORTFILE"""
import os
import socket
import sys

s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(("127.0.0.1", 0))
s.listen(1)
with open(sys.argv[1] + ".tmp", "w") as f:
    f.write(str(s.getsockname()[1]))
os.replace(sys.argv[1] + ".tmp", sys.argv[1])
s.settimeout(30)
c, _ = s.accept()
d = c.recv(1000)
c.sendall(b"echo: " + d)
c.close()
