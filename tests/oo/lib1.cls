say 'lib1 prolog'
::requires 'lib2'
::class c3 public subclass c2
::method who
  return 'c3'
::method chain
  return 'c3>' || self~chain:super
::routine libfn public
  return 'libfn' arg(1) helper2(arg(1))
