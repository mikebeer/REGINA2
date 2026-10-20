/* 01_classes.rexx - classes, attributes, methods, class methods
 *
 * A bank account class: instance variables (EXPOSE), read-only attributes,
 * a private helper method, a class method counting the accounts, and a
 * STRING method that SAY uses.
 */
a = .account~new('Alice', 100)
b = .account~new('Bob')
a~deposit(50)
b~deposit(20)
a~transfer(30, b)
say a                                  /* SAY calls the STRING method */
say b
say 'accounts opened:' .account~count

if \b~withdraw(500) then
  say 'refused: Bob has only' b~balance
say 'owner is read-only, balance is' a~balance
exit

::class Account
::method count class                   /* class methods have their */
  expose n                             /* own variables            */
  if symbol('N') <> 'VAR' then n = 0
  return n

::method opened class
  expose n
  if symbol('N') <> 'VAR' then n = 0
  n = n + 1

::attribute owner get                  /* getters only */
::attribute balance get

::method init                          /* runs for every ~new */
  expose owner balance
  use arg owner, balance
  if arg(2, 'O') then balance = 0
  self~class~opened

::method deposit
  expose balance
  use arg amount
  if self~valid(amount) then balance = balance + amount

::method withdraw                      /* returns .true or .false */
  expose balance
  use arg amount
  if \self~valid(amount) | amount > balance then return .false
  balance = balance - amount
  return .true

::method transfer
  use arg amount, other
  if self~withdraw(amount) then other~deposit(amount)

::method valid private                 /* only callable via self */
  use arg amount
  return datatype(amount, 'N') & amount > 0

::method string
  expose owner balance
  return left(owner, 8) right(format(balance, , 2), 10)
