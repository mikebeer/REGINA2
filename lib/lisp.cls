/* lisp.cls - LISP-style list functions for ooRegina
 *
 * ::requires 'lisp' loads this library. It is built into the
 * interpreter, so no file is needed; a file named lisp.cls next to the
 * program or in REGINA_MACROS takes precedence.
 *
 * Lists are made of Cons cells; .nil is the empty list. The functions
 * call the methods of the Cons class. Names that are Rexx built-in
 * functions get an L in front: LLENGTH, LREVERSE.
 */
::routine cons public           /* (cons a d) */
  use arg a, d
  return .cons~new(a, d)
::routine car public
  use arg x
  return x~car
::routine cdr public
  use arg x
  return x~cdr
::routine caar public
  use arg x
  return x~car~car
::routine cadr public
  use arg x
  return x~cdr~car
::routine cdar public
  use arg x
  return x~car~cdr
::routine cddr public
  use arg x
  return x~cdr~cdr
::routine caddr public
  use arg x
  return x~cdr~cdr~car
::routine list public           /* (list a b c) */
  r = .nil
  do i = arg() to 1 by -1
    r = .cons~new(arg(i), r)
  end
  return r
::routine null public           /* 1 for the empty list */
  use arg x
  return x~isNil
::routine atom public           /* 1 for anything but a cell */
  use arg x
  return \x~isA(.cons)
::routine consp public
  use arg x
  return x~isA(.cons)
::routine listp public
  use arg x
  return x~isNil | x~isA(.cons)
::routine nth public            /* (nth n l), 0-based as in LISP */
  use arg n, l
  return l~nthcdr(n)~car
::routine nthcdr public
  use arg n, l
  return l~nthcdr(n)
::routine llength public        /* LENGTH is a Rexx built-in function */
  use arg l
  return l~length
::routine lreverse public       /* REVERSE is a Rexx built-in function */
  use arg l
  return l~reverse
::routine last public           /* the last cell */
  use arg l
  return l~last
::routine append public         /* (append l1 l2 ...) */
  if arg() = 0 then return .nil
  r = arg(arg())
  do i = arg() - 1 to 1 by -1
    r = arg(i)~append(r)
  end
  return r
::routine member public         /* (member x l) */
  use arg x, l
  return l~member(x)
::routine assoc public          /* (assoc key alist) */
  use arg k, al
  return al~assoc(k)
::routine mapcar public         /* (mapcar 'f l) */
  use arg f, l
  return l~map(f)
::routine filter public         /* the items for which f returns 1 */
  use arg f, l
  return l~filter(f)
::routine reduce public         /* (reduce 'f l [initial]) */
  use arg f, l, init
  if arg(3, 'E') then return l~reduce(f, init)
  return l~reduce(f)
::routine apply public          /* (apply 'f l) */
  use arg f, l
  return .cons~apply(f, l)
::routine equal public          /* the same structure and atoms */
  use arg a, b
  if a~isA(.cons) then return a~equal(b)
  return a == b
::routine eq public             /* the same object, or equal atoms */
  use arg a, b
  return a == b
::routine rplaca public         /* changes the car of a cell */
  use arg c, x
  c~setCar(x)
  return c
::routine rplacd public         /* changes the cdr of a cell */
  use arg c, x
  c~setCdr(x)
  return c
::routine read public           /* the list written in a string */
  use arg s
  return .cons~read(s)
