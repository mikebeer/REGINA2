/* 11_blocks.rexx - blocks and the collection methods
 *
 * A block { ... } is a small routine written where it is used.
 * { :x | expression } takes the parameter x and returns the expression;
 * { use arg x; ... } is the long form. Blocks see the variables of the
 * code that created it, and keep them when that code has returned.
 * Collections answer do, select, reject, collect, detect, inject,
 * anySatisfy, allSatisfy, count and (Arrays) sortWith.
 */
books = .array~new
call book 'Dune', 'Herbert', 1965, 412
call book 'Neuromancer', 'Gibson', 1984, 271
call book 'Foundation', 'Asimov', 1951, 255
call book 'Hyperion', 'Simmons', 1989, 482
call book 'Solaris', 'Lem', 1961, 204

say 'Before 1970:'
books~select({ :b | b['year'] < 1970 })~do({ use arg b; call show b })

say 'Titles:    ' books~collect({ :b | b['title'] })~makestring('line', ', ')
say 'Pages:     ' books~inject(0, { :sum :b | sum + b['pages'] })
say 'Long ones: ' books~count({ :b | b['pages'] > 400 })
say 'Any by Lem?' books~anySatisfy({ :b | b['author'] = 'Lem' })
first = books~detect({ :b | b['year'] > 1980 })
say 'First after 1980:' first['title']

limit = 260                                  /* blocks see this variable */
short = books~reject({ :b | b['pages'] > limit })
say 'Short:     ' short~collect({ :b | b['title'] })~makestring('line', ', ')

say 'By year:'
books~sortWith({ :a :b | a['year'] <= b['year'] })~do({ use arg b; call show b })

/* a block that keeps its own state: a counter made by a routine */
next = counter(100)
say 'Tickets:   ' next~call next~call next~call

/* the long form, with several clauses */
stats = { use arg list
          total = list~inject(0, { :s :x | s + x })
          return 'n='list~items 'mean='format(total / list~items, , 1) }
say 'Stats:     ' stats~call(books~collect({ :b | b['pages'] }))
exit

book: procedure expose books
  d = .directory~new
  d['title'] = arg(1); d['author'] = arg(2); d['year'] = arg(3); d['pages'] = arg(4)
  books~append(d)
  return

show: procedure
  use arg b
  say '  ' left(b['title'], 12) left(b['author'], 8) b['year']
  return

counter: procedure
  use arg n
  return { n = n + 1; return n }
