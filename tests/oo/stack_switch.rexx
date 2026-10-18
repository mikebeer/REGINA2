/* OOREGINA - --stack=64M (stack_switch.switches) allows deep nesting; --no-gc
   keeps REGINA_GC=stress from scanning 20000 levels before every clause */
say 'depth reached:' .rec~new~down(20000)
signal on syntax name full
say .rec~new~down(100000000)
exit
full:
say 'very deep:' condition('D')
exit
::class rec
::method down
  use arg n
  if n = 0 then return 'all'
  r = self~down(n - 1)
  if r == 'all' then return 20000
  return r
