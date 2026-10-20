/* blocks { ... } and the collection methods do, select, reject, collect, ... */
double = { use arg x; return x * 2 }
say double~call(21) double
factor = 3
triple = { :x | x * factor }
say triple~value(5)
factor = 10
say triple~value(5)
a = .array~of(5, 3, 8, 1, 9, 2)
say a~select({ :x | x > 4 })~makestring('line', ' ')
say a~reject({ :x | x > 4 })~makestring('line', ' ')
say a~collect({ :x | x * x })~makestring('line', ' ')
say a~detect({ :x | x > 7 }) a~detect({ :x | x > 70 }) a~detect({ :x | x > 70 }, 'none')
say a~inject(0, { :acc :x | acc + x })
say a~anySatisfy({ :x | x > 8 }) a~allSatisfy({ :x | x > 0 }) a~count({ :x | x // 2 = 0 })
a~do({ use arg x, i; call charout , i || '=' || x' ' })
say
sum = 0
a~each({ use arg x; sum = sum + x })
say 'sum' sum
say a~sortWith({ :p :q | p <= q })~makestring('line', ' ')
say a~sortWith({ :p :q | p >= q })~makestring('line', ' ')
w = .array~of('pear', 'fig', 'banana', 'kiwi')
say w~sortWith({ :p :q | length(p) <= length(q) })~makestring('line', ' ')
d = .directory~new; d['a'] = 1; d['b'] = 20; d['c'] = 3
big = d~select({ :v :k | v > 2 })
do k over big~allIndexes~sortWith({:p :q | p <= q}); say k big[k]; end
s.1 = 'x'; s.2 = 'yy'
st = .stem~new; st[1] = 10; st[2] = 20
say st~collect({:v | v + 1})[2]
sq = { :x | x * x }
say sq~source '|' sq~string '|' sq~class
nb = .block~new('use arg a, b; return a || b')
say nb~call('x', 'y')
k = 4
say .block~new(':x | x + k')~call(1)
call part3
call part2
exit

part3:
signal on syntax name p3
bad = { :x x }
p3: say 'caught' condition('d')
signal on syntax name p3b
say .array~of(1, 2)~select({ :x | 'maybe' })
p3b: say 'caught' condition('d')
return

part2:
acct = .account~new
acct~add(10); acct~add(20); acct~add(5)
say 'total with fee' acct~total
say 'big' acct~big(8)~makestring('line', ' ')
/* a block calls an internal routine of the program */
say .array~of(1, 2, 3)~collect({ :x | square(x) })~makestring('line', ' ')
/* labels inside a block */
b = { use arg n; call twice n; return result
      twice: return arg(1) * 2 }
say b~call(7)
/* LISP functions take blocks */
l = list(1, 2, 3)
say l~map({ :x | x * 10 }) mapcar({ :x | x + 1 }, l) l~filter({ :x | x > 1 }) l~reduce({ :a :x | a + x }, 0)
say list(4, 5)~select({ | 1 })
/* whileTrue */
i = 0
{ | i < 3 }~whileTrue({ i = i + 1; say ' step' i })
/* recursion through a variable */
fact = { use arg n; if n <= 1 then return 1; return n * fact~call(n - 1) }
say fact~call(10)
/* a block made in a routine and used after the routine returned */
adder = make_adder(5)
say adder~call(1)
p = make_proc()
say p~call(4)
/* nested blocks */
outer = { use arg k; return { :y | y * k } }
say outer~call(3)~call(7)
/* parameters are local, other variables shared */
x = 'outer x'; n = 0
.array~of(1, 2)~do({ :x | n + x })
.array~of(1, 2)~do({ use arg x; n = n + x; tmp = 1 })
say x n symbol('TMP')
/* a counter: the block keeps the variables of make_counter */
c = make_counter()
say c~call c~call c~call
/* errors in a block reach the caller */
signal on syntax
say { | 1 / 0 }~value
return
syntax: say 'caught' rc condition('d')
signal on syntax name s2
say adder2~call(1)
s2: say 'caught' condition('d')
return

square: return arg(1) ** 2
make_adder: procedure
  use arg n
  return { :x | x + n }
make_counter: procedure
  count = 0
  return { count = count + 1; return count }
make_proc: procedure
  return { procedure; use arg x; return x * x }

::class account
::method init
  expose items rate
  items = .array~new; rate = 1.1
::method add
  expose items
  items~append(arg(1))
::method total
  expose items rate
  return items~inject(0, { :a :x | a + x * rate })
::method big
  expose items
  lim = arg(1)
  return items~select({ :x | x > self~limit(lim) })
::method limit private
  return arg(1)
::requires 'lisp'
