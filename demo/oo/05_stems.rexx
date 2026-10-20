/* 05_stems.rexx - stems by reference, stems as results, Stem objects
 *
 * Classic Rexx passes a stem as the string "NAME.", so routines usually
 * EXPOSE global stems. With the OO extension, USE ARG s. binds the
 * caller's stem directly, and RETURN x. gives a whole stem back.
 * The marker below switches the OO keywords on for this file.
 */
/* OOREGINA */
names.0 = 4
names.1 = 'Wilma'; names.2 = 'Fred'; names.3 = 'Barney'; names.4 = 'Betty'

call sortstem names.                   /* sorted in place */
do i = 1 to names.0
  say i names.i
end

long. = filter(names., 5)              /* a new stem comes back */
say long.0 'names longer than 5:' long.1

call addtotals names., 'COUNT'     /* tails are case sensitive */
say 'added by the routine:' names.count

/* a returned stem kept in a variable is a Stem object */
s = filter(names., 0)
say 'items' s~items '- first' s[1] '- tails' s~allIndexes~toString(, ' ')
do with index t item v over s
  say '  'left(t, 6) v
end
exit

sortstem: procedure                    /* a simple insertion sort */
  use arg s.                           /* s. is the caller's stem */
  do i = 2 to s.0
    v = s.i
    do j = i - 1 to 1 by -1 while s.j > v
      k = j + 1; s.k = s.j
    end
    k = j + 1; s.k = v
  end
  return

filter: procedure                      /* names longer than min */
  use arg in., min
  out.0 = 0
  do i = 1 to in.0
    if length(in.i) > min then do
      n = out.0 + 1; out.n = in.i; out.0 = n
    end
  end
  return out.

addtotals: procedure
  use arg s., tail
  s.tail = s.0
  return
