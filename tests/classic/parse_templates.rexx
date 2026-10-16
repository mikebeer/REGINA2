/* relative and absolute positions in templates */
s = 'abcdefghij'
parse var s 3 x +2 y -4 z
say x '/' y '/' z
parse var s a 5 b
say a '/' b
p = 2
parse var s =(p) c +(p) d
say c '/' d
parse var s e -1 f
say e '/' f
