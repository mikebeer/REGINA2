/* run-time errors of the object system, each caught with SIGNAL ON SYNTAX */
call try 'x = .array~new~nosuchmethod'
call try 'x = "abc"~nosuchmethod(1)'
call try 'x = .counter~new~nothing'
call try 'x = .array~new[0]'
call try 'x = .array~new[1, 2]'
call try 'x = .array~of(1)~section("x")'
call try 'x = .object~new~isa("notaclass")'
call try 'x = .shape~new~area'
call try 'do x over "plain string"; end'
call try 'x = .flyer~new'
exit
try:
  parse arg code
  signal on syntax name failed
  interpret code
  say '  ok:' code
  return
failed:
  say left(code, 36) '->' condition('D')
  return

::class Counter
::method nothing
  nop
::class Shape
::method area abstract
::class Flyer mixinclass Object
