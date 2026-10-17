/* run with --no-oo: everything as in Regina 3.9.7, markers ignored */
/* OOREGINA */
options ooregina
say 'nil is' .nil
a = 1; b = 0
say 'a ~b' a ~b
x = ''; over = ''
r = 0; do x over 2; r = r + 1; end; say 'do x over 2:' r
loop = 'a variable'; say loop
counter = 2; r = 0; do counter; r = r + 1; end; say 'do counter:' r
call t 'x = a~b'
call t 'x = a->b'
call t 'x = a[1]'
call t '::class foo'
call t 'do with index i item v over a; end'
exit
t: parse arg code
  signal on syntax name bad
  interpret code
  say 'ran' code
  return
bad: say 'error' rc'.'condition('E') code
  return
