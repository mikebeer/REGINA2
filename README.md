# ooRegina

ooRegina is Regina Rexx 3.9.7 with an optional **object-oriented
extension**: classes, methods, attributes, messages (`~`, `->` and the
period of NetRexx and CREXX), collections, `DO … OVER`, `USE ARG`, stems
passed by reference and more. Classic Regina programs run unchanged.

```rexx
acct = .account~new(100)
acct~deposit(50)
say acct~balance                  /* 150 */
exit

::class Account
::attribute balance get
::method init
  expose balance
  use arg balance
::method deposit
  expose balance
  use arg amount
  balance = balance + amount
```

Blocks in the style of Smalltalk work with all collections:

```rexx
big = numbers~select({ :x | x > 100 })
sum = numbers~inject(0, { :s :x | s + x })
```

Python libraries can be used from Rexx through a built-in Python
interpreter (from [mikebeer/python](https://github.com/mikebeer/python)):

```rexx
stats = .python~import('statistics')
say stats~mean(.array~of(1, 2, 3, 4))      /* 2.5 */
```

External function packages (RxSock, Rexx/SQL, Rexx/CURL, RegUtil, ...)
work as before with the `regina` executable (on Windows `regina.exe` with
`regina.dll`). New is **RgMqtt**, an MQTT 3.1.1 client package
([rgmqtt/README](rgmqtt/README), samples in [demo/mqtt](demo/mqtt), one of
them using the public broker test.mosquitto.org), with an Mqtt class for
OO programs:

```rexx
c = .mqtt~connect('localhost')
c~subscribe('sensors/#')
c~listen({ use arg m; say m~topic '=' m~message }, 10)
::requires 'mqtt'
```

The notation of **NetRexx and CREXX** works, too: `obj.m(args)`,
`.Class(args)`, `this`, `null`, `x is .T`, `x as .T` in every OO program,
and whole NetRexx programs (`options netrexx` or `*.nrx`) and CREXX
programs (`options levelg`) with their classes, interfaces and
factories (manual, Chapter 10):

```rexx
options netrexx
p = Point(3, 4)
say p.norm2()                     /* 25 */

class Point
  properties
    x = int; y = int
  method Point(ax = int, ay = int)
    x = ax; y = ay
  method norm2
    return x * x + y * y
```

## Downloads

| System | File |
|---|---|
| Linux (Debian, Ubuntu; x86-64) | [regina-rexx-oo_3.9.7+oo1_amd64.deb](dist/regina-rexx-oo_3.9.7+oo1_amd64.deb) |
| Windows (64-bit) | [regina-rexx-oo-3.9.7-win64.zip](dist/regina-rexx-oo-3.9.7-win64.zip) |
| macOS 11+ (Apple Silicon and Intel) | [regina-rexx-oo-3.9.7-macos.tar.gz](dist/regina-rexx-oo-3.9.7-macos.tar.gz) |

Checksums are in [dist/SHA256SUMS](dist/SHA256SUMS). The macOS build is
cross-compiled and has not yet been run on a Mac; the Windows build is
tested under Wine and was checked on Windows 11. See chapter 9 of the
manual.

## Documentation

- **Manual:** [ooRegina-Manual.pdf](doc/ooRegina-Manual.pdf)
  ([Word](doc/ooRegina-Manual.docx)) covers installation (binary and from
  source), the differences from classic Regina, a comparison with ooRexx,
  NetRexx and CREXX, the
  reference of the new instructions, directives and classes, sample
  programs and the restrictions.
- **Short guide:** [README.OO.md](README.OO.md)
- **Samples:** [demo/oo](demo/oo)

## Building

```sh
./configure && make            # not make -j
tests/run-regression.sh ./rexx
```

`tools/build-deb.sh`, `tools/build-windows.sh` and `tools/build-macos.sh`
build the packages; `node doc/manual/build-manual.js` and
`python3 doc/manual/finish.py` rebuild the manual.

Regina Rexx is distributed under the GNU Library General Public License
(see [COPYING-LIB](COPYING-LIB)); so is this extension.
