/* 02_inheritance.rexx - inheritance, polymorphism, ::requires
 *
 * The classes live in shapes.cls. Every shape answers AREA in its own
 * way; the loop doesn't care which kind of shape it has.
 */
::requires 'shapes.cls'                 /* load the class library */

shapes = .array~of(.circle~new(1), .rectangle~new(2, 3), .square~new(4))
total = 0
do s over shapes
  say s~describe
  total = total + s~area
end
say 'total area' format(total, , 2)

sq = shapes[3]
say 'a square is a rectangle:' sq~isA(.rectangle)
say 'and a shape:' sq~isA(.shape) ' class:' sq~class~id
say 'superclass of Square:' .square~superclass~id

signal on syntax name abstract
x = .shape~new('blob')~area             /* Shape doesn't implement AREA */
exit
abstract:
  say 'as expected:' condition('D')
