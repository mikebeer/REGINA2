/* USE, ARG, EXPOSE, RAISE are ordinary symbols outside OO code */
use = 'U'; arg = 'A'; raise = 'R'; forward = 'F'; guard = 'G'
say use arg raise forward guard
call sub 7
exit
sub: use arg x
  say 'sub' arg(1)
  return
