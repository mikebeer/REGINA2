/* run with --oo: the OO keywords work without any marker */
call show 'a', 'b'
loop counter n i = 1 to 3; end
say 'loop ran' n 'times'
say 'nil is' .nil
interpret 'loop label l j = 1 to 2; end l'
say 'interpret j' j
exit
show: procedure
  use arg x, y
  say 'use arg:' x y
  return
