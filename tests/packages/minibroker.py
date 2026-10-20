#!/usr/bin/env python3
"""A small MQTT 3.1.1 broker for the RgMqtt tests (standard library only).

   python3 minibroker.py PORTFILE

Listens on a free port of 127.0.0.1 and writes the port number to PORTFILE.
Supports CONNECT (user "bad" is refused with code 4), SUBSCRIBE and
UNSUBSCRIBE with + and # wildcards, PUBLISH with QoS 0, 1 and 2 in both
directions, retained messages, wills, PINGREQ and DISCONNECT. A message to
the topic "$broker/kick" closes the sender's connection without a
DISCONNECT, so its will is published. Stops after 60 seconds.
"""
import socket
import sys
import threading
import time

lock = threading.Lock()
clients = {}        # socket -> Client
retained = {}       # topic -> (payload, qos)


def matches(flt, topic):
    f, t = flt.split("/"), topic.split("/")
    for i, part in enumerate(f):
        if part == "#":
            return True
        if i >= len(t):
            return False
        if part != "+" and part != t[i]:
            return False
    return len(f) == len(t)


def enc_len(n):
    out = bytearray()
    while True:
        d = n % 128
        n //= 128
        out.append(d | (0x80 if n else 0))
        if not n:
            return bytes(out)


def s16(b):
    return len(b).to_bytes(2, "big") + b


class Client:
    def __init__(self, sock):
        self.sock = sock
        self.subs = {}
        self.will = None
        self.next_id = 0
        self.wlock = threading.Lock()

    def send(self, hdr, body=b""):
        with self.wlock:
            self.sock.sendall(bytes([hdr]) + enc_len(len(body)) + body)

    def deliver(self, topic, payload, qos, retain=False):
        hdr = 0x30 | (qos << 1) | (1 if retain else 0)
        body = s16(topic.encode())
        if qos:
            self.next_id = self.next_id % 65535 + 1
            body += self.next_id.to_bytes(2, "big")
        self.send(hdr, body + payload)


def publish(topic, payload, qos, retain):
    if retain:
        with lock:
            if payload:
                retained[topic] = (payload, qos)
            else:
                retained.pop(topic, None)
    with lock:
        targets = [(c, max(q for f, q in c.subs.items() if matches(f, topic)))
                   for c in clients.values() if any(matches(f, topic) for f in c.subs)]
    for c, q in targets:
        try:
            c.deliver(topic, payload, min(qos, q))
        except OSError:
            pass


def recv_exact(sock, n):
    data = b""
    while len(data) < n:
        k = sock.recv(n - len(data))
        if not k:
            raise EOFError
        data += k
    return data


def read_packet(sock):
    hdr = recv_exact(sock, 1)[0]
    n, mult = 0, 1
    while True:
        d = recv_exact(sock, 1)[0]
        n += (d & 0x7F) * mult
        mult *= 128
        if not d & 0x80:
            break
    return hdr, recv_exact(sock, n)


def handle(sock):
    c = Client(sock)
    clean_exit = False
    try:
        hdr, body = read_packet(sock)
        if hdr >> 4 != 1 or body[2:6] != b"MQTT":
            return
        flags = body[7]
        p = 10
        n = int.from_bytes(body[p:p + 2], "big"); p += 2 + n      # client id
        if flags & 0x04:
            n = int.from_bytes(body[p:p + 2], "big"); wt = body[p + 2:p + 2 + n].decode(); p += 2 + n
            n = int.from_bytes(body[p:p + 2], "big"); wm = body[p + 2:p + 2 + n]; p += 2 + n
            c.will = (wt, wm, (flags >> 3) & 3, bool(flags & 0x20))
        user = None
        if flags & 0x80:
            n = int.from_bytes(body[p:p + 2], "big"); user = body[p + 2:p + 2 + n].decode(); p += 2 + n
        if user == "bad":
            c.send(0x20, b"\x00\x04")
            return
        c.send(0x20, b"\x00\x00")
        with lock:
            clients[sock] = c
        while True:
            hdr, body = read_packet(sock)
            kind = hdr >> 4
            if kind == 3:                                   # PUBLISH
                qos = (hdr >> 1) & 3
                n = int.from_bytes(body[:2], "big")
                topic = body[2:2 + n].decode()
                p = 2 + n
                pid = body[p:p + 2]
                if qos:
                    p += 2
                if topic == "$broker/kick":
                    return
                publish(topic, body[p:], qos, bool(hdr & 1))
                if qos == 1:
                    c.send(0x40, pid)
                elif qos == 2:
                    c.send(0x50, pid)
            elif kind == 6:                                 # PUBREL
                c.send(0x70, body[:2])
            elif kind in (4, 5, 7):                         # acks from the client
                if kind == 5:
                    c.send(0x62, body[:2])
            elif kind == 8:                                 # SUBSCRIBE
                pid, p, granted, new = body[:2], 2, b"", []
                while p < len(body):
                    n = int.from_bytes(body[p:p + 2], "big")
                    flt = body[p + 2:p + 2 + n].decode()
                    q = body[p + 2 + n]
                    p += 3 + n
                    if flt.startswith("$denied"):
                        granted += b"\x80"
                        continue
                    with lock:
                        c.subs[flt] = q
                    granted += bytes([q])
                    new.append((flt, q))
                c.send(0x90, pid + granted)
                with lock:
                    keep = list(retained.items())
                for flt, q in new:
                    for t, (pl, rq) in keep:
                        if matches(flt, t):
                            c.deliver(t, pl, min(q, rq), True)
            elif kind == 10:                                # UNSUBSCRIBE
                p = 2
                while p < len(body):
                    n = int.from_bytes(body[p:p + 2], "big")
                    with lock:
                        c.subs.pop(body[p + 2:p + 2 + n].decode(), None)
                    p += 2 + n
                c.send(0xB0, body[:2])
            elif kind == 12:                                # PINGREQ
                c.send(0xD0)
            elif kind == 14:                                # DISCONNECT
                clean_exit = True
                return
    except (EOFError, OSError, IndexError):
        pass
    finally:
        with lock:
            clients.pop(sock, None)
        try:
            sock.close()
        except OSError:
            pass
        if c.will and not clean_exit:
            publish(*c.will)


def main():
    srv = socket.socket()
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("127.0.0.1", 0))
    srv.listen(16)
    with open(sys.argv[1] + ".tmp", "w") as f:
        f.write(str(srv.getsockname()[1]))
    import os
    os.replace(sys.argv[1] + ".tmp", sys.argv[1])
    srv.settimeout(1)
    end = time.time() + 60
    while time.time() < end:
        try:
            s, _ = srv.accept()
        except socket.timeout:
            continue
        s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        threading.Thread(target=handle, args=(s,), daemon=True).start()


main()
