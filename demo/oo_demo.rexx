/* oo_demo.rexx - try out Regina 3.9.7 with the OO extension
 *
 * Part 1 runs classic Rexx, including "~" used as NOT, to show it still
 * works. Part 2 uses classes, messages, collections and operators; every
 * line must say "ok". Part 2b shows constructs that must stay errors.
 * Part 3 shows that the main program ends at the first directive.
 * Any "WRONG" or "unexpected" line is a bug - please report it.
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
  say '  RAN (unexpected):' code
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
say '--- Part 2: objects, messages and collections'
acct = .account~new(100)
acct~deposit(50)
acct -> deposit(25)
call check 'messages ~ and ->  ', acct~balance, 175
call check 'string as object   ', 'hello'~upper~reverse, 'OLLEH'
call check 'literal receiver   ', 'abc'~length, 3
call check 'expression receiver', (1 - 4)~abs, 3
arr = .array~new
arr ->> append('a') ->> append('b')
arr~~append('c')
call check 'cascades ->> and ~~', arr~items, 3
call check 'array [] index     ', arr[2], 'b'
arr[5] = 'e'
call check 'array []= assign   ', arr~size arr[5], '5 e'
list = ''
do item over arr
  list = list item
end
call check 'DO OVER            ', strip(list), 'a b c e'
d = .directory~new
d['city'] = 'Zurich'
d~country = 'CH'
call check 'directory          ', d['city'] d~country, 'Zurich CH'
v = .vector~new(1, 2) + .vector~new(10, 20)
call check 'operator method "+"', v, '(11,22)'
call check 'SAY uses ~string   ', acct, 'Account with 175'
call check 'subclass + super   ', .savings~new(10)~describe, 'Savings: Account with 10'
call check 'routine            ', twice(21), 42
signal on syntax name nomethod
x = acct~fly
say '  WRONG  unknown message did not raise an error'
nomethod:
call check 'unknown message    ', rc, 97
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
::attribute balance get
::method init
  expose balance
  use arg balance
::method deposit
  expose balance
  use arg amount
  balance = balance + amount
::method string
  expose balance
  return 'Account with' balance

::class Savings subclass Account
::method describe
  return 'Savings:' self~string:super

::class Vector
::method init
  expose x y
  use arg x, y
::method x
  expose x
  return x
::method y
  expose y
  return y
::method "+"
  expose x y
  use arg other
  return .vector~new(x + other~x, y + other~y)
::method string
  expose x y
  return '(' || x || ',' || y || ')'

::routine twice
  use arg n
  return n * 2
