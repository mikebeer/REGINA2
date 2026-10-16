say "abc"~length "abc"~reverse "hello world"~word(2) "abc"~pos("c")
a = .account~new(100)
say a
a~deposit(50)
a -> deposit(25)
say 'balance' a~balance
say a~class a~class~id
say .nil .nil~isnil a~isnil
arr = .array~of(10, 20, 30)
say arr~items arr[2] arr~size
arr[4] = 40
do x over arr
  say 'item' x
end
d = .directory~new
d['one'] = 1
d~two = 2
say d['one'] d~two d~items
s = .savings~new(10)
s~deposit(5)
say s s~balance s~interest
say 'done'
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
::method string
  return 'Account with' self~balance

::class Savings subclass Account
::method interest
  return self~balance * 0.05
