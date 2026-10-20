c = .ctr~new
do i = 1 to 50
  c~inc(i)
end
say c~total c~depth(5) c~total
d = .ctr~new; d~inc(280)
d~inc(1000)
say c~total d~total
e = .ctr~new
say e~total
call proc c
say c~total
exit
proc: procedure
  o = arg(1)
  o~inc(1)
  return
::class ctr
::method init
  expose tot st.
  tot = 0; st. = 0
::method inc
  expose tot st.
  use arg v
  tot = tot + v
  st.v = st.v + 1
::method total
  expose tot
  return tot
::method depth
  expose tot
  use arg n
  if n = 0 then return tot
  tot = tot + 1
  r = self~depth(n - 1)
  self~inc(0)
  return r tot
