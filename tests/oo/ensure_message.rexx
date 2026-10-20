/* Block~ensure, ifCurtailed, condition('O') and the Message class */
/* OOREGINA */
log = .array~new
r = { log~append('work'); return 42 }~ensure({ log~append('cleanup') })
say r log~makestring('line', ' ')
log~empty
signal on syntax name s1
{ log~append('start'); x = 1 / 0 }~ensure({ log~append('cleanup') })
say 'not reached'
s1:
say 'caught' rc condition('d')
say log~makestring('line', ' ')
c = condition('O')
say c~class c['CONDITION'] c['CODE'] c['RC'] c['ERRORTEXT'] c['INSTRUCTION'] c['POSITION']
say c['DESCRIPTION']
log~empty
say { return 'fine' }~ifCurtailed({ log~append('never') }) log~items
signal on syntax name s2
{ y = 'a' + 1 }~ifCurtailed({ log~append('curtailed') })
s2:
say 'caught' condition('O')['CODE'] log~makestring('line', ' ')
m = .message~new(.array~of(3, 1, 2), 'sortWith', { :a :b | a <= b })
say m m~messageName m~target~class m~arguments~items
say m~send~makestring('line', ' ') m~result~class
m2 = .message~new('abc', 'reverse')
say m2~send m2~send('xyz') m2~result
say condition('O')~class
call f
exit
f: procedure
  say condition('O')
  { say 'body'; exit 3 }~ensure({ say 'cleanup before exit' })
