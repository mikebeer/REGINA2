/* 03_collections.rexx - Array, Directory, Queue, Table, Supplier
 *
 * Counts the words of a text with a Directory, sorts them with an Array,
 * walks a queue of jobs and keeps objects as keys in a Table.
 */
text = 'the quick brown fox jumps over the lazy dog the fox sleeps'

/* word frequencies: Directory index -> count */
counts = .directory~new
do w over text~makeArray(' ')          /* a string split into an Array */
  if counts~hasIndex(w) then counts[w] = counts[w] + 1
  else counts[w] = 1
end
say counts~items 'different words'

/* sort "count word" pairs; right() makes the counts sort as text */
pairs = .array~new
do with index w item n over counts
  pairs~append(right(n, 3) w)
end
pairs~sort
say 'most frequent:'
do i = pairs~items to pairs~items - 2 by -1
  parse value pairs[i] with n w
  say '  ' left(w, 6) n
end

/* a queue of jobs: first in, first out */
jobs = .queue~new
jobs~queue('print report')
jobs~queue('send mail')
jobs~push('urgent: backup')            /* PUSH puts it in front */
do while \jobs~isEmpty
  say 'doing' jobs~pull
end

/* a Table accepts objects as indexes */
alice = .directory~new; alice['name'] = 'Alice'
bob = .directory~new;   bob['name'] = 'Bob'
manager = .table~new
manager[bob] = alice
say bob['name'] 'reports to' manager[bob]['name']

/* a Supplier walks index/item pairs */
s = .array~of('red', 'green', 'blue')~supplier
do while s~available
  say s~index':' s~item
  s~next
end
