options netrexx  -- NetRexx notation (stage 2)
/* a comment over
   two lines */
s = 'hello world'
say s.length() s.upper() s.word(2) s.length
t = s.reverse
say t
total = 0
loop for 3
  total = total + 1
end
say 'total' total
a = int[3]
a[1] = 'x'
say a.size() a[1]
say Greeter('Ann').greet()
g = Greeter('Bob')
say g.greet('Hi') -
    'and bye'
say (g <= Named) (g <= Greeter)
say Util.twice(21) Util.calls
say Util.twice(1) Util.calls
cnt = Counter()
cnt.inc(); cnt.inc(2)
say cnt.value cnt.getValue()
cnt.setValue(7)
say cnt.value
signal on syntax
s = Square(3)
say s.area() s.describe()           -- method calls
c = Circle(1)
say c.name 'area' c.area().format(1, 2)
l = .list~of(s, c)
loop sh over l
  say sh.describe()
end
say Shape.count 'shapes made'
p = Point(2, 3)
say p.x p.y p.dist2()
p.x = 10
say p.x
say (s <= Shape) (c <= Square)
cnt.inc('x')
exit
syntax:
  say 'error' rc':' condition('d')
  say errortext(rc) 'line' sigl
  exit

class Named interface
  method name returns String

class Greeter implements Named, Serializable
  properties private
    who
  method Greeter(w)
    who = w
  method name returns String
    return who
  method greet(word = 'Hello') returns String  -- a default argument
    return word name()

class Util
  properties static
    calls = 0
  method twice(n = int) static
    calls = calls + 1
    return n * 2

class Counter
  properties indirect
    value = int
  method inc(by = int 1)
    value = value + by

class Shape
  properties static
    count = 0
  properties
    name = String
  method Shape(aname)
    name = aname
    count = count + 1
  method area returns double
    return 0
  method describe returns String
    return name 'with area' area()

class Square extends Shape
  properties private
    side = int
  method Square(s = int)
    super('square')
    side = s
  method area returns double
    return side * side

class Circle extends Shape
  properties
    r
  method Circle(radius)
    super('circle'); r = radius
  method area
    return 3.14159 * r * r

class Point
  properties indirect
    x = int; y = int
  method Point(ax, ay = 0)
    x = ax
    y = ay
  method dist2
    return x * x + y * y
