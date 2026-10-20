a = .array~of(1)
parse value a with w1 w2
say 'w1=['w1'] w2=['w2'] class:' w1~class~id
parse var a v1 3 v2
say 'pos: v1 len' length(v1) 'v2 len' length(v2) 'c2x(v1)=' c2x(v1)
parse upper value a with u
say 'upper:' u
b = .bag~new
parse value b with x y
say 'bag:' x '/' y
x = translate(a)
say 'translate:' x
push a
pull z
say 'pulled' z
exit
::class bag
::method string
  return 'my bag'
