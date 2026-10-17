# Object-oriented extensions for Regina Rexx

The full manual is [doc/Regina-OO-Manual.pdf](doc/Regina-OO-Manual.pdf); this page is a short guide.

This version of Regina (3.9.7 plus the OO extension) can run object-oriented
Rexx programs in the style of ooRexx. **It is fully optional:** a program
that doesn't use any of the new syntax runs exactly as it does on Regina
3.9.7, including its TRACE output, error messages and limits. The object
system switches on only when a program sends its first message, refers to an
environment symbol such as `.array`, or contains a directive (`::class`, …).

```rexx
acct = .account~new(100)
acct~deposit(50)
acct -> withdraw(30)            /* -> is the same as ~ */
say acct~balance                /* 120 */

list = .array~new
list ->> append('a') ->> append('b')   /* cascade: returns the receiver */
say list[1] list~items          /* a 2 */
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
::method withdraw
  expose balance
  use arg amount
  if amount > balance then return 0   /* refused */
  balance = balance - amount
  return 1
```

## Syntax

| Construct | Meaning |
|---|---|
| `obj~name(args)` or `obj -> name(args)` | Sends a message. With `~`, don't put a blank before it (`a ~b` is still classic "a concatenated with NOT b"). Blanks are allowed around `->`. |
| `obj~~name` or `obj ->> name` | Cascade: sends the message and returns the receiver, not the result. |
| `obj~name:super(args)` | Starts the method lookup in the superclass. |
| `obj~name = value` | Calls `name=` (attribute assignment). `obj~name(i) = v` calls `name=` with `v` as the first argument and `i` as the second. |
| `coll[i]`, `coll[i, j]` | Sends `[]`. `coll[i] = v` sends `[]=`. The `[` must directly follow the term. |
| `.name` | Environment symbol: a class (`.array`, `.myclass`), `.nil`, `.true`, `.false`, `.environment`, `.local`. |
| `DO x OVER coll` | Iterates over a collection or the tails of a stem. See [Loops](#loops) for `DO WITH`, `FOR`, `COUNTER`, `LABEL` and `LOOP`. |
| `USE ARG a, b` | Assigns the arguments to variables without converting them to strings, so objects keep their identity. |
| `EXPOSE a b c.` | First instruction of a method: shares these variables with the object. |
| `::directive …` | Starts the directive section. The main program ends at the first directive. |

Message terms bind tighter than prefix operators: `-a~size` is `-(a~size)`.

### Loops

All classic DO forms work unchanged. The extension adds these:

| Form | Meaning |
|---|---|
| `DO x OVER coll` | `x` takes each item of an Array, List or Queue, each index of a Directory, StringTable or Table, or each tail of a stem (`DO t OVER st.`). Other objects are asked for `makeArray`. |
| `DO x OVER coll FOR n` | At most `n` iterations. |
| `DO WITH INDEX i ITEM x OVER coll` | Sets both the index and the item: the position and the element for an Array, the key and the value for a Directory or Table, the tail and its value for a stem. Either part can be left out, and the order of `INDEX` and `ITEM` doesn't matter. `FOR n` works here too. Other objects are asked for `supplier`, or else for `makeArray`. |
| `DO COUNTER c …` | Sets `c` to 0 when the loop starts and adds 1 before each pass through the body, so afterwards `c` holds the number of passes. Works with every kind of loop. |
| `DO LABEL name …` | Names the loop for `LEAVE name`, `ITERATE name` and `END name`. The loop's control variable (`i` in `DO i = …`, `x` in `DO x OVER …`) still works there too. A plain `DO LABEL name` group runs once, and `LEAVE name` leaves it. |
| `LOOP …` | The same as `DO`, except that `LOOP` with no repetitor and no condition repeats until `LEAVE` (like `DO FOREVER`). |

`WHILE` and `UNTIL` work with all of them. In an OVER loop, `WHILE` is
tested after the variables are set to the next item. The collection is
copied when the loop starts, so changing it in the body doesn't change the
loop. `LABEL` and `COUNTER` come first, in either order:
`DO LABEL outer COUNTER n i = 1 TO 10`.

```rexx
d = .directory~new
d['apple'] = 3; d['pear'] = 5
do counter n with index fruit item count over d
  say n':' fruit count
end
loop label search i = 1 to 100
  if i * i > 50 then leave search
end
```

The new keywords are only recognized where [OO keywords are on](#switching-the-oo-keywords-on).

### Switching the OO keywords on

Message operators, `[ ]`, `.name` symbols and directives always work,
because none of them was valid in classic Rexx (except `a ~b` with a blank,
which keeps its classic meaning). The new keywords `EXPOSE`, `USE ARG`,
`LOOP` and the DO options `LABEL`, `COUNTER` and `WITH` used to be ordinary
symbols or commands, so they are recognized only where the program asks
for the extension:

| In the program | Effect |
|---|---|
| `/* OOREGINA */` | A comment holding only this word, anywhere in the file. |
| `OPTIONS OOREGINA` or `OPTIONS 'OOREGINA'` | Also inside a longer option string, such as `OPTIONS 'OOREGINA STRICT_ANSI'`. |
| A `::` directive line | Any program with directives. |
| An earlier `~`, `->` or `[` | From that point in the source on. |
| INTERPRET | Inside INTERPRET data, the keywords are on once the program uses objects. |

Plain Regina 3.9.7 ignores all of these markers (an unknown option is
skipped), so a marked program still starts there. The markers are found
before the program is parsed, so they can be on any line, even after the
code that uses the keywords. A marker inside a string
or in a longer comment doesn't count. Elsewhere the words keep their classic
meaning, so a classic program with a variable named `counter` or a command
named `loop` runs as before.

### Forcing the mode when starting

| Switch | Environment | Effect |
|---|---|---|
| `--oo` or `-O` | `REGINA_OO=on` (also `1`, `yes`, `oo`) | The OO keywords are on in every program, required file and INTERPRET string, with no marker needed. Environment symbols such as `.nil` always resolve. |
| `--no-oo` or `-N` | `REGINA_OO=off` (also `0`, `no`, `classic`) | The extension is off completely: no message operators, `[ ]`, directives, `.name` symbols, `DO … OVER` or new keywords, and markers are ignored. Programs behave exactly as in Regina 3.9.7. Regina's own trip suite and the classic tests give identical output in this mode. |
| neither | `REGINA_OO` unset | Decided per program, as described above. |

A switch on the command line wins over `REGINA_OO`. The environment
variable also works for programs started through the API (`RexxStart`)
and for `#!` scripts.

```sh
rexx --oo  myprog.rexx       # OO keywords everywhere
rexx --no-oo old.rexx        # guaranteed classic behaviour
REGINA_OO=off rexx old.rexx  # the same through the environment
```

### Stems as arguments and results

```rexx
/* OOREGINA */
list.0 = 2; list.1 = 'b'; list.2 = 'a'
call sortit list.                 /* passed by reference */
say list.1 list.2                 /* a b */
r. = makelist()                   /* copied back */
say r.0 r.1                       /* 1 made */
exit
sortit: procedure
  use arg s.                      /* s. is the caller's list. */
  if s.1 > s.2 then parse value s.1 s.2 with s.2 s.1
  return
makelist: procedure
  x.0 = 1; x.1 = 'made'
  return x.                       /* returns a Stem object */
```

- **Passing:** if an argument is just a stem (`call f st.`, `f(st.)` or
  `obj~m(st.)`), `USE ARG s.` makes `s.` an alias of the caller's stem. The
  callee reads the caller's values, and everything it sets, drops or adds
  is visible to the caller afterwards. This also works with routines that
  aren't PROCEDUREs, through recursion, and in methods. `PARSE ARG` and
  `ARG()` still see the classic string value (`ST.` or the stem's default
  value).
- **Returning:** `RETURN x.` in OO code returns a Stem object with a copy of
  the stem: all tails and the default value. Assigning it to a stem,
  `r. = f()`, replaces `r.` with that copy. Assigned to a plain variable, it
  is an object: `s[1]`, `s~items`, `s~allIndexes`, `DO t OVER s` and
  `DO WITH INDEX t ITEM v OVER s` work, and `USE ARG s.` binds to it. Its
  string value is the stem's default value or its name, as in classic Rexx.
- **The Stem class:** `.stem~new` or `.stem~new(default)` creates an empty
  stem. Methods: `[] []= at put remove hasIndex items isEmpty empty
  allIndexes makeArray allItems supplier string`. Several indexes are
  joined with dots (`s['a', 'b']` is the tail `a.b`), and tails are case
  sensitive, as for compound variables.

In a classic program (none of the switches above), `call f st.` and
`RETURN x.` behave exactly as in Regina 3.9.7.

### Directives

| Directive | Options |
|---|---|
| `::class Name` | `subclass Parent`, `mixinclass Parent`, `inherit Mixin1 Mixin2`, `public`, `private`. `metaclass` is accepted and ignored. |
| `::method name` | `class` (class method), `private`, `abstract`, `attribute`. `guarded`, `unguarded`, `protected` and `unprotected` are accepted and ignored. Operator methods are named in quotes, for example `::method "+"`. |
| `::attribute name` | `get`, `set`, `class`, `private`. |
| `::constant name value` | A method that returns `value`. |
| `::routine name` | `public`, `private`. Called with `CALL name` or `name()`, like an internal routine. |
| `::requires 'file'` | Loads another file's classes and routines and runs its prolog once. The file is searched next to the program, in the current directory and in `REGINA_MACROS`, with the extensions "", `.cls`, `.rex` and `.rexx`. |
| `::options` | Accepted and ignored. |

Inside a method, `self` is the receiver and `super` is the superclass.
Each method runs with its own variables, its own condition traps and the
default NUMERIC settings, like a routine called with PROCEDURE. A SYNTAX
error in a method that the method doesn't trap is passed to the caller,
so `SIGNAL ON SYNTAX` in the caller catches it.

Labels in a method or routine body belong to that body. The main program
can't `CALL` or `SIGNAL` to them.

### Operators

If the left operand of an operator is an object, the operator is sent to it
as a message: `a + b` is `a~"+"(b)` and `-a` is `a~"-"()`. This works for
arithmetic, comparison, logical and concatenation operators. Without an
operator method, objects compare by identity (`=`, `==`, `\=`, …), and
anything else uses the object's string value.

### Where objects become strings

An object is turned into a string (by its `STRING` method) when it is used
with SAY, concatenation, a built-in function argument, a command, PUSH or
QUEUE, INTERPRET, PARSE, or as the program's result. Assignment, arguments
to your own routines and methods, RETURN, `USE ARG` and stem or collection
storage all keep the object itself.

## Built-in classes

| Class | Methods |
|---|---|
| Object | `init string defaultName objectName objectName= class isA isInstanceOf hashCode identityHash isNil send sendWith hasMethod request copy` and the identity comparisons |
| Class | `new id name superclass superclasses subclass mixinclass inherit isSubclassOf queryMixinClass methods` |
| String | `upper lower equals caselessEquals startsWith caselessStartsWith endsWith caselessEndsWith contains caselessContains makeArray`, and every string built-in function as a method: `'abc'~length`, `'a-b'~changeStr('-', '+')` |
| Array, Queue, List | `of` (class method) `[] []= at put size items append first last firstItem lastItem next previous hasIndex hasItem index remove removeItem insert makeArray allItems allIndexes isEmpty empty supplier section toString`. Array also has `fill sort delete`; Queue has `push queue pull peek`. |
| Directory, StringTable, Table | `[] []= at put hasIndex remove items isEmpty empty makeArray allIndexes allItems hasItem index supplier`. Directory also has `entry setEntry hasEntry removeEntry`, and an unknown message name reads or sets the entry of that name (`d~city`, `d~city = 'Zurich'`). |
| MutableBuffer | `init append insert delete length string substr pos lastPos left right words word countStr verify` |
| Supplier | `available item index next allItems allIndexes` |

A Directory or StringTable uses the string value of an index, and `[]`,
`at` and `put` are case sensitive. The Directory methods `entry`,
`setEntry`, `hasEntry`, `removeEntry` and the `d~name` form uppercase the
name, as in ooRexx: `d~city` finds the entry stored with `d['CITY']`. A
Table also accepts objects as indexes and compares them by identity.

## Errors

The new error numbers follow ooRexx:

| Error | Meaning |
|---|---|
| 88.x | Invalid argument (for example an array index that isn't a positive whole number, or one above 100000000) |
| 91.x | A message that must return a result didn't return one |
| 97.x | The object doesn't understand the message |
| 98.x | Execution errors: missing class, abstract method called, class inheritance cycle, … |
| 99.x | Translation errors in directives: duplicate class, method or routine, a `::method` outside a class, an unknown directive, a missing `::requires` file, … |

Messages are available in all languages that Regina ships. The texts are
English except German.

## Limitations

- **No garbage collection.** Objects live until the program ends. A program
  that creates millions of short-lived objects uses a lot of memory.
- Not implemented: `GUARD`, `REPLY`, `START` (concurrency), `FORWARD`,
  `RAISE`, `SIGNAL ON USER`, `SELECT LABEL`, `::resource`, and the
  `.methods`, `.routines` and `.context` environment symbols.
- An error in a file loaded by `::requires` names the main program, not the
  required file.
- A program run through INTERPRET can't contain directives (error 98.2).
- The classic reading of `DO x OVER y` (a counted loop with variables
  named `x` and `OVER`) still works when `y` is a whole number.

## Building and testing

- After changing `lexsrc.l` or `yaccsrc.y`, run `tools/regenerate-parser.sh`
  (flex 2.6.4, bison 3.8.2) and commit the generated files.
- Build with `make`, not `make -j` (the Makefile has a race).
- Run `tests/run-regression.sh path/to/rexx`. The `trip` and `classic`
  suites must stay identical to Regina 3.9.7; `oo` covers the extension.
  See `tests/README.md`.
- `tools/build-deb.sh` builds a Debian package, and `tools/build-windows.sh`
  builds a static Windows `rexx.exe` with MinGW.
