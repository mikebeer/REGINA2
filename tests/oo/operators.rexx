v1 = .vector~new(1, 2)
v2 = .vector~new(10, 20)
v3 = v1 + v2
say v3 v1 * 3 (v1 + v2) * 2
say (v1 = v2) (v1 = .vector~new(1, 2)) (v1 == v1) (v1 \= v2) (-v1) (\ (v1 = v2))
if v1 = .vector~new(1, 2) then say 'equal by value'
say 'v1 is' v1 || '!'
total = .vector~new(0, 0)
do v over .array~of(v1, v2, v3)
  total = total + v
end
say 'total' total
s = .money~new(5)
say s + 1 s~"+"(2)
n = 4
say n + 1 n * n 'abc' || 'def'
exit

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
::method "*"
  expose x y
  use arg k
  return .vector~new(x * k, y * k)
::method "="
  expose x y
  use arg other
  return x = other~x & y = other~y
::method "\="
  return \ (self = arg(1))
::method string
  expose x y
  return '(' || x || ',' || y || ')'

::method "-"
  expose x y
  if arg() = 0 then return .vector~new(-x, -y)
  use arg o
  return self + o * -1

::class Money subclass String
