/* oo_demo.rexx - try out Regina 3.9.7 + OO syntax preview 1
 *
 * Part 1 runs classic Rexx, including "~" used as NOT, to show it still
 * works. Part 2 runs every new construct. The object system does not exist
 * yet, so each new construct must be ACCEPTED by the parser and then stop
 * with error 91.1 when it runs. Anything else is a bug - please report it.
 * Part 3 shows that the main program ends at the first directive.
 *
 * Run:  rexx ./oo_demo.rexx      (Windows: rexx.exe oo_demo.rexx)
 */
signal main                 /* skip over the helper routines */

check: procedure
  parse arg label, got, expected
  if got == expected then say '  ok    ' label '=>' got
  else say '  WRONG ' label '=> got "'got'", expected "'expected'"'
  return

stemtest: procedure
  a.1 = 'one'; i = 1
  return a.i

plustest: procedure
  x = 10; x += 2
  return x

parsetest: procedure
  parse value 'abcdefghij' with 3 x +2 -3 y
  return x'/'y

try:
  parse arg code
  signal on syntax name failed
  interpret code
  say '  PARSED AND RAN (unexpected):' code
  return
failed:
  say '  ' left(code, 40) '->' condition('D')
  return

main:
parse version v
say 'Interpreter:' v
say

/* ---------------------------------------------------------------- */
say '--- Part 1: classic Rexx must behave exactly as before'
a = 1; b = 0
call check 'NOT with ~         ', ~b, 1
call check 'a ~b  (concat+NOT) ', a ~b, '1 1'
call check 'a~=b  (comparison) ', a~=b, 1
call check 'a~ =b (comparison) ', a~ =b, 1
call check 'say~b (NOT)        ', ~b, 1
call check 'stem a.1           ', stemtest(), 'one'
call check 'x += 2             ', plustest(), 12
call check 'brackets in string ', length('a[1]'), 4
call check 'parse with -3      ', parsetest(), 'cd/bcdefghij'
say

/* ---------------------------------------------------------------- */
say '--- Part 2: new syntax must parse, then stop with error 91.1'
obj = 'anything'; i = 2
call try 'x = obj~length'
call try 'x = obj~substr(2, 3)'
call try 'x = obj -> length'
call try 'x = obj -> substr(2, 3)'
call try 'x = obj~~append("a")'
call try 'x = obj ->> append("a") ->> append("b")'
call try 'x = obj~upper~reverse'
call try 'x = "literal"~length'
call try 'x = (1 + 2)~abs'
call try 'x = obj[1]'
call try 'x = obj[i, i + 1]'
call try 'x = obj~items[1]~name'
call try 'obj~display'
call try 'obj -> display'
say
say '--- Part 2b: these must stay syntax errors'
call try 'x = obj~'
call try 'x = -> length'
call try 'x = obj [1]'
call try '::class NotHere'
say

/* ---------------------------------------------------------------- */
say '--- Part 3: the main program ends at the first directive'
say 'This is the last line you should see.'

::class Account
::method init
  say 'BUG: method body was executed'
::method deposit
::routine helper
  say 'BUG: routine body was executed'
