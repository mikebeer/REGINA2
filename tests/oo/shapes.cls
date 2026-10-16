/* a required file with its own prolog */
say 'loading shapes.cls'
call setup
exit
setup:
  say 'shapes prolog label works'
  return

::class Shape public
::method init
  expose name
  use arg name
::method name
  expose name
  return name
::method area abstract
::method describe
  return self~name 'with area' self~area

::class Circle subclass Shape public
::constant pi 3.14159
::method init
  expose r
  use arg r, name
  self~init:super(name)
::method area
  expose r
  return self~pi * r * r
