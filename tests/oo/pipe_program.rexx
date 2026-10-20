/* a program with directives read from a pipe (the pre-scan must not lose it) */
say .greeter~new~hello('pipe')
say twice(21)
exit
::class greeter
::method hello
  use arg who
  return 'hello from a' who
::routine twice
  return arg(1) * 2
