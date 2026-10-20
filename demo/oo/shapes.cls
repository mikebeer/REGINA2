/* shapes.cls - a small class library, loaded with ::requires 'shapes.cls'
 * Shape is abstract: every subclass must implement AREA.
 */
::class Shape public
::attribute name get
::method init
  expose name
  use arg name
::method area abstract
::method describe
  return left(self~name, 10) 'area' format(self~area, , 2)

::class Circle subclass Shape public
::constant pi 3.14159265
::method init
  expose r
  use arg r
  self~init:super('circle')          /* run Shape's INIT too */
::method area
  expose r
  return self~pi * r * r

::class Rectangle subclass Shape public
::method init
  expose w h
  use arg w, h, name
  if arg(3, 'O') then name = 'rectangle'
  self~init:super(name)
::method area
  expose w h
  return w * h

::class Square subclass Rectangle public
::method init
  use arg side
  self~init:super(side, side, 'square')
