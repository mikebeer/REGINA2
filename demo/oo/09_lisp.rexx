/* 09_lisp.rexx - LISP-style list processing
 *
 * ::requires 'lisp' gives the LISP functions: cons, car, cdr, list,
 * null, atom, append, mapcar, ... Lists are made of Cons cells and .nil
 * is the empty list. The program reads expressions written in LISP
 * notation, evaluates them, and shows recursive list functions.
 */
::requires 'lisp'

/* expressions in prefix notation, read from strings */
do e over .array~of('(+ 1 2 3)', '(* 2 (+ 3 4))', '(- 10 (/ 9 3) 1)', '(max 4 (* 2 5) 7)')
  x = read(e)
  say left(x, 22) '=' evaluate(x)
end

/* recursive list functions in the style of LISP */
l = list(5, 3, 8, 1, 9, 2)
say 'list     ' l
say 'sorted   ' qsort(l)
say 'sum      ' sum(l)
say 'squares  ' mapcar('square', l)
say 'evens    ' filter('even', l)
say 'flattened' flatten(read('(1 (2 (3 4)) ((5) 6))'))

/* an association list: a small phone book */
book = list(cons('alice', 4711), cons('bob', 1234), cons('carol', 5555))
say 'bob has  ' cdr(assoc('bob', book))
exit

evaluate: procedure            /* (op arg1 arg2 ...) or a number */
  use arg x
  if atom(x) then return x
  op = car(x)
  args = mapcar('evaluate', cdr(x))
  select
    when op = '+' then return reduce('plus', args, 0)
    when op = '*' then return reduce('times', args, 1)
    when op = '-' then return reduce('minus', args)
    when op = '/' then return reduce('divide', args)
    otherwise return apply(op, args)       /* any Rexx function: max, min, ... */
  end

qsort: procedure               /* quicksort on lists */
  use arg l
  if null(l) then return .nil
  p = car(l)
  smaller = .nil; larger = .nil
  do x over cdr(l)
    if x < p then smaller = cons(x, smaller)
    else larger = cons(x, larger)
  end
  return append(qsort(smaller), list(p), qsort(larger))

sum: procedure                 /* plain recursion */
  use arg l
  if null(l) then return 0
  return car(l) + sum(cdr(l))

flatten: procedure
  use arg x
  if null(x) then return .nil
  if atom(x) then return list(x)
  return append(flatten(car(x)), flatten(cdr(x)))

square: procedure; return arg(1) * arg(1)
even:   procedure; return arg(1) // 2 = 0
plus:   procedure; return arg(1) + arg(2)
times:  procedure; return arg(1) * arg(2)
minus:  procedure; return arg(1) - arg(2)
divide: procedure; return arg(1) / arg(2)
