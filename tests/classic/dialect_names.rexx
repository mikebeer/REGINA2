/* Names that the OO dialect notations use keep their classic meaning */
this = 'T'; null = 'N'
say this null
drop this null
say this null
is = 1; as = 2
say is as
say 'a' is .b
say 'a' as .b
x.1 = 'one'
say x.1 length(x.1)
signal on syntax
say a.b(1)
exit
syntax:
  say 'error' rc
  exit
