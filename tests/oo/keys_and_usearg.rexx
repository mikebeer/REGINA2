use arg first
say 'use arg in main ->' symbol('FIRST')
a = .array~of(1)
b = .array~of(2)
s.a = 'A'; s.b = 'B'
say s.a s.b
t = .table~new
t[a] = 'ta'; t[b] = 'tb'
say t[a] t[b] t~items
d = .directory~new
d[a] = 'da'; d[b] = 'db'
say d[a] d~items
k = d~allindexes
say k[1]
do key over t
  say 'table key class' key~class~id t[key]
end
stem.1 = a
say stem.1~items
q = 'x'
call r1 a
say 'r1 result class' result~class~id
exit
r1: return arg(1)
::class nothing
::method expose_test
  expose = 5
  return expose
