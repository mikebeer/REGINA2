/* LISP-style lists: the lisp library and the Cons class */
::requires 'lisp'
l = list(1, 2, 3)
say 'list      ' l
say 'car/cdr   ' car(l) cdr(l) cadr(l) caddr(l) cddr(l)
say 'cons      ' cons(0, l) cons('a', 'b')
say 'null      ' null(l) null(.nil) null(cdr(list(1)))
say 'atom      ' atom('x') atom(l) consp(l) listp(.nil) listp('x')
say 'length    ' llength(l) llength(.nil)
say 'reverse   ' lreverse(l)
say 'append    ' append(l, list(4, 5), .nil, list(6))
say 'nth       ' nth(0, l) nth(2, l) nth(5, l)
say 'member    ' member(2, l) member(9, l)
al = list(cons('red', 1), cons('green', 2))
say 'assoc     ' assoc('green', al) cdr(assoc('green', al))
say 'mapcar    ' mapcar('square', l) mapcar('abs', list(-1, 2, -3))
say 'filter    ' filter('odd', list(1, 2, 3, 4, 5))
say 'reduce    ' reduce('add', l) reduce('add', l, 100) reduce('add', .nil, 0)
say 'apply     ' apply('max', list(3, 9, 4)) apply('add', list(2, 3))
say 'nested    ' list(1, list(2, list(3)), 'two words', '')
say 'read      ' read('(define (sq x) (* x x)) ; comment') read('(a . b)') read('"a ""q"""')
say 'equal     ' equal(list(1, list(2)), read('(1 (2))')) equal(l, list(1, 2)) eq(l, l) eq(l, list(1,2,3))
say 'flatten   ' flatten(read('(1 (2 (3 4)) ((5)))'))
say 'methods   ' l~length l~reverse l[2] l~second l~lastItem l~last l~map('square') l~makeArray~toString(, '+')
say 'nil       ' .nil~car .nil~length .nil~reverse .nil~map('square') .nil~append(l)
c = list(1, 2); call rplaca c, 'X'; call rplacd cdr(c), list(3); say 'rplac     ' c
do x over l; say 'over' x; end
do with index i item x over list('a', 'b'); say 'with' i x; end
q = list(1, 2, 3); call rplacd last(q), q
signal on syntax name circ
say llength(q)
circ: say 'circular  ' condition('D')
signal on syntax name rd
x = read('(1 2')
rd: say 'read error' condition('D')
signal on syntax name ca
x = car('atom')
ca: say 'car atom  ' condition('D')
exit
square: procedure; return arg(1) ** 2
odd: procedure; return arg(1) // 2
add: procedure; return arg(1) + arg(2)
flatten: procedure          /* recursive, LISP style */
  use arg x
  if null(x) then return .nil
  if atom(x) then return list(x)
  return append(flatten(car(x)), flatten(cdr(x)))
