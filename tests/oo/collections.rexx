/* collections */
a = .array~new
a~append('x'); a~append('y'); a[5] = 'z'
say a~size a~items a~first a~last a~firstitem a~lastitem a~hasindex(3) a~hasindex(5)
say a~makearray~items a~allindexes~tostring(, ',') a~index('y')
say a~remove(1) a~items a[1]~isnil
b = .array~of('pear', 'apple', 'fig')
b~sort
say b~tostring('C', ' ')
say b~tostring
s = .array~of(1,2,3)~section(2)
say s~items s[1]
q = .queue~new
q~queue('a'); q~queue('b'); q~push('first')
say q~items q~peek q~pull q~pull q~items
l = .list~of('one', 'three')
l~insert('two', 1)
say l~tostring('C', '-') l~items
t = .table~new
k = .array~new
t[k] = 'by object'
t['k'] = 'by string'
say t[k] t['k'] t~items
d = .directory~new
d~setentry('Name', 'Mike')
d['city'] = 'Zurich'
say d~entry('name') d~name d['city'] d~hasentry('NAME') d~hasindex('nope')
do key over d
  say 'key' key '=' d[key]
end
sup = d~supplier
do while sup~available
  say 'sup' sup~index sup~item
  sup~next
end
st.1 = 'a'; st.2 = 'b'; st.10 = 'c'; st.x = 'd'
do t over st.
  say 'tail' t st.t
end
n = 0
do v over .array~of(1, 2, 3, 4) while v < 4
  n = n + v
end
say 'sum' n
do v over .array~of(1, 2, 3)
  if v = 2 then iterate v
  say 'v' v
end
do v over .array~of(1, 2, 3)
  if v = 2 then leave v
  say 'w' v
end
mb = .mutablebuffer~new('abc')
mb~append('def', 'gh')~append('!')
say mb mb~length mb~substr(2, 3) mb~pos('d')
mb~insert('XY', 1)~delete(5, 2)
say mb
/* strings */
say 'hello'~upper 'HeLLo'~lower 'abc'~length 'a b c'~words
say 'hello'~substr(2,3) 'hello'~pos('l') 'hello'~lastpos('l') 'a-b-c'~changestr('-', '+')
say 'abc'~"||"('def') 3~"+"(4) 10~"//"(3) 'a'~"="('a') 'a'~"=="(' a') 5~"-"
say 'abcdef'~startsWith('abc') 'abcdef'~endsWith('def') 'abc'~contains('x') 'ABC'~caselessEquals('abc')
parts = 'a,b,,c'~makearray(',')
say parts~items parts[3]~length
lines = ('line1' || '0a'x || 'line2')~makearray
say lines~items lines[2]
/* objects in stems, arguments, concatenation, comparison */
o.1 = .array~of(1); o.2 = .directory~new
say o.1~items o.2~items 'concat:' o.1 || '!' 'space:' o.1 o.2
same = o.1
say (same == o.1) (same = o.1) (o.1 == o.2) (o.1 \== o.2)
call show o.1
say length(o.1) left(o.2, 5)
say .environment~nil .environment['ARRAY'] .true .false
x = .object~new
x~objectname = 'my thing'
say x x~objectname
exit
show: procedure
  parse arg obj
  say 'show got' obj~class~id obj~items
  return
