/* NetRexx classes from a .nrx file, used from a Regina OO program */
v = .vec~new(1, 2)~plus(.vec~new(10, 20))
say v~toString v~x v~y
w = .Vec(3, 4).plus(v)                 -- the CREXX/NetRexx notation, too
say w.toString()
::requires 'geometry.nrx'
