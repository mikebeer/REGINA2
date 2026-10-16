/* A subroutine that runs into a directive returns without a value */
call sub
say 'result set?' symbol('RESULT')
exit

sub:
  say 'in sub'
::routine r
  say 'not reached'
