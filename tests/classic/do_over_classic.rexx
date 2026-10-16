/* classic: DO x OVER y is "DO expr" when x and OVER are empty */
x = ''; over = ''; y = 3; n = 0
do x over y
  n = n + 1
end
say 'looped' n 'x=[' || x || ']'
over = 'a'
signal on syntax
do x over y; end
exit
syntax: say 'error' rc
