/* stems by reference: USE ARG s., RETURN x., Stem objects */
/* OOREGINA */
a.0 = 3; a.1 = 1; a.2 = 2; a.3 = 3
b.0 = 2; b.1 = 10; b.2 = 20
say 'sum a' sum(a.) 'sum b' sum(b.) 'sum a again' sum(a.)
call double a.; call double b.
say 'doubled' a.1 a.2 a.3 '/' b.1 b.2
call noproc a.
say 'noproc' a.4
call deep b., 3
say 'deep' b.deep
x. = 'dflt'; x.1 = 'one'
r. = 'old'; r.7 = 'gone'
r. = copyback()
say 'copy' r.1 r.2 r.7 r.zz
q. = nodefault()
say 'nodefault' q.1 q.9
call plain 'just a string'
call classic a.
o = .acc~new
o~fill(a.)
say 'method' a.0 a.4 a.5
s = copyback()
say 'object' s~items s[1] s['ZZ'] s~hasindex(1) s~hasindex(5)
do t over s; say 'over' t s[t]; end
do with index t item v over s; say 'with' t v; end
r2. = s
say 'from object' r2.2
st = .stem~new('none'); st[1] = 'x'; st['a', 'b'] = 'ab'
say 'new stem' st[1] st['A.B'] st[2] st~items st~allindexes~toString(, ',')
call viaobj st
say 'via object' st[9]
cs = .stem~new; cs['a'] = 'low'; cs['A'] = 'up'
say 'case' cs['a'] cs['A'] cs~items
say 'string value' f5() g5()
exit

f5: procedure
  y.1 = 1
  return y.
g5: procedure
  z. = 'zdef'
  return z.

sum: procedure
  use arg s.
  t = 0
  do i = 1 to s.0; t = t + s.i; end
  return t
double: procedure
  use arg s.
  do i = 1 to s.0; s.i = s.i * 2; end
  return
noproc:
  use arg z.
  z.4 = 'four'
  return
deep: procedure
  use arg s., n
  if n > 0 then call deep s., n - 1
  else s.deep = 'reached'
  return
copyback: procedure
  y. = 'dflt'; y.1 = 'one'; y.2 = 'two'
  return y.
nodefault: procedure
  w.1 = 'only'
  return w.
plain: procedure
  use arg p.
  say 'plain' p.1 p.anything
  return
classic: procedure
  parse arg x
  say 'classic sees' x arg(1)
  return
viaobj: procedure
  use arg m.
  m.9 = 'set via alias'
  return
::class acc
::method fill
  use arg s.
  s.4 = 'from method'; s.5 = s.0
