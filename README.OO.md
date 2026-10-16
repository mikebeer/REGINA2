# Object-oriented extensions for Regina Rexx

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
| `DO x OVER coll` | Iterates over the items of a collection, or over the tails of a stem (`DO t OVER st.`). WHILE/UNTIL, LEAVE and ITERATE work as usual. |
| `USE ARG a, b` | Assigns the arguments to variables without converting them to strings, so objects keep their identity. |
| `EXPOSE a b c.` | First instruction of a method: shares these variables with the object. |
| `::directive …` | Starts the directive section. The main program ends at the first directive. |

Message terms bind tighter than prefix operators: `-a~size` is `-(a~size)`.

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
  `RAISE`, `SIGNAL ON USER`, `DO … OVER … FOR`, `::resource`, and the
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
