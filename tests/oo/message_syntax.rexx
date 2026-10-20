/* every spelling of the message operators, on strings and objects */
a = 'abc'; i = 2
say a~length a->length a~substr(2, 1) a -> substr(2, 1)
say a~~reverse a ->> reverse            /* cascades return the receiver */
say "str"~length 'str' -> length (1 + 2)~abs a~reverse~upper a->reverse->upper
say a~length + 1 -a~length \a~length~"="(3) a~ length
say a~"||"('d') '5'~"+"(1)~"="(6) 'x'~"=="('x')
arr = .array~of(10, 20, 30)
say arr[1] arr[i] arr[i + 1] arr~makearray[3] arr[1]~"+"(1)
m = .array~new
m[1] = .array~of('x', 'y')
say m[1][2] m[1]~items
arr[2] = 'twenty'
say arr~tostring('C', ',')
arr~append('forty')                     /* message instruction */
say arr~items result
q = .queue~new
q ->> queue(1) ->> queue(2)~~queue(3)   /* no blank before ~~ */
say q~items
/* classic meanings that must not change */
b = 0
say a ~b
say a~=b a~ =b a~<b
say ~b ~ b
say~b
if~b then say "if-not ok"
