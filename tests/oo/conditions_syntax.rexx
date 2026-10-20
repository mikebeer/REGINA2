o = .thing~new
do i = 1 to 3
  signal on syntax name trap
  say o~boom(i)
  back:
end
say 'after loop' o~ok
say o~boomlabel
exit
trap:
  say 'caught' rc 'line' sigl condition('D')
  say 'still works:' o~ok
  if i < 3 then do; i = i + 1; signal back2; end
  say 'done traps' o~ok
  exit
back2:
  say 'resumed at i='i o~ok
  signal on syntax name trap
  say o~boom(i)
  exit
::class thing
::method boom
  use arg n
  x = 1 / 0
  return x
::method ok
  return 'ok'
::method boomlabel
  signal on syntax name inner
  y = 'a' + 1
  return 'no'
inner:
  return 'inner caught' rc
