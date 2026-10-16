/* OO operators in places where they are still syntax errors */
call try 'x = a~'
call try 'x = a->'
call try 'x = -> b'
call try 'x = ->> b'
call try 'x = a - > b'
call try 'x = a -> > b'
call try 'x = a~1'
call try 'x = a['
call try 'x = a[1'
call try 'x = a]'
call try 'x = a [1]'
call try 'x = [1]'
call try '::class foo'
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
