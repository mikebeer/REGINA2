/* Python calls Rexx blocks: key=, map, filter, reduce; errors and EXIT */
::requires 'python'
words = .array~of('banana', 'Fig', 'apple', 'kiwi')
say .python~sorted(words)
say .python~sorted(words, .python~kw('key', { :w | length(w) }))
say .python~sorted(words, .python~kw('key', { :w | translate(w) }, 'reverse', 1))
ft = .python~import('functools')
say ft~reduce({ :a :b | a * b }, .array~of(1, 2, 3, 4, 5))
say .python~list(.python~map({ :x | x * 10 }, .array~of(1, 2, 3)))
say .python~list(.python~filter({ :x | x // 2 }, 1~to(9)~makearray))
count = 0
.python~list(.python~map({ use arg x; count = count + x }, .array~of(5, 6)))
say 'count' count
.python~exec('def twice(f, x):' '0a'x '    return f(f(x))')
say .python~twice({ :s | s || '!' }, 'hi')
signal on syntax
say .python~sorted(words, .python~kw('key', { :w | 1 / 0 }))
exit
syntax: say 'caught' condition('d')
say 'after'
call more
exit
more:
say .python~twice({ :s | s '+' }, 'x')
.python~twice({ exit 7 }, 1)
say 'not reached'
