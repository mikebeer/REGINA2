/* The main program ends at the first directive (implicit RETURN) */
say 'main runs'
say 'last line of main'

::class Account subclass Object
::method init
  say 'never printed (method body)'
::attribute balance get
::method "+"
::constant pi 3.14159
::routine helper public
  say 'never printed (routine body)'
