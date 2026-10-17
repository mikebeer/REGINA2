/* classic: things that look like the REGINA-OO marker but are not */
say '/* REGINA-OO */'
say "options regina-oo"
/* the word REGINA-OO inside a longer comment */
/* outer /* REGINA-OO */ comment */
x = 'a',
  'options regina-oo'
say x
options 'NOT_A_MARKER'
counter = 2
r = 0; do counter; r = r + 1; end; say 'do counter:' r
loop = 'still a variable'; say loop
