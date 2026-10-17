::requires 'shapes.cls'       /* at the top, like #include in C */
/* ::REQUIRES and ::OPTIONS don't end the main program */
::options
say 'main starts' .circle~new(1)~describe
call helper
::requires 'lib1.cls'         /* in the middle of the main program */
say 'main continues'
o = .thing~new
o~run
exit
helper:
  say 'label in the main program found'
  return
::class Thing
::method run
  say 'method part 1'
::requires 'lib2.cls'         /* between statements of a method */
  say 'method part 2'
