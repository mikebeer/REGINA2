/* classic: things that look like the OOREGINA marker but are not */
say '/* OOREGINA */'
say "options ooregina"
/* the word OOREGINA inside a longer comment */
/* outer /* OOREGINA */ comment */
x = 'a',
  'options ooregina'
say x
options 'NOT_A_MARKER'
counter = 2
r = 0; do counter; r = r + 1; end; say 'do counter:' r
loop = 'still a variable'; say loop
