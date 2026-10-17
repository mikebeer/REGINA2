/* DO forms: FOR with OVER, DO WITH, COUNTER, LABEL, LOOP */
a = .array~of('a', 'b', 'c', 'd')
r=''; do v over a for 2; r = r v; end; say 'for:' r
r=''; do v over a for 3 while v \= 'c'; r = r v; end; say 'for+while:' r
d = .directory~new; d['x'] = 1; d['y'] = 2
r=''; do with index k item v over d; r = r k'='v; end; say 'with dir:' r
r=''; do with item v index k over a; r = r k':'v; end; say 'with arr:' r
r=''; do with index k over a for 2; r = r k; end; say 'with idx for:' r
st.1 = 'one'; st.2 = 'two'; st.b = 'bee'
r=''; do with index t item v over st.; r = r t'='v; end; say 'with stem:' r
r=''; do with index k item v over d while v < 2; r = r k; end; say 'with while:' r
do counter c i = 1 to 3; end; say 'counter ctl:' c i
do counter c 4; end; say 'counter n:' c
do counter c v over a; end; say 'counter over:' c
do counter c while 0; end; say 'counter while0:' c
do label outer i = 1 to 3
  do label inner j = 1 to 3
    if j = 2 then iterate outer
    if i = 3 then leave outer
    say 'lbl' i j
  end inner
end outer
do label blk
  say 'in block'
  leave blk
  say 'not reached'
end blk
n = 0
loop
  n = n + 1
  if n = 3 then leave
end
say 'loop forever:' n
loop i = 1 to 2; say 'loop ctl' i; end
loop label l counter k while k < 5; end; say 'loop label counter:' k
r=''; loop with index i item x over a for 2; r = r i||x; end; say 'loop with:' r
loop = 'var'; say loop
/* a variable named loop still works with messages and [] */
loop = .array~of('x')
loop~append('y')
say 'loop var:' loop~items loop[2]
a = .array~of(10, 20, 30)
do with item v index i over a
  if i = 2 then leave v
  say 'leave item var' i v
end v
do counter c.1 v over a; end; say 'compound counter' c.1
interpret 'do with item x over a for 1; say "interp" x; end'
call t 'do v over a to 3; end'
call t 'do v over a by 2; end'
call t 'do label a label b; end'
call t 'do counter a counter b 2; end'
call t 'do with index i over 5; end'
call t 'do label l i = 1 to 2; end m'
call t 'do label l i = 1 to 2; end i'
call t 'do label l i = 1 to 2; end l'
call t 'do with index i item x over a; end x'
call t 'do with index i item x over a; end q'
call t 'do v over a for -1; end'
do label l2 i = 1 to 3; leave i; end
say 'leave ctl var with label' i
do label m v over a; leave v; end
say 'leave over var with label' v
do label n with index k item w over a; iterate w; end
say 'iterate item var with label' k w
call traced
exit

t: parse arg code
  signal on syntax name bad
  interpret code
  say 'ok   ' code
  return
bad: say 'error' code '->' condition('D')
  return

traced:
  a = .array~of('p', 'q')
  trace r
  do label l counter c with index i item x over a
    say i x c
  end l
  trace o
  return
