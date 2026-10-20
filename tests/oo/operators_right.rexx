/* OOREGINA */  /* objects as right operands use their string value */
o = .fr~new
say 1 + .fr~new  '|' 1 + o '|' 2 * (o) '|' (1 = o) (41 = o) ('41' == o) (o = 41) (40 < o)
say 'abc' || o 'abc' o
signal on syntax
x = 3 + .array~new
exit
syntax: say 'error' rc condition('D')
exit
::class fr
::method string
 return 41
::method "+"
 return "plus called"
