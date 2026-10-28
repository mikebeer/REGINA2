/* CREXX / NetRexx notation in OO programs (stage 1) */
a = .array(3)                     -- .Class(args) is .Class~new(args)
a.put('x', 1)                     -- obj.m(args) as an instruction
say a.items() a.at(1) a[1]
d = .directory()
d.put(5, 'k')
say d.at('k') d.items()
l = .list.of(1, 2, 3)             -- .Class.f(args) is .Class~f(args)
say l.items() l.firstItem()
p = .Point(1, 2)
say p.x() p.sum() p.plus(10).x() p.plus(1).plus(2).sum()
say p.twice().x() p.twice()~y
say a.at(1).length() a[1].upper()
/* the classic meaning stays when the left side is not an object */
x.1 = 'abc'; t = 'T'
say length(x.1) x.1 x.t
/* this and null */
say this null~class~id (null == .nil)
this = 5; null = 6
say this null
drop this null
/* USE ARG defaults */
call f
call f 1
call f 1, 2
call f , 'B'
/* is, as, typeof and type comparisons */
say (a is .array) (a is .string) ('abc' is .string) (p is .point) (5 is .object)
say (p <= .point) (.point >= p) (p <= .array) (3 <= 4) ('b' >= 'a')
is = 7; as = 'x'
say is as is + 1
say (p as .point).sum() typeof(p) typeof(a) typeof('abc')
signal on syntax
c = 'abc' as .array
say 'not reached'
exit
syntax:
  say 'error' condition('c') rc
  exit

f: procedure
  use arg a = 'A', b = length(a) * 10
  say 'f:' a b
  return

::class Point
::attribute x
::attribute y
::method init
  expose x y
  use arg x, y = 0
::method sum
  expose x y
  return x + y
::method plus
  use arg n
  return .Point(this.x() + n, this.y())
::method twice
  return this.plus(this.x())
