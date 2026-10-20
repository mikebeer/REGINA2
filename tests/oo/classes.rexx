say 'main starts'
c = .circle~new(2, 'circle')
say c~describe
say .circle~pi c~pi
say 'counter:' .counter~next .counter~next .counter~next
p = .point~new(1,2)
q = p~copy
q~x = 10
say p~x p~y '|' q~x q~y
say p~send('X') p~sendWith('ADD', .array~of(5))
say p~hasmethod('add') p~hasmethod('nope')
say p~isA(.point) p~isA(.object) p~isA(.array)
call helper 'via call'
say helper('as function')
say greet~upper
signal on syntax
say p~nosuchmethod
exit
syntax:
  say 'caught' rc condition('D')
  call tests2
  exit

tests2:
  signal on syntax name s2
  x = p~private1
  say 'should not get here'
s2:
  say 'caught' rc condition('D')
  say p~callprivate
  signal on syntax name s3
  x = p~noresult
s3:
  say 'caught' rc condition('D')
  m = .mixed~new
  say m~hello m~fly
  u = .ghost~new
  say u~anything(1, 2)
  say u~other
  return

::requires 'shapes'

::class Counter
::method next class
  expose n
  if symbol('N') <> 'VAR' then n = 0
  n = n + 1
  return n

::class Point
::attribute x
::attribute y get
::method init
  expose x y
  use arg x, y
::method add
  expose x
  use arg d
  return x + d
::method private1 private
  return 'secret'
::method callprivate
  return 'via self:' self~private1
::method noresult
  nop

::class Flyer mixinclass Object
::method fly
  return 'flying'

::class Mixed subclass Object inherit Flyer
::method hello
  return 'hello'

::class Ghost
::method unknown
  use arg name, args
  return 'unknown' name 'with' args~items 'args'

::routine helper
  use arg how
  say 'helper called' how
  return 'helper result'

::routine greet
  return 'hi there'
