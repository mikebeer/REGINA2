/* OO syntax that now parses. Until the object system exists, running a
 * message term or array reference raises error 91.1. Each case is run
 * through INTERPRET so one failure does not stop the test. */
a = 'abc'; b = 0; i = 1
call try 'x = a~b'
call try 'x = a~b()'
call try 'x = a~b(1, 2)'
call try 'x = a->b'
call try 'x = a -> b'
call try 'x = a -> b(1)'
call try 'x = a~~b'
call try 'x = a->>b'
call try 'x = a ->> b(1) ->> c'
call try 'x = "str"~length'
call try "x = 'str' -> length"
call try 'x = (1+2)~abs'
call try 'x = a~b~c'
call try 'x = a->b->c'
call try 'x = a~b + 1'
call try 'x = -a~b'
call try 'x = \a~b'
call try 'x = a~ b'
call try 'x = a~"+"(1)'
call try 'x = a[1]'
call try 'x = a[1, 2]'
call try 'x = a[i+1]'
call try 'x = a~b[1]'
call try 'x = a[1]~b'
call try 'x = a[1][2]'
call try 'x = a[]'
call try 'a~b'
call try 'a -> b(1)'
call try 'say a~b'
/* classic meanings that must not change */
call try 'say a ~b'
call try 'say a~=b a~ =b a~<b'
call try 'say ~b ~ b'
call try 'say~b'
call try 'if~b then say "if-not ok"'
exit

try:
  parse arg code
  signal on syntax name caught
  interpret code
  say '  ok:' code
  return
caught:
  say 'err' rc':' code '=>' condition('D')
  return
