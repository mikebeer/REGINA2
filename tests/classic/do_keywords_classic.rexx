/* classic program: LOOP, COUNTER, LABEL, WITH, INDEX, ITEM stay symbols */
counter = 2; label = ''; c = 3; with = 1
r = 0; do counter; r = r + 1; end; say 'do counter:' r
r = 0; do label c; r = r + 1; end; say 'do label c:' r
r = 0; do with; r = r + 1; end; say 'do with:' r
with = ''; index = ''; i = ''; item = ''; over = ''
r = 0; do with index i item 2; r = r + 1; end; say 'do with index i item 2:' r
x = ''
r = 0; do x over 2; r = r + 1; end; say 'do x over 2:' r
loop = 5; say 'loop =' loop
loop: say 'label loop reached'
signal on syntax
interpret 'do x over 2 for 1; end'
exit
syntax: say 'Error' rc'.'condition('E') condition('D')
signal on syntax name s2
interpret 'loop i = 1 to 2; end'
s2: say 'Error' rc condition('D')
