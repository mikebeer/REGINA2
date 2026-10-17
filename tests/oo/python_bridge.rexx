/* the Python bridge: .Python and .PyObject */
::requires 'python'

say '--- modules and functions'
math = .python~import('math')
say math~sqrt(16) math~floor(2.7) math~pi
say math~SQRT(2)                          /* names are found in any case */
say .python~len('abc') .python~max(3, 9, 4) .python~abs(-5)

say '--- values coming back'
say .python~eval('None') == .nil          /* None is .nil */
say .python~eval('True') .python~eval('1 < 0')
say .python~eval('2 ** 70')
say .python~eval('1 / 4') .python~eval('"text"')
l = .python~eval('[1, "two", 3.5]')
say l~pytype l~string .python~len(l)

say '--- values going to Python'
say .python~repr(42) .python~repr(4.5) .python~repr('4 5') .python~repr(.nil)
say .python~repr(.python~str('007')) .python~repr(.array~of(1, 'a', .array~of(2)))
d = .directory~new; d['k'] = 'v'
say .python~repr(d)
m = .array~new(2, 2); m[1,1] = 1; m[1,2] = 2; m[2,1] = 3; m[2,2] = 4
say .python~repr(m)
s.1 = 'one'; s.2 = 'two'; s.x = 'ex'
say .python~repr(s.)

say '--- objects'
sh = .python~import('pyshapes')
r = sh~Rect(3, 4)
say r r~area r~w
r~w = 5
say r~area r~getAttr('h') r~scaled~area r~scaled(.python~kw('f', 3))
say sh~describe(1, 2, .python~kw('z', 26, 'a', 1))
r~setAttr('h', 10); say r

say '--- items and iteration'
lst = .python~list(.array~of(30, 10, 20))
lst~append(5); lst~invoke('sort')
say lst lst~at(0) lst~at(-1)
lst~put(99, 0); say lst
do x over lst
  say '  item' x
end
dct = .python~dict
dct~put('Mike', 'name'); dct~put(60, 'age')
say dct dct~at('name')
do k over dct
  say '  key' k
end
say dct~toRexx~class dct~toRexx['name']
n = .python~eval('[1, [2, 3], {"a": (4, 5)}]')~toRexx
say n~class n[2][2] n[3]['a'][1]
cnt = .python~eval('[10, 20, 30]')~toStem(st.)
say cnt st.0 st.1 st.3
.python~eval('{"city": "Bern", "zip": 3000}')~toStem(addr.)
say addr.city addr.zip

say '--- operators'
a = .python~eval('[1, 2]')
say a + a  a * 2  (a = .python~eval('[1, 2]'))  (a \= a)
f = .python~eval('7.5')                   /* a float comes back as string */
say f + 1
fr = .python~import('fractions')
h = fr~Fraction(1, 2)
say h + fr~Fraction(1, 3)  h * 3  h ** 2  (h < 1)  (-h)  h % fr~Fraction(1, 3)

say '--- exec and eval'
.python~exec('def twice(v):' || '0a'x || '    return v * 2')
say .python~twice(21) .python~twice('ab')
v = .directory~new; v['a'] = 6; v['b'] = 7
say .python~eval('a * b', v)
say .python~eval('len(s)', .python~eval('{"s": "hello"}'))
json = .python~import('json')
say json~dumps(.python~eval('{"b": [1, 2], "a": None}'), .python~kw('sort_keys', 1))

say '--- errors'
signal on syntax name e1
say math~sqrt(-1)
e1: say 'caught' rc condition('d')
signal on syntax name e3
say .python~import('no_such_module')
e3: say 'caught' condition('d')
signal on syntax name e4
say r~nonexistent
e4: say 'caught' condition('d')
signal on syntax name e5
.python~exec('1 +')
e5: say 'caught' condition('d')
say 'done'
