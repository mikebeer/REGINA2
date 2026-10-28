/* 13_notation.rexx - cREXX and NetRexx notation in any OO program
 *
 * The usual Regina OO notation obj~m(args) can also be written obj.m(args),
 * as in cREXX and NetRexx. The period sends a message only when the
 * symbol before it holds an object: x.1 stays a compound variable.
 */
acct = .Account('Ann', 100)          -- .Class(args) is .Class~new(args)
acct.deposit(50)                     -- obj.m(args), also as an instruction
say acct.owner() 'has' acct.balance()
say acct.deposit(25).balance()       -- chains: a.b().c()

/* class methods and named constructors */
say .Account.count() 'account(s)'
bob = .Account.empty('Bob')
say bob.describe()

/* this (self) and null (.nil) while no variable of that name is set */
say (acct.partner() == null) 'no partner yet'
acct.link(bob)
say acct.partner().owner()

/* USE ARG defaults */
call greet
call greet 'Hello', 'Rexx'

/* type tests: is, as, typeof, and NetRexx's  x <= Class */
say (acct is .Account) (acct is .string) typeof(acct)
say (bob <= .Account) ('abc' <= .Account)
safe = acct as .Account
say safe.owner()

/* the classic meaning stays */
x.1 = 'one'; this = 'a variable'
say x.1 '/' this
exit

greet: procedure
  use arg word = 'Hi', who = 'world'
  say word who
  return

::class Account
::attribute owner get
::attribute balance get
::attribute partner get
::method init
  expose owner balance partner
  use arg owner, balance = 0
  partner = null
  this.class().added()
::method count class
  expose n
  if symbol('n') \== 'VAR' then n = 0
  return n
::method added class
  expose n
  if symbol('n') \== 'VAR' then n = 0
  n = n + 1
::method empty class
  use arg owner
  return this.new(owner)            -- this is the class in a class method
::method deposit
  expose balance
  use arg amount
  balance = balance + amount
  return this                       -- for chains
::method link
  expose partner
  use arg partner
::method describe
  return this.owner() 'with' this.balance()
