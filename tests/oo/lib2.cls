say 'lib2 prolog'
::requires 'lib1'
::class c1 public
::method chain
  return 'c1'
::class c2 public subclass c1
::method chain
  return 'c2>' || self~chain:super
::routine helper2 public
  return arg(1)*10
