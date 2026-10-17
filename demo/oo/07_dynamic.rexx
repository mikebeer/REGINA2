/* 07_dynamic.rexx - UNKNOWN, mixins, messages chosen at run time
 *
 * A logging proxy forwards every message it doesn't know to another
 * object (UNKNOWN). A mixin adds the same methods to unrelated classes
 * (INHERIT). SEND and SENDWITH send a message whose name is in a variable.
 */
list = .array~new
log = .logger~new(list)                /* the proxy wraps the Array */
log~append('first')
log~append('second')
say 'proxied array has' log~items 'items:' log~toString(, ', ')

/* a mixin: Printable adds PRINT to two unrelated classes */
.point~new(3, 4)~print
.person~new('Ada', 36)~print
say 'Person has PRINT:' .person~new('x', 0)~hasMethod('print')
say 'Printable is a mixin:' .printable~queryMixinClass
say 'Person inherits it:' .person~isSubclassOf(.printable)

/* message names chosen at run time */
p = .point~new(6, 8)
do m over .array~of('x', 'y', 'length')
  say 'p~'m '=' p~send(m)
end
say 'scaled:' p~sendWith('scale', .array~of(10))
exit

::class Logger
::method init
  expose target
  use arg target
::method unknown                       /* any message not defined here */
  expose target
  use arg name, args
  say '  [log]' name 'with' args~items 'argument(s)'
  return target~sendWith(name, args)

::class Printable mixinclass Object
::method print
  say self~class~id':' self~describe

::class Point inherit Printable
::method init
  expose x y
  use arg x, y
::method x;      expose x; return x
::method y;      expose y; return y
::method length; expose x y; return sqrt(x * x + y * y)
::method scale
  expose x y
  use arg k
  return .point~new(x * k, y * k)
::method describe
  expose x y
  return '(' || x || ', ' || y || ')'   /* not '('x: that is a hex string */
::method string
  return self~describe

::class Person inherit Printable
::method init
  expose name age
  use arg name, age
::method describe
  expose name age
  return name 'aged' age

::routine sqrt                         /* Newton's method */
  use arg n
  if n = 0 then return 0
  r = n
  do 20
    r = (r + n / r) / 2
  end
  return format(r, , 4) / 1            /* / 1 drops trailing zeros */
