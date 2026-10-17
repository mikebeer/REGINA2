/* classic: stems passed to and returned from routines stay strings */
st.0 = 1; st.1 = 'a'
call show st.
say 'value' value_of(st.)
x. = 'dflt'
say 'returned' back() back2()
exit
show: procedure
  parse arg v
  say 'show sees' v arg(1) arg()
  return
value_of: procedure
  return arg(1)
back: procedure
  y.1 = 1
  return y.
back2: procedure
  z. = 'zdef'
  return z.
