/* 04_operators.rexx - operator methods
 *
 * A Fraction class that defines + - * / = < and the prefix minus.
 * When the left operand of an operator is an object, Rexx sends the
 * operator as a message to it, so a + b means a~"+"(b).
 */
a = .fraction~new(1, 2)
b = .fraction~new(1, 3)
say a '+' b '=' a + b
say a '-' b '=' a - b
say a '*' b '=' a * b
say a '/' b '=' a / b
say '-('a') =' (-a)                  /* prefix minus: a~"-"() */
say a '+ 1 =' a + 1                    /* plain numbers work too */
say 'a = 2/4 ?' (a = .fraction~new(2, 4))
say 'b < a ?' (b < a)

/* sum of 1/1 + 1/2 + ... + 1/6, exactly */
sum = .fraction~new(0)
do n = 1 to 6
  sum = sum + .fraction~new(1, n)
end
say 'harmonic(6) =' sum '=' format(sum~value, , 5)
exit

::class Fraction
::method init                          /* stored in lowest terms */
  expose num den
  use arg num, den
  if arg(2, 'O') then den = 1
  if den < 0 then do; num = -num; den = -den; end
  g = gcd(abs(num), den)
  num = num % g; den = den % g
::method num;   expose num; return num
::method den;   expose den; return den
::method value; expose num den; return num / den

::method "+"
  expose num den
  use arg o
  o = asFraction(o)
  return .fraction~new(num * o~den + o~num * den, den * o~den)
::method "-"
  expose num den
  if arg() = 0 then return .fraction~new(-num, den)    /* prefix minus */
  use arg o
  o = asFraction(o)
  return .fraction~new(num * o~den - o~num * den, den * o~den)
::method "*"
  expose num den
  use arg o
  o = asFraction(o)
  return .fraction~new(num * o~num, den * o~den)
::method "/"
  expose num den
  use arg o
  o = asFraction(o)
  return .fraction~new(num * o~den, den * o~num)
::method "="
  expose num den
  use arg o
  o = asFraction(o)
  return num = o~num & den = o~den
::method "<"
  expose num den
  use arg o
  o = asFraction(o)
  return num * o~den < o~num * den
::method string
  expose num den
  if den = 1 then return num
  return num'/'den

::routine asFraction                   /* accept 3 as well as 3/1 */
  use arg x
  if x~isA(.fraction) then return x
  return .fraction~new(x)

::routine gcd
  use arg a, b
  do while b > 0
    parse value b a // b with a b
  end
  if a = 0 then return 1
  return a
