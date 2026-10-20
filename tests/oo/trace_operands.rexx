/* TRACE I in an OO-active program must look like classic Regina */
o = .array~new; n = 2
trace i
r = n * 2 + 1
s = n || 'x'
t = (n+1) * n
trace o
say r s t
/* NOVALUE still raised for an unset left operand */
signal on novalue name nov
x = undefinedvar + 1
say 'not reached'
exit
nov: say 'NOVALUE for' condition('D') 'at line' sigl
