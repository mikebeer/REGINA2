/* regressions found by the code review */
o = .array~new
k = 'K'; s.k = 5
if s.k > 2 then say 'compound comparison ok'
say 'nested BIF in STRING:' length(.p~new) left(.p~new, 6, '*')
a = .v~new(5)
say 'operand changed by right side:' a + seta()
f = .array~of('xxxx', 'y', 'z')
f~sendwith('FILL', f)
say 'sendwith fill:' f~tostring('C', ',')
c = .holder~new
c~put('k', 'v')
d = c~copy
d~put('k2', 'v2')
say 'copy keeps stems:' c~count d~count
x = .props~new
x~set(1) = 'one'
say 'indexed attribute assignment:' x~set(1)
say 'STRING using self:' left(.selfish~new, 12)
signal on syntax name bigidx
b = .array~new
b[2000000000] = 1
bigidx:
say 'big index:' condition('D')
mb = .mutablebuffer~new('ab')
mb~insert(.array~new, 1)
say 'buffer insert object:' mb
signal on syntax name deep
say .rec~new~down(1000000)
deep:
say 'deep recursion:' condition('D')
exit

seta: a = 'zz'; return 3

::class p
::method string
  return 'P' || length('ab')
::class v
::method init
  expose n
  use arg n
::method "+"
  expose n
  return n + arg(1)
::class holder
::method init
  expose st. n
  st. = ''
  n = 0
::method put
  expose st. n
  use arg key, val
  n = n + 1
  st.n = key '=' val
::method count
  expose n
  return n
::class props
::method "SET="
  expose s.
  use arg value, i
  s.i = value
::method set
  expose s.
  use arg i
  return s.i
::class selfish
::method string
  return 'S:' self
::class rec
::method down
  use arg n
  if n = 0 then return 0
  return self~down(n - 1)
