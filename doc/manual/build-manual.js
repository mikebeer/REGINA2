// Builds doc/Regina-OO-Manual.docx:   node doc/manual/build-manual.js
// The sample chapter reads demo/oo/*.rexx and tests/expected/samples/*.out,
// so the manual always shows the shipped programs and their real output.
const fs = require('fs');
const path = require('path');
const S = require('./style.js');
const { d, P, H1, H2, H3, UL, OL, table, screen, caption, pageBreak, runs, ARIAL } = S;
const { Document, Packer, Paragraph, TextRun, Header, Footer, AlignmentType, TabStopType,
        PageNumber, TableOfContents, BorderStyle } = d;

const ROOT = path.resolve(__dirname, '..', '..');
const PUB = 'RXO-0001-0';
const TITLE = 'Regina Rexx OO Introduction, Installation and Reference';
const DATE = '17 October 2026';
const W = (...a) => a;                          // column widths helper
const count = (dir, ext) => fs.readdirSync(path.join(ROOT, dir)).filter((f) => f.endsWith(ext)).length;
const N = { trip: 23, classic: count('tests/classic', '.rexx'), oo: count('tests/oo', '.rexx'),
            samples: count('demo/oo', '.rexx') };

const read = (f) => fs.readFileSync(path.join(ROOT, f), 'utf8');
const output = (name) => read(`tests/expected/samples/${name}.out`)
  .split('\n').filter((l) => !/^rc=\d+$/.test(l)).join('\n');

// ------------------------------------------------------------------
// title page, edition notice, contents
// ------------------------------------------------------------------
const big = (t, size, bold = true, after = 0) => new Paragraph({ spacing: { after },
  children: [new TextRun({ text: t, font: ARIAL, size, bold })] });

const front = [
  new Paragraph({ spacing: { before: 3600 }, children: [] }),
  big('Regina Rexx OO', 56, true, 60),
  big('Regina Rexx 3.9.7 with Object-Oriented Extensions', 24, false, 600),
  big('Introduction, Installation', 30, true, 0),
  new Paragraph({ spacing: { after: 240 },
    border: { bottom: { style: BorderStyle.SINGLE, size: 18, color: '000000', space: 8 } },
    children: [new TextRun({ text: 'and Reference', font: ARIAL, size: 30, bold: true })] }),
  big(`Release 1  \u2022  updated ${DATE}`, 20, false, 60),
  big('based on Regina Rexx 3.9.7 (18 March 2025)', 18, false, 2400),
  big(`Publication ${PUB}`, 18, false, 40),
  pageBreak(),
  new Paragraph({ spacing: { after: 160 }, children: [new TextRun({ text: 'First Edition (October 2026)', font: ARIAL, bold: true, size: 22 })] }),
  P('This edition applies to Regina Rexx 3.9.7 with the object-oriented extension, Release 1, as published in the repository github.com/mikebeer/REGINA2, and to all later builds until otherwise indicated in new editions.'),
  P('Regina Rexx is an open-source REXX interpreter distributed under the GNU Library General Public License. The object-oriented extension described here is distributed under the same license. It is not part of an official Regina release.'),
  P('Open Object Rexx (ooRexx) is maintained by the Rexx Language Association. This extension follows the syntax of ooRexx where it can, but it is a separate implementation and is not affiliated with that project. IBM and Object REXX are names of IBM products, used here only to identify the language this work descends from.'),
  P('The repository holds the source, the build tools and the regression tests from which the statements in this manual can be checked. Comments on this publication may be addressed to the project.'),
  pageBreak(),
  new Paragraph({ spacing: { after: 240 }, children: [new TextRun({ text: 'Contents', font: ARIAL, bold: true, size: 32 })] }),
  new TableOfContents('Contents', { hyperlink: true, headingStyleRange: '1-2' }),
];

// ------------------------------------------------------------------
// Preface
// ------------------------------------------------------------------
const preface = [
  H1('Preface', false),
  P('This manual introduces the object-oriented extension of Regina Rexx. It tells you how to install the interpreter from a binary package or from source, how the extension differs from classic Regina and from Open Object Rexx, and it describes the new instructions, directives and classes. It ends with sample programs, the restrictions of this release, and the error messages.'),
  P('It is written for REXX programmers who know classic REXX. Some familiarity with object-oriented programming helps, but each concept is explained where it is first used.'),
  H3('How this manual is organized'),
  ...UL([
    '**Chapter 1, Introduction**, says what the extension is and how it keeps classic programs unchanged.',
    '**Chapter 2, Installing Regina Rexx OO**, covers the Linux, Windows and macOS packages and building from source.',
    '**Chapter 3, First Steps**, shows a first program and explains when the OO features are switched on.',
    '**Chapter 4, Differences from Classic Regina**, lists every construct whose meaning depends on the extension.',
    '**Chapter 5, Differences from Open Object Rexx**, lists what is supported, what behaves differently and what is missing.',
    '**Chapter 6, Language Reference**, describes messages, instructions, directives, methods and operators.',
    '**Chapter 7, Built-in Classes**, lists the classes and their methods.',
    '**Chapter 8, Sample Programs**, prints the sample programs that come with the packages, with their output.',
    '**Chapter 9, Restrictions and Known Problems**, lists the limitations of this release.',
    '**Appendix A** lists the new error messages, **Appendix B** the switches and environment variables, **Appendix C** the contents of the packages.',
  ]),
  H3('Conventions'),
  P('Program text, commands and file names are shown in `monospaced type`. Examples of programs and terminal sessions are shown in framed screens; lines you type and the lines the system prints are not marked separately. REXX keywords are shown in uppercase in the text, although REXX itself ignores case.'),
  H3('Summary of changes'),
  P('This edition is kept up to date as the extension grows; each change is listed here, newest first.'),
  ...UL([
    '**17 October 2026.** Packages for macOS (one universal binary for Apple Silicon and Intel). Eight sample programs in `demo/oo`. Objects on the right side of an operator use their string value.',
    '**17 October 2026.** Startup switches `--oo` and `--no-oo` and the environment variable `REGINA_OO`. The marker `OOREGINA` (comment or OPTIONS) switches the new keywords on.',
    '**17 October 2026.** Stems passed by reference with `USE ARG s.`, stems returned with `RETURN x.`, the Stem class.',
    '**17 October 2026.** `DO WITH INDEX ... ITEM ... OVER`, `FOR` with `OVER`, `DO COUNTER`, `DO LABEL` and `LOOP`.',
    '**16 October 2026.** The object system: classes, methods, attributes, routines, `::requires`, collections, operator methods, error messages 88 to 99. Packages for Linux and Windows.',
    '**15 October 2026.** Syntax only: message operators, directives and `[ ]` were parsed but not yet executed.',
  ]),
];

// ------------------------------------------------------------------
// Chapter 1
// ------------------------------------------------------------------
const ch1 = [
  H1('Chapter 1. Introduction'),
  P('Regina is a REXX interpreter for Linux, Windows, macOS and many other systems. Regina Rexx OO is Regina 3.9.7 with an optional object-oriented extension in the style of Open Object Rexx (ooRexx). With it, a REXX program can define classes and methods, create objects, send messages to them and use collections such as arrays and directories:'),
  ...screen(`acct = .account~new(100)          /* create an object       */
acct~deposit(50)                  /* send it a message      */
say acct~balance                  /* 150                    */
exit

::class Account                   /* the class definition   */
::attribute balance get
::method init
  expose balance
  use arg balance
::method deposit
  expose balance
  use arg amount
  balance = balance + amount`, 'A first object-oriented REXX program'),
  P('The extension has three goals:'),
  ...UL([
    '**Existing programs run unchanged.** A program that does not use the new features runs exactly as it does on Regina 3.9.7: same results, same error messages, same TRACE output. The test suite of Regina itself and a set of classic tests check this for every build.',
    '**Object orientation is an option.** The OO features switch on when a program uses them, or when it asks for them with a marker or a startup switch. A switch can also turn the whole extension off.',
    '**ooRexx syntax.** Where it can, the extension uses the syntax and the error numbers of ooRexx, so knowledge and many programs carry over in both directions.',
  ]),
  H2('How it works'),
  P('Every new construct was a syntax error in classic Regina, or is recognized only where a program asks for the extension. That is why classic programs cannot change meaning. For example, `a~b` (no blank before the tilde) was an error and is now a message, while `a ~b` (with a blank) still means "a concatenated with NOT b".'),
  P('Objects are stored in REXX variables as short internal "handle" strings that cannot be typed in a program. Assignment, stems, arguments and RETURN therefore copy references to objects without any change to the variable pool. Wherever a real string is needed, for example in SAY, in a concatenation or as an argument of a built-in function, the object\'s STRING method is called, so `say acct` prints what the class decides.'),
  P('The object system starts only when a program sends its first message, uses a directive, or is run with `--oo`. Until then the interpreter takes exactly the code paths of Regina 3.9.7.'),
  H2('What is included'),
  ...table(['Area', 'Contents'], [
    ['Messages', '`obj~name(args)` and `obj -> name(args)`, cascades `~~` and `->>`, `~name:super`, `obj~name = value`, `coll[i]` and `coll[i] = value`'],
    ['Directives', '`::class`, `::method`, `::attribute`, `::constant`, `::routine`, `::requires`, `::options`'],
    ['Instructions', '`USE ARG`, `EXPOSE`, `DO x OVER`, `DO WITH INDEX i ITEM x OVER`, `DO COUNTER`, `DO LABEL`, `LOOP`'],
    ['Classes', 'Object, Class, String, Array, Queue, List, Directory, StringTable, Table, Stem, MutableBuffer, Supplier; `.nil`, `.true`, `.false`, `.environment`, `.local`'],
    ['Methods', 'instance and class methods, attributes, constants, private and abstract methods, UNKNOWN, operator methods, mixins and multiple inheritance'],
    ['Stems', 'stems passed by reference with `USE ARG s.`, stems returned with `RETURN x.`'],
    ['Control', 'markers `/* OOREGINA */` and `OPTIONS OOREGINA`, switches `--oo` and `--no-oo`, variable `REGINA_OO`'],
    ['Errors', 'error numbers 88, 91, 97, 98 and 99 as in ooRexx, in all message languages'],
  ], W(1700, 7938), 'What the extension adds'),
  H2('How it was tested'),
  P('Each build runs four test suites with `tests/run-regression.sh`. The results of this release are:'),
  ...table(['Suite', 'What it checks', 'Result'], [
    ['trip', 'Regina\'s own test suite; output must be identical to Regina 3.9.7', '23 of 23 identical'],
    ['classic', 'classic programs, including programs that use the new words as variables; recorded with Regina 3.9.7', `${N.classic} of ${N.classic} identical`],
    ['oo', 'the extension: syntax, classes, collections, errors, loops, stems, switches', `${N.oo} of ${N.oo}`],
    ['samples', 'the sample programs of Chapter 8', `${N.samples} of ${N.samples}`],
  ], W(1300, 6338, 2000), 'Test suites'),
  P('The trip and classic suites are also identical to Regina 3.9.7 when the interpreter is run with `--no-oo`. The Linux build is checked with valgrind. The Windows build runs the oo suite under Wine. The macOS build is cross-compiled; see Chapter 9.'),
];

// ------------------------------------------------------------------
// Chapter 2
// ------------------------------------------------------------------
const ch2 = [
  H1('Chapter 2. Installing Regina Rexx OO'),
  H2('What you need'),
  ...table(['System', 'Package', 'Notes'], [
    ['Linux (Debian, Ubuntu; x86-64)', '`regina-rexx-oo_3.9.7+oo1_amd64.deb`', 'Replaces the distribution\'s `regina-rexx` package. Includes the library, `rexx`, `regina`, `rxstack`, `rxqueue`, man pages, samples and this manual.'],
    ['Windows (64-bit)', '`regina-rexx-oo-3.9.7-win64.zip`', 'A standalone `rexx.exe`; no DLL, no installation.'],
    ['macOS 11 or later', '`regina-rexx-oo-3.9.7-macos.tar.gz`', 'One universal `rexx` for Apple Silicon and Intel Macs.'],
    ['Any other system', 'the source', 'See "Building from source" below.'],
  ], W(2200, 3238, 4200), 'Installation packages'),
  P('The packages are in the `dist` folder of the repository, github.com/mikebeer/REGINA2, together with a file of SHA-256 checksums.'),
  H2('Linux'),
  P('Install the package with `dpkg`. If the distribution\'s Regina is installed, it is replaced; remove the package again to go back to it.'),
  ...screen(`sudo dpkg -i regina-rexx-oo_3.9.7+oo1_amd64.deb
rexx -v
rexx /usr/share/doc/regina-rexx-oo/examples/01_classes.rexx`, 'Installing on Debian or Ubuntu'),
  P('The message files are installed in `/usr/share/regina-rexx`, the samples in `/usr/share/doc/regina-rexx-oo/examples` and this manual in `/usr/share/doc/regina-rexx-oo`. To remove the package, use `sudo dpkg -r regina-rexx-oo`.'),
  H2('Windows'),
  ...OL([
    'Unpack the zip file into a folder of your choice, for example `C:\\Rexx`.',
    'Add the folder to the PATH, or call `rexx.exe` with its full path.',
    'Run a sample: `rexx demo\\oo\\01_classes.rexx`.',
  ], 's2'),
  P('`rexx.exe` contains the English error messages. For another language, set `REGINA_LANG` (for example `de`) and `REGINA_LANG_DIR` to the folder with the `*.mtb` files.'),
  H2('macOS'),
  P('Unpack the archive in Terminal. Files downloaded from the internet carry a quarantine mark, and the binary is not notarized by Apple, so remove the mark before the first run:'),
  ...screen(`tar xzf regina-rexx-oo-3.9.7-macos.tar.gz
cd regina-rexx-oo-3.9.7-macos
xattr -dr com.apple.quarantine .
./rexx demo/oo/01_classes.rexx`, 'Unpacking and running on macOS'),
  P('Without the `xattr` command, macOS reports that `rexx` "cannot be opened"; you can then also allow it once in System Settings, Privacy & Security. To install it for all users:'),
  ...screen(`sudo mkdir -p /usr/local/bin /usr/local/share/regina-rexx
sudo cp rexx /usr/local/bin/
sudo cp *.mtb /usr/local/share/regina-rexx/`, 'Installing on macOS'),
  H2('Checking the installation'),
  P('`rexx -v` prints the version. The version string is the same as that of Regina 3.9.7, so that classic programs which test it keep working. `rexx -h` lists the switches; the switches `--oo` and `--no-oo` show that the extension is present. The program `demo/oo_demo.rexx` checks classic and OO features and prints `ok` for each of them.'),
  ...screen(`rexx -v
REXX-Regina_3.9.7 5.00 18 Mar 2025 (64 bit)
rexx -h | grep oo
  --oo, -O              object-oriented keywords on in every program
  --no-oo, -N           object-oriented extension off (classic Regina)`, 'Checking the version and the switches'),
  P('A program can test for the extension itself:'),
  ...screen(`signal on syntax name classic
interpret 'ok = .nil~isNil'
say 'OO extension available'
exit
classic: say 'classic Regina, no OO'`, 'Testing for the extension in a program'),
  H2('Building from source'),
  P('You need a C compiler (gcc or clang), make and git. flex 2.6.4 and bison 3.8.2 are needed only if you change the grammar; the generated files are in the repository.'),
  ...screen(`git clone https://github.com/mikebeer/REGINA2.git
cd REGINA2
git checkout oo-syntax
./configure
make
tests/run-regression.sh ./rexx
sudo make install`, 'Building and installing from source'),
  P('Build with `make`, not `make -j`: the Makefile of Regina 3.9.7 has a race between parallel jobs. After changing a structure in `mt.h` or `regina_t.h`, or a node type in `defs.h`, run `make clean` first. After changing `lexsrc.l` or `yaccsrc.y`, run `tools/regenerate-parser.sh` and commit the generated files.'),
  H3('Building the packages'),
  ...table(['Script', 'Builds', 'Needs'], [
    ['`tools/build-deb.sh [dir]`', 'the Debian package; runs the regression tests first', '`dpkg-deb`, a C compiler'],
    ['`tools/build-windows.sh [dir]`', 'the standalone `rexx.exe` and the zip file', 'MinGW-w64 (`x86_64-w64-mingw32-gcc`)'],
    ['`tools/build-macos.sh [dir]`', 'the universal macOS `rexx` and the tar.gz file', 'zig (`pip install ziglang`), `lipo` or `llvm-lipo`'],
  ], W(2900, 3938, 2800), 'Package build scripts'),
  P('The Windows and macOS builds leave out readline (line editing in interactive trace), the curses functions and GCI, as their scripts are standalone cross builds. On a Mac, `./configure && make` builds the full interpreter instead.'),
];

// ------------------------------------------------------------------
// Chapter 3
// ------------------------------------------------------------------
const ch3 = [
  H1('Chapter 3. First Steps'),
  H2('A program with a class'),
  P('An object-oriented REXX program has two parts. First comes the main program, as in classic REXX. Then come the directives: lines starting with `::` that define classes, methods and routines. The main program ends at the first directive, as if it ended with EXIT.'),
  ...screen(`/* greet.rexx */
g = .greeter~new('Hello')
g~greet('world')
say g~count 'greeting(s)'
exit

::class Greeter
::method init
  expose word count
  use arg word
  count = 0
::method greet
  expose word count
  use arg who
  say word',' who'!'
  count = count + 1
::method count
  expose count
  return count`, 'greet.rexx'),
  ...screen(`rexx greet.rexx
Hello, world!
1 greeting(s)`, 'Running greet.rexx'),
  P('`.greeter` is an environment symbol: a name with a leading dot that refers to the class. Sending it `new` creates an instance and runs its `init` method with the arguments of `new`. `EXPOSE` makes the listed variables the object\'s own, shared by all its methods. `USE ARG` takes the arguments without converting them to strings, so objects keep their identity.'),
  H2('When the OO features are on'),
  P('Message operators (`~`, `->`), `[ ]`, environment symbols and directives always work, because they were not valid in classic REXX. A few new keywords are different: `EXPOSE`, `USE ARG`, `LOOP` and the DO options `LABEL`, `COUNTER` and `WITH` used to be ordinary words. A classic program may use `loop` as a variable or `use arg` as a command. These keywords are therefore recognized only where the program asks for the extension:'),
  ...table(['In the program or at startup', 'Effect'], [
    ['`/* OOREGINA */`', 'A comment holding only this word, anywhere in the file: the keywords are on in the whole file.'],
    ['`OPTIONS OOREGINA` or `OPTIONS \'OOREGINA\'`', 'The same; the word may also be part of a longer option string such as `OPTIONS \'OOREGINA STRICT_ANSI\'`.'],
    ['a `::` directive line', 'Any program with directives has the keywords on in the whole file.'],
    ['an earlier `~`, `->` or `[`', 'From that point in the source on.'],
    ['INTERPRET', 'Inside interpreted strings, once the program uses objects.'],
    ['`rexx --oo` or `REGINA_OO=on`', 'On in every program, required file and INTERPRET string.'],
    ['`rexx --no-oo` or `REGINA_OO=off`', 'The whole extension is off; programs behave exactly as on Regina 3.9.7, and markers are ignored.'],
  ], W(3300, 6338), 'Switching the OO keywords on'),
  P('Plain Regina 3.9.7 ignores both markers (an unknown option is skipped), so a marked classic program still runs there. The markers are found before the program is parsed, so they can be on any line. A marker inside a string or inside a longer comment does not count.'),
  P('A switch on the command line wins over `REGINA_OO`. The environment variable is meant for programs started through the REXX API (`RexxStart`) and for `#!` scripts.'),
  H2('Where objects become strings'),
  P('An object is converted to a string, by sending it the STRING message, wherever REXX needs a string:'),
  ...UL([
    'in SAY, in concatenations (`||`, blank and abuttal) and in comparisons with a string on the left,',
    'as an argument of a built-in function such as `length(obj)`,',
    'in arithmetic, when the object is the right operand,',
    'as a command, in PUSH and QUEUE, in INTERPRET and in PARSE,',
    'as the result of the whole program.',
  ]),
  P('Assignment, arguments of your own routines and methods, RETURN, USE ARG and the items of stems and collections keep the object itself. Every class inherits a STRING method from Object that returns a description such as `an Array`; a class can define its own.'),
];

// ------------------------------------------------------------------
// Chapter 4
// ------------------------------------------------------------------
const ch4 = [
  H1('Chapter 4. Differences from Classic Regina'),
  P('This chapter lists everything that can behave differently from Regina 3.9.7. With `--no-oo`, none of it applies.'),
  H2('Constructs that were errors'),
  ...table(['Construct', 'Regina 3.9.7', 'With the extension'], [
    ['`a~b`, `a~b(1)`, `(x)~b`', 'Error 64, syntax error', 'Sends message B'],
    ['`a->b`, `a -> b`', 'Error 35.1 at `>`', 'Sends message B'],
    ['`a~~b`, `a ->> b`', 'Error 64 / 35.1', 'Cascade: sends B, the result is A'],
    ['`a[1]`, `a[i, j]`', 'Error 13.1, invalid character', 'Sends `[]`'],
    ['`::class Foo` (any `::` clause)', 'Error 20.1', 'Directive'],
    ['`1 + obj`, `\'41\' = obj`', 'not possible (no objects)', 'Uses the string value of the object'],
  ], W(3000, 3000, 3638), 'Constructs that were errors in classic Regina'),
  H2('Constructs that keep their meaning'),
  ...table(['Construct', 'Meaning in both'], [
    ['`a ~b`', 'A concatenated with NOT B (blank before the tilde)'],
    ['`~b`, `a~=b`, `a~ =b`, `say~b`', 'NOT and the comparison "not equal"'],
    ['`a [1]`', 'Error 13.1: `[` must follow a term directly'],
    ['`.nil` in a program that uses no objects', 'The constant symbol `.NIL`'],
    ['`use arg x`, `expose a`, `loop`', 'Commands or ordinary words, unless the OO keywords are on'],
    ['`do counter`, `do label c`, `do with`', 'Count loops with the variables COUNTER, LABEL and WITH'],
    ['`do x over 5` (X and OVER empty)', 'A loop running 5 times, as in classic REXX'],
    ['`call f st.` in classic code', 'Passes the string value of `st.`'],
    ['PARSE VERSION, `rexx -v`', 'Unchanged version string'],
  ], W(3600, 6038), 'Constructs that keep their classic meaning'),
  H2('Behaviour that changes in OO code'),
  ...UL([
    '**RETURN of a stem.** In code with the OO keywords on, `RETURN x.` returns a Stem object with a copy of the stem instead of the stem\'s default value. Its string value is still the default value or the stem name, so `say f()` prints the same as before.',
    '**Stem assignment from a Stem object.** `r. = f()` copies the returned Stem object into `r.`. In classic REXX the right side is always a string, so this cannot occur there.',
    '**Message instructions set RESULT.** A message used as an instruction, such as `list~append(x)`, sets the special variable RESULT to the message\'s result, as CALL does. Do not use a variable named RESULT for your own data in OO code.',
    '**TRACE.** Messages and directives are traced like other clauses; TRACE I also shows the position counter of DO OVER loops.',
  ]),
  H2('Pitfalls'),
  P('Three classic REXX rules surprise programmers who write object-oriented code for the first time:'),
  ...UL([
    '**A string followed by X or B is a hex or binary string.** `\'(\'x\', \'y\')\'` is an error (error 15) because `\'(\'x` is a hex string. Write `\'(\' || x || \', \' || y || \')\'`.',
    '**Tails are case sensitive.** `s.tail` with `tail = \'count\'` sets the tail `count`, while `s.count` reads the tail `COUNT`.',
    '**A blank before `~` changes the meaning.** `obj ~msg` is "obj concatenated with NOT msg". Write `obj~msg` or `obj -> msg`.',
  ]),
  H2('New error numbers'),
  P('The extension adds the error numbers 88 (invalid argument), 91 (no result object), 97 (method not found), 98 (execution error) and 99 (translation error), with the numbering of ooRexx. Regina 3.9.7 did not use these numbers. Appendix A lists all messages.'),
];

// ------------------------------------------------------------------
// Chapter 5
// ------------------------------------------------------------------
const ch5 = [
  H1('Chapter 5. Differences from Open Object Rexx'),
  P('The extension follows ooRexx syntax, so many ooRexx programs that use classes, methods and the common collections run unchanged. ooRexx is a much larger system, however. This chapter lists what is supported, what behaves differently and what is missing.'),
  H2('Supported as in ooRexx'),
  ...UL([
    'Messages with `~`, cascades with `~~`, `~name:super`, attribute assignment `obj~name = value`, `[]` and `[]=`.',
    'The directives `::class` (with SUBCLASS, MIXINCLASS and INHERIT), `::method` (CLASS, PRIVATE, ABSTRACT, ATTRIBUTE), `::attribute` (GET, SET, CLASS), `::constant`, `::routine`, `::requires`.',
    'SELF and SUPER, EXPOSE, USE ARG, the UNKNOWN method, operator methods, mixins and multiple inheritance.',
    '`DO x OVER`, `DO WITH INDEX i ITEM x OVER`, `DO ... FOR` with OVER, `DO COUNTER`, `DO LABEL`, LOOP.',
    'The error numbers 88, 91, 97, 98 and 99.',
  ]),
  H2('Additions'),
  ...UL([
    '`->` and `->>` as alternative spellings of `~` and `~~`, with blanks allowed around them.',
    'The markers `OOREGINA` and the switches `--oo` and `--no-oo` (ooRexx has no classic mode).',
    'Regina\'s classic features remain: its built-in functions, OPTIONS, the external queue, ADDRESS WITH and the Regina API.',
  ]),
  H2('Different behaviour'),
  ...table(['Topic', 'ooRexx', 'Regina Rexx OO'], [
    ['Memory', 'Garbage collection frees unused objects', 'Objects live until the program ends (Chapter 9)'],
    ['Keywords', 'Always reserved where they apply', 'EXPOSE, USE ARG, LOOP and the DO options need a marker, a directive or `--oo` (Chapter 3)'],
    ['`.nil` and other environment symbols', 'Always objects', 'Constant symbols until the object system is active'],
    ['Stem assignment `r. = stemobject`', 'r. refers to the same Stem object', 'r. receives a copy'],
    ['`a. = b.`', 'a. refers to the Stem object of b.', 'Classic REXX: the default value of a. is set'],
    ['Class names', '`~id` returns the name', '`~id` returns the name in uppercase for classes defined with `::class`'],
    ['Array', 'Multi-dimensional arrays', 'One dimension; indexes up to 100,000,000'],
    ['Sorting', '`sort` uses the Comparable interface; `sortWith`', '`sort` orders the string values; no comparators'],
    ['`::requires` search', 'Program folder, current folder, REXX_PATH, PATH', 'Program folder, current folder, REGINA_MACROS; extensions "", .cls, .rex, .rexx'],
    ['Required files', 'Only PUBLIC classes and routines are visible to the requiring program', 'All classes and routines are visible'],
    ['Error messages', 'ooRexx texts', 'Same numbers, Regina texts (English, German)'],
  ], W(2400, 3300, 3938), 'Behaviour that differs from ooRexx'),
  H2('Not available'),
  ...UL([
    '**Concurrency:** GUARD, REPLY, START, the GUARDED and UNGUARDED options (accepted and ignored), and the Message and Monitor classes.',
    '**Instructions:** RAISE, FORWARD, SIGNAL ON USER, SELECT LABEL, `::resource`.',
    '**Environment symbols:** `.methods`, `.routines`, `.context`, `.stdout`, `.stdin`, `.error`, `.endofline` and similar.',
    '**Classes:** Bag, Set, Relation, CircularQueue, IdentityTable, Collection classes as mixins (OrderedCollection and others), Comparable, Comparator, Method, Routine, Package, Stream, File, DateTime, TimeSpan, RegularExpression, RexxContext, WeakReference, Pointer and Buffer.',
    '**Methods:** many methods of the String class beyond the built-in functions, for example `caselessPos` or `decodeBase64`, and many methods of the collection classes.',
  ]),
  P('Most ooRexx programs that use these features need small changes. Programs that use only classes, methods and the common collections usually run as they are.'),
];

// ------------------------------------------------------------------
// Chapter 6
// ------------------------------------------------------------------
const ch6 = [
  H1('Chapter 6. Language Reference'),
  H2('Messages'),
  ...table(['Form', 'Meaning'], [
    ['`receiver~name`\n`receiver~name(arg1, arg2)`', 'Sends the message NAME to the receiver. The result is the method\'s return value; a message in an expression must return a result (error 91).'],
    ['`receiver -> name(args)`', 'The same as `~`. Blanks are allowed around `->`; the characters `-` and `>` must be adjacent.'],
    ['`receiver~~name(args)`\n`receiver ->> name(args)`', 'Cascade: sends the message and returns the receiver, so several messages can be chained: `list ->> append(1) ->> append(2)`.'],
    ['`receiver~name:super(args)`', 'Inside a method: starts the method lookup in the superclass of the method\'s class.'],
    ['`receiver~name = value`\n`receiver~name(i) = value`', 'Sends `name=` with the value as first argument (and I as second): attribute assignment.'],
    ['`receiver[i]`, `receiver[i, j]`', 'Sends `[]` with the indexes. The `[` must follow the receiver without a blank.'],
    ['`receiver[i] = value`', 'Sends `[]=` with the value and the indexes.'],
    ['`receiver~"+"(x)`', 'A message name can be a string; this is how operator methods are called directly.'],
  ], W(3300, 6338), 'Message forms'),
  P('The receiver can be any term: a variable, a literal (`\'abc\'~length`), an expression in parentheses or another message. Message terms bind tighter than prefix operators: `-a~size` is `-(a~size)`. A string receiver is an instance of the String class.'),
  P('A message can be used as an instruction: `obj~display`. Its result, if any, is assigned to the special variable RESULT.'),
  H2('Environment symbols'),
  P('A symbol with a leading dot followed by a letter, such as `.array` or `.nil`, is an environment symbol. It names a class (built-in or defined with `::class`), or one of these objects:'),
  ...table(['Symbol', 'Value'], [
    ['`.nil`', 'The NIL object, "no value"; `obj~isNil` tests for it. Collections return it for missing items.'],
    ['`.true`, `.false`', 'The strings `1` and `0`.'],
    ['`.environment`, `.local`', 'A Directory for global values shared by all parts of the program.'],
  ], W(2400, 7238), 'Environment objects'),
  P('An environment symbol that names nothing evaluates to its own name, as a constant symbol does in classic REXX.'),
  H2('Instructions'),
  H3('USE ARG'),
  ...screen(`USE ARG name1, name2., ...`),
  P('Assigns the arguments of the routine or method to the variables in order, without converting objects to strings. An omitted argument drops the variable; `arg(n, \'O\')` tests for it. A name can be left out to skip an argument: `use arg , second`.'),
  P('If a name ends with a dot (`s.`) and the caller passed a plain stem (`call f st.`, `f(st.)` or `obj~m(st.)`), `s.` becomes an alias of the caller\'s stem: the callee reads the caller\'s values, and everything it sets, drops or adds is seen by the caller. If the argument is a Stem object, `s.` refers to it. Any other value becomes the default value of `s.`.'),
  H3('EXPOSE'),
  ...screen(`EXPOSE name1 name2 stem. ...`),
  P('The first instruction of a method. The listed variables are the object\'s own variables, shared by all methods of the same class for that object. A stem or compound variable can be exposed as well. Variables that are not exposed are local to the method call.'),
  H3('DO and LOOP'),
  P('All classic forms of DO work as before. The extension adds:'),
  ...table(['Form', 'Meaning'], [
    ['`DO x OVER coll`', 'X takes each item of an Array, List or Queue, each index of a Directory, StringTable or Table, each tail of a stem (`DO t OVER st.`) or of a Stem object. Other objects are asked for `makeArray`.'],
    ['`DO x OVER coll FOR n`', 'At most N iterations.'],
    ['`DO WITH INDEX i ITEM x OVER coll`', 'Sets the index and the item: the position and element of an Array, the key and value of a Directory or Table, the tail and value of a stem. Either part may be left out, the order is free, FOR may follow. Other objects are asked for `supplier`, then `makeArray`.'],
    ['`DO COUNTER c ...`', 'C is 0 at the start and grows by 1 before each pass through the body. Works with every kind of loop.'],
    ['`DO LABEL name ...`', 'Names the loop for LEAVE, ITERATE and END. The control variable still works there too. A plain `DO LABEL name` group runs once and can be left with `LEAVE name`.'],
    ['`LOOP ...`', 'The same as DO, except that LOOP without repetitor and condition loops until LEAVE.'],
  ], W(3300, 6338), 'The new forms of DO'),
  P('WHILE and UNTIL work with all forms. In an OVER loop, WHILE is tested after the variables have been set to the next item. The collection is copied when the loop starts, so changes to it in the body do not change the loop. LABEL and COUNTER come first, in either order: `DO LABEL outer COUNTER n i = 1 TO 10`.'),
  H3('RETURN with a stem'),
  P('In code with the OO keywords on, `RETURN x.` returns a new Stem object with a copy of all tails and the default value of `x.`. Assigned to a stem (`r. = f()`), it replaces `r.` with that copy; assigned to a simple variable, it is an object (see Stem in Chapter 7).'),
  H2('Directives'),
  P('Directives start with `::` at the beginning of a clause. They follow the main program; the main program ends at the first directive.'),
  ...table(['Directive', 'Options and meaning'], [
    ['`::class name`', '`SUBCLASS parent` (default Object), `MIXINCLASS parent` (a class whose methods other classes inherit), `INHERIT mixin ...`. `PUBLIC`, `PRIVATE` and `METACLASS` are accepted; in this release all classes of a required file are visible (Chapter 9).'],
    ['`::method name`', '`CLASS` (a class method), `PRIVATE` (only through SELF), `ABSTRACT` (subclasses must define it; calling it is error 98.6), `ATTRIBUTE`. `GUARDED`, `UNGUARDED`, `PROTECTED` and `UNPROTECTED` are accepted and ignored. Operator methods are named in quotes: `::method "+"`.'],
    ['`::attribute name`', 'Defines `name` and `name=` for an exposed variable. `GET` or `SET` defines only one of them; `CLASS` and `PRIVATE` as for methods.'],
    ['`::constant name value`', 'A method that returns VALUE.'],
    ['`::routine name`', 'A routine called with `CALL name` or `name()`. `PUBLIC` and `PRIVATE` are accepted; all routines of a required file are visible.'],
    ['`::requires \'file\'`', 'Loads the classes and public routines of another file and runs its main program (prolog) once. The file is searched next to the program, in the current directory and in REGINA_MACROS, with the extensions "", .cls, .rex and .rexx.'],
    ['`::options ...`', 'Accepted and ignored.'],
  ], W(2400, 7238), 'Directives'),
  H2('Methods'),
  P('A method body runs like a routine called with PROCEDURE: it has its own variables (except those it EXPOSEs), its own condition traps and the default NUMERIC settings. A SYNTAX error in a method that the method does not trap is passed to the caller, so `SIGNAL ON SYNTAX` in the caller catches it. Labels in a method or routine belong to that body.'),
  ...table(['Name', 'Meaning inside a method'], [
    ['SELF', 'The receiver of the message.'],
    ['SUPER', 'The superclass of the method\'s class, for `~name:super`.'],
    ['`arg()`, `arg(n)`', 'The arguments, as in a routine.'],
    ['UNKNOWN', 'A method named UNKNOWN receives every message the class does not define, with the arguments NAME and an Array of the arguments.'],
    ['INIT', 'Runs for every new instance, with the arguments of `new`.'],
  ], W(2400, 7238), 'Special names in methods'),
  P('Method lookup searches the class, then its inherited mixins, then its superclasses. Private methods can be called only through SELF.'),
  H2('Operators'),
  P('If the left operand of an operator is an object, the operator is sent to it as a message: `a + b` is `a~"+"(b)`, and the prefix `-a` is `a~"-"()`. This works for the arithmetic, comparison, logical and concatenation operators. Without an operator method, objects compare by identity (`=`, `==`, `\\=`, `<>` and the others), and the other operators use the string value. If the left operand is not an object but the right one is, the right operand is used by its string value.'),
];

// ------------------------------------------------------------------
// Chapter 7
// ------------------------------------------------------------------
const cls = (name, desc, methods) => [H2(name), P(desc), ...table(['Method', 'Meaning'], methods, W(3100, 6538))];
const ch7 = [
  H1('Chapter 7. Built-in Classes'),
  P('All classes are subclasses of Object. Class methods are sent to the class (`.array~of(1, 2)`), instance methods to an object. Method names are not case sensitive.'),
  ...cls('Object', 'The root of all classes.', [
    ['`init`', 'Called by `new`; does nothing.'],
    ['`string`, `defaultName`, `objectName`', 'A description such as `an Array`; `objectName=` sets a name.'],
    ['`class`', 'The object\'s class.'],
    ['`isA(c)`, `isInstanceOf(c)`', 'True if the object is an instance of class C or of a subclass.'],
    ['`= == \\= \\== <> ><`', 'Identity comparisons.'],
    ['`hashCode`, `identityHash`', 'A number identifying the object.'],
    ['`isNil`', 'True only for `.nil`.'],
    ['`send(name, args...)`, `sendWith(name, array)`', 'Sends a message whose name is in a variable.'],
    ['`hasMethod(name)`', 'True if the object understands NAME.'],
    ['`request(classid)`', 'The object itself if it is of that class, otherwise `.nil`.'],
    ['`copy`', 'A copy of the object with copies of its variables.'],
  ]),
  ...cls('Class', 'Every class is an object of class Class.', [
    ['`new(args...)`', 'Creates an instance and runs its `init`.'],
    ['`id`, `name`', 'The name of the class.'],
    ['`superclass`, `superclasses`', 'The direct superclass, an Array of all direct superclasses.'],
    ['`subclass(name)`, `mixinclass(name)`', 'Creates a subclass at run time.'],
    ['`inherit(mixin)`', 'Adds a mixin class.'],
    ['`isSubclassOf(c)`', 'True if C is a superclass or an inherited mixin.'],
    ['`queryMixinClass`', 'True if the class is a mixin class.'],
    ['`methods`', 'An Array with the names of the instance methods.'],
  ]),
  ...cls('String', 'Every string is an object of class String. Each built-in string function is also a method, with the string as first argument: `\'abc\'~length`, `\'a-b\'~changeStr(\'-\', \'+\')`, `\'hello\'~substr(2, 3)`. In addition:', [
    ['`upper`, `lower`', 'Uppercase, lowercase.'],
    ['`equals(s)`, `caselessEquals(s)`', 'Comparison.'],
    ['`startsWith(s)`, `endsWith(s)`, `contains(s)`', 'And their caseless forms.'],
    ['`makeArray(separator)`', 'An Array of the parts; without separator, the lines.'],
    ['`makeString`, `string`', 'The string itself.'],
  ]),
  ...cls('Array, Queue and List', 'Sequences indexed by position. Array has holes where no item was stored; Queue and List are ordered lists.', [
    ['`of(items...)` (class)', 'A new collection with these items.'],
    ['`[] at(i)`, `[]= put(item, i)`', 'Read and store; a missing item is `.nil`.'],
    ['`append(item)`, `insert(item, i)`', 'Add at the end, insert after position I.'],
    ['`size`, `items`, `isEmpty`, `empty`', 'Last index, number of items, test, remove all.'],
    ['`first`, `last`, `firstItem`, `lastItem`, `next(i)`, `previous(i)`', 'Navigation.'],
    ['`hasIndex(i)`, `hasItem(x)`, `index(x)`', 'Tests and search.'],
    ['`remove(i)`, `removeItem(x)`', 'Remove by position or by value.'],
    ['`makeArray`, `allItems`, `allIndexes`, `supplier`, `section(i, n)`', 'Copies and walking.'],
    ['`toString(format, separator)`', 'One string; format `C` joins without line breaks.'],
    ['Array only: `fill(x)`, `sort`, `delete(i)`', 'Fill, sort the string values, delete with shift.'],
    ['Queue only: `push`, `queue`, `pull`, `peek`', 'Add in front or at the end, take and look at the first item.'],
  ]),
  ...cls('Directory, StringTable and Table', 'Collections indexed by key. Directory and StringTable use the string value of the key; Table also accepts objects as keys and compares them by identity.', [
    ['`[] at(k)`, `[]= put(item, k)`', 'Read and store; keys are case sensitive.'],
    ['`hasIndex(k)`, `hasItem(x)`, `index(x)`, `remove(k)`', 'Tests, search and removal.'],
    ['`items`, `isEmpty`, `empty`', 'Counting and clearing.'],
    ['`makeArray`, `allIndexes`, `allItems`, `supplier`', 'Keys, items and walking.'],
    ['Directory, StringTable: `entry`, `setEntry`, `hasEntry`, `removeEntry`', 'Access by name, uppercased as in ooRexx.'],
    ['Directory, StringTable: any other message', '`d~city` reads and `d~city = x` sets the entry CITY.'],
  ]),
  ...cls('Stem', 'A stem as an object. `RETURN x.` creates one in OO code; `.stem~new` or `.stem~new(default)` creates an empty one.', [
    ['`[] at(t...)`, `[]= put(v, t...)`', 'Read and set a tail; several indexes are joined with dots. A missing tail gives the default value or `.nil`.'],
    ['`remove(t)`, `hasIndex(t)`', 'Drop and test a tail.'],
    ['`items`, `isEmpty`, `empty`', 'Number of tails with a value, test, drop all.'],
    ['`allIndexes`, `makeArray`, `allItems`, `supplier`', 'Tails sorted as DO OVER sorts them, values, walking.'],
    ['`string`', 'The default value, or the stem\'s name.'],
  ]),
  ...cls('MutableBuffer', 'A string that can be changed in place.', [
    ['`new(s)`, `append(s...)`, `insert(s, pos)`, `delete(pos, n)`', 'Build and change; `append` returns the buffer.'],
    ['`length`, `string`', 'Length and value.'],
    ['`substr pos lastPos left right words word countStr verify`', 'As the built-in functions.'],
  ]),
  ...cls('Supplier', 'Walks index/item pairs, returned by `supplier`.', [
    ['`available`, `index`, `item`, `next`', 'Test, current index and item, step.'],
    ['`allIndexes`, `allItems`', 'The remaining indexes and items.'],
  ]),
];

// ------------------------------------------------------------------
// Chapter 8: samples
// ------------------------------------------------------------------
const samples = [
  ['01_classes', 'Classes and methods', 'A bank account class shows instance variables, read-only attributes, a private helper method, class methods that count the accounts, and a STRING method that SAY uses.'],
  ['02_inheritance', 'Inheritance and class libraries', 'Shapes with an abstract AREA method, defined in the library `shapes.cls` and loaded with `::requires`. The loop handles every shape the same way (polymorphism).'],
  ['03_collections', 'Collections', 'Counting words with a Directory, sorting with an Array, a queue of jobs, objects as keys in a Table, and a Supplier.'],
  ['04_operators', 'Operator methods', 'A Fraction class that defines `+ - * / = <` and the prefix minus, so fractions calculate exactly like numbers.'],
  ['05_stems', 'Stems as arguments and results', 'A routine that sorts the caller\'s stem in place, a routine that returns a new stem, and a returned stem used as an object.'],
  ['06_data_structures', 'Data structures', 'A Stack built on Array and a binary search tree of Node objects linked by references.'],
  ['07_dynamic', 'Dynamic messages', 'A logging proxy built with UNKNOWN, a mixin class that adds PRINT to two unrelated classes, and messages chosen at run time with SEND and SENDWITH.'],
  ['08_inventory', 'A small application', 'Stock keeping with Product and Inventory classes: orders, a sorted value report and a reorder list.'],
];
const ch8 = [
  H1('Chapter 8. Sample Programs'),
  P('The sample programs are in the folder `demo/oo` of every package and of the repository; the Debian package installs them in `/usr/share/doc/regina-rexx-oo/examples`. Each section below shows a program and its output. The output is checked by the `samples` test suite, so it is what the program prints.'),
  ...table(['Program', 'Shows'], samples.map(([f, t]) => [`\`${f}.rexx\``, t])
    .concat([['`shapes.cls`', 'The class library used by 02_inheritance']]), W(3100, 6538), 'The sample programs'),
];
for (const [f, title, desc] of samples) {
  ch8.push(H2(title), P(desc), ...screen(read(`demo/oo/${f}.rexx`), `${f}.rexx`));
  if (f === '02_inheritance')
    ch8.push(...screen(read('demo/oo/shapes.cls'), 'shapes.cls'));
  ch8.push(...screen(`rexx ${f}.rexx\n` + output(f), `Output of ${f}.rexx`));
}

// ------------------------------------------------------------------
// Chapter 9: restrictions
// ------------------------------------------------------------------
const ch9 = [
  H1('Chapter 9. Restrictions and Known Problems'),
  H2('Memory'),
  ...UL([
    '**No garbage collection.** An object lives until the program ends, even when no variable refers to it any more. A program that creates many short-lived objects in a long loop uses more and more memory. Reuse objects in such loops, or keep the work in strings and stems.',
    'All objects of a program are freed when the program ends, also when it is called through the REXX API.',
  ]),
  H2('Language'),
  ...UL([
    'The ooRexx features listed in Chapter 5 under "Not available" are missing, notably RAISE, FORWARD, GUARD, REPLY, START and many classes.',
    'Arrays have one dimension; an index above 100,000,000 is error 88.6.',
    '`sort` orders the string values of the items; there are no comparators.',
    'The new keywords need a marker, a directive, an earlier message operator or `--oo` (Chapter 3). A program that uses objects only through routine arguments, without any of these, must add a marker to use them.',
    'Directives are not allowed in INTERPRET data (error 98.2).',
    'Only a plain stem argument (`st.`) is passed by reference; a compound variable or an expression is passed by value. `RETURN x.` returns a copy, and `EXIT x.` returns the stem\'s string value as in classic REXX.',
    'An operator with an object as right operand and a string as left operand uses the object\'s string value; the object\'s operator method is not called.',
    'Class IDs of classes defined with `::class` are in uppercase.',
    'PUBLIC and PRIVATE on `::class` and `::routine` are accepted but have no effect: all classes and routines of a file loaded with `::requires` are visible to the program that loads it.',
    'The collection of a DO OVER loop is copied when the loop starts; changes in the body do not affect the loop.',
    'Objects belong to the thread that created them and cannot be shared between threads that run REXX programs at the same time.',
  ]),
  H2('Errors and tracing'),
  ...UL([
    'An error in a file loaded by `::requires` names the main program in the error message, not the required file.',
    'TRACE I shows the hidden position counter of DO OVER loops.',
    'Deep recursion of methods ends with error 11 (control stack full), based on the measured stack use. The Windows and macOS builds have a 256 MB stack.',
  ]),
  H2('Platforms'),
  ...table(['Package', 'Status'], [
    ['Linux x86-64 (`.deb`)', 'Built and tested natively: all four test suites, valgrind.'],
    ['Windows 64-bit (zip)', 'Cross-compiled with MinGW-w64. The oo test suite passes under Wine; not yet tested on Windows itself. No readline, curses or GCI.'],
    ['macOS (tar.gz)', 'Cross-compiled with zig for Apple Silicon and Intel, macOS 11 or later. The same sources and configuration pass all test suites on Linux, but the binary has not yet been run on a Mac. It is ad-hoc signed, not notarized (see Chapter 2). No readline, curses or GCI.'],
  ], W(2400, 7238), 'Platform status'),
  H2('Performance'),
  P('Classic programs run at the speed of Regina 3.9.7: built with the same compiler options, the REXXCPS benchmark gave a median of 9.67 million clauses per second against 9.51 million for Regina 3.9.7 (seven runs each), a difference within the noise of the measurement. A message is slower than a call of an internal routine, because it creates a new variable scope and looks up the method. Collections are implemented in C and are fast; a loop over thousands of items is no problem.'),
];

// ------------------------------------------------------------------
// Appendices
// ------------------------------------------------------------------
const msgs = read('en.mts').split('\n')
  .map((l) => l.match(/^\s*(88|91|97|98|99),\s*(\d+),\[(.*?)\]/))
  .filter(Boolean)
  .map((m) => [`${m[1]}.${m[2]}`, m[3].replace(/%s/g, '...').replace(/%d/g, 'n')]);
const appA = [
  H1('Appendix A. Error Messages'),
  P('The messages of the extension, as printed by the English message file. "..." stands for a name or value filled in at run time. The texts are also available in German; the other languages show the English texts.'),
  ...table(['Error', 'Message'], msgs, W(1100, 8538), 'Error messages of the extension'),
  P('Errors of the extension are SYNTAX conditions: `SIGNAL ON SYNTAX` traps them, RC holds the main number and `condition(\'D\')` the message.'),
];
const appB = [
  H1('Appendix B. Switches and Environment Variables'),
  ...table(['Switch or variable', 'Meaning'], [
    ['`--oo`, `-O`', 'The OO keywords are on in every program, required file and INTERPRET string.'],
    ['`--no-oo`, `-N`', 'The extension is off: Regina 3.9.7 behaviour.'],
    ['`REGINA_OO`', '`on`, `1`, `yes` or `oo` act like `--oo`; `off`, `0`, `no` or `classic` like `--no-oo`. A command-line switch wins.'],
    ['`REGINA_MACROS`', 'Folders searched for external routines and for `::requires` files.'],
    ['`REGINA_LANG`, `REGINA_LANG_DIR`', 'Language of the error messages and the folder of the `*.mtb` files.'],
  ], W(3100, 6538), 'Switches and environment variables'),
  P('All other switches of Regina 3.9.7 work as before; `rexx -h` lists them.'),
];
const appC = [
  H1('Appendix C. Contents of the Packages'),
  ...table(['Package', 'Contents'], [
    ['Debian', '`/usr/bin`: rexx, regina, rxstack, rxqueue, regina-config\n`/usr/lib/<arch>`: libregina.so, libregina.a, add-on libraries\n`/usr/share/regina-rexx`: message files and the classic examples\n`/usr/share/doc/regina-rexx-oo`: README.OO.md, this manual, examples (the OO samples)\n`/usr/share/man/man1`: man pages'],
    ['Windows', '`rexx.exe`, `*.mtb`, `README.txt`, `README.OO.md`, this manual, `COPYING-LIB.txt`, `demo` with the classic examples and `demo\\oo` with the OO samples'],
    ['macOS', '`rexx` (universal), `*.mtb`, `INSTALL-macOS.txt`, `README.OO.md`, this manual, `COPYING-LIB.txt`, `demo` and `demo/oo`'],
  ], W(1600, 8038), 'Package contents'),
];

// ------------------------------------------------------------------
// the document
// ------------------------------------------------------------------
const header = new Header({ children: [new Paragraph({
  border: { bottom: { style: BorderStyle.SINGLE, size: 6, color: '000000', space: 4 } },
  children: [new TextRun({ text: TITLE, font: ARIAL, size: 16 })] })] });
const footer = new Footer({ children: [new Paragraph({
  tabStops: [{ type: TabStopType.RIGHT, position: S.BODY_W }],
  children: [new TextRun({ text: PUB, font: ARIAL, size: 16 }),
             new TextRun({ children: ['\t', PageNumber.CURRENT], size: 21 })] })] });
const page = { size: { width: 11906, height: 16838 },
               margin: { top: 1300, right: 1134, bottom: 1134, left: 1134, header: 708, footer: 708 } };

const doc = new Document({
  creator: 'Mike Beer', title: 'Regina Rexx OO: Introduction, Installation and Reference',
  description: 'Manual of the object-oriented extension of Regina Rexx 3.9.7',
  styles: S.styles, numbering: S.numbering, features: { updateFields: true },
  sections: [
    { properties: { page }, children: front },
    { properties: { page: { ...page, pageNumbers: { start: 1 } } },
      headers: { default: header }, footers: { default: footer },
      children: [...preface, ...ch1, ...ch2, ...ch3, ...ch4, ...ch5, ...ch6, ...ch7, ...ch8,
                 ...ch9, ...appA, ...appB, ...appC] },
  ],
});

const out = process.argv[2] || path.join(ROOT, 'doc', 'Regina-OO-Manual.docx');
Packer.toBuffer(doc).then((b) => { fs.writeFileSync(out, b); console.log('wrote', out); });
