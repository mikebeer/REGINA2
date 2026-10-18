/* OOREGINA - workspace functions: []NL []FX []SAVE []CLEAR []LOAD []OUT []IN []EX []CR */
call []WSID 'demo'
x = 42; name = 'Mike''s'; bin = '00ff'x
s. = 'default'; s.1 = 'one'; s.2 = 'two'; s.0 = 2
a = .array~of(1, 'two', .directory~new)
a[3]['k'] = 'v'
a[4] = a                                   /* a cycle */
m = .array~new(2, 2); m[1,2] = 'x'
p = .point~new(3, 4)
st = .set~of('a', 'b'); bg = .bag~of('x', 'x', 'y')
iv = 1~to(9, 2)
l = .cons~of(1, 2, 3)
sq = { :n | n * n }
sc = .sortedCollection~new({ :a :b | a >= b }); sc~add(1); sc~add(5); sc~add(3)
say []FX('::routine double', '  return arg(1) * 2')
say double(21)
say 'NL 2:' []NL(2)
say 'NL 3:' []NL(3)
say 'NL 9:' []NL(9)
say 'saved to' []SAVE()
say []CLEAR() 'after clear:' '['[]NL()']' []WSID()
say 'loaded:' []LOAD('demo') 'wsid:' []WSID()
say x name c2x(bin) s.1 s.2 s.0 s.xyz
say a~items a[2] a[3]['k'] (a[4] == a) m[1,2] m~dimension
say p~x p~y p~class st~items st~hasItem('b') bg~occurrencesOf('x') iv iv~last
say l sq~call(7) sc~makestring('line', ' ')
sc~add(4); say sc~makestring('line', ' ')
say double(4) []CR('double')
say 'out:' []OUT('part', 'x s.')
say []EX('x s. double nothere')
say '['[]NL(2)']' 
say 'in:' []IN('part')
say x s.2 s.xyz
exit
::class point
::method init
  expose x y
  use arg x, y
::attribute x
::attribute y
