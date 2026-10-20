/* Interval, Set, Bag and SortedCollection */
/* OOREGINA */
iv = 1~to(10)
say iv iv~size iv~first iv~last iv[3] iv~hasItem(7) iv~hasItem(11)
do i over 1~to(5); call charout , i' '; end; say
do i over 10~to(1, -3); call charout , i' '; end; say
say .interval~new(2, 20, 4) .interval~new(5, 1)~size 10~to(1, -2)~reverse
say 1~to(10)~select({ :x | x // 3 = 0 })~makestring('line', ' ')
say 1~to(5)~inject(1, { :p :x | p * x })
say 1~to(5)~collect({ :x | x * x })~makestring('line', ' ')
s = .set~of('a', 'b', 'a', 'c')
say s s~items s~hasItem('b') s~hasItem('z')
s~add('d'); s~remove('a')
say s
t = .set~of('c', 'd', 'e')
say s~union(t) '|' s~intersection(t) '|' s~difference(t) '|' .set~of('c')~isSubsetOf(s)
do x over s; call charout , x' '; end; say
b = .bag~of('x', 'y', 'x', 'x')
say b b~items b~occurrencesOf('x') b~occurrencesOf('q')
b~remove('x'); b~add('z', 3)
say b b~items
do x over b; call charout , x' '; end; say
say b~select({ :x | x \= 'z' }) b~collect({ :x | x || x })
words = .bag~new
do w over 'the cat and the dog and the bird'~makearray(' ')
  words~add(w)
end
say 'the:' words~occurrencesOf('the') 'and:' words~occurrencesOf('and')
sc = .sortedCollection~new
sc~add(5); sc~add(1); sc~add(3); sc~add(10); sc~add(2)
say sc~makestring('line', ' ') sc~first sc~last sc[2] sc~class
by_len = .sortedCollection~new({ :a :b | length(a) <= length(b) })
do w over .array~of('banana', 'fig', 'apple', 'kiwi'); by_len~add(w); end
say by_len~makestring('line', ' ')
say by_len~select({ :w | length(w) > 3 })~class by_len~collect({ :w | length(w) })~class
signal on syntax
by_len[1] = 'x'
exit
syntax: say condition('d')
