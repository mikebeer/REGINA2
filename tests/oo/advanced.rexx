s = .stack~new
s~push(1); s~push(2); s~push(3)
say s~items s~pop s~pop s~items s~class~superclass~id
n = .name~new('mike')
say n n~upper n~length n~class~id
say 'tree sum' .node~new(5, .node~new(3), .node~new(8, .node~new(7)))~sum
b = .bag~new
b~add('x'); b~add('y')
say b~count b~names
.config~mode = 'fast'
say .config~mode
interpret 'say "interpreted" .array~of(1,2)~items'
x = .array~of(1, 2)
say x~items~"+"(1)
v = .array~of('b', 'a')
call trace_it
say 'ok'
exit

trace_it:
  trace r
  y = v~items
  trace o
  return

::class Stack subclass Array
::method push
  self~append(arg(1))
::method pop
  v = self[self~last]
  self~remove(self~last)
  return v

::class Name subclass String
::method upper
  return 'NAME:' self~upper:super

::class Node
::method init
  expose value left right
  use arg value, left, right
::method sum
  expose value left right
  t = value
  if symbol('LEFT') == 'VAR' then t = t + left~sum
  if symbol('RIGHT') == 'VAR' then t = t + right~sum
  return t

::class Bag
::method init
  expose items. count
  items. = ''
  count = 0
::method add
  expose items. count
  count = count + 1
  items.count = arg(1)
::method count
  expose count
  return count
::method names
  expose items. count
  r = ''
  do i = 1 to count
    r = r items.i
  end
  return strip(r)

::class Config
::attribute mode class
