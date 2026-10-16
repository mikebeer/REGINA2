signal on novalue
o = .thing~new
say o~greet
say o~greet('Bob')
exit
novalue: say 'NOVALUE in main' condition('D') 'line' sigl; exit 1
::class thing
::method greet
  use arg who
  if symbol('WHO') \== 'VAR' then who = 'nobody'
  return 'hi' who x
