/* ::extension: methods added to existing classes, also built-in ones */
say 'hello world'~shout 'abc'~isPalindrome 'otto'~isPalindrome
say 'abc'~length 'abc'~reverse
a = .array~of(3, 1, 2)
say a~sum a~average a~max
say a~sortWith({ :x :y | x <= y })~join('-')
say 5~squared 1~to(4)~sum
say .directory~new~describe
r = .rect~new(3, 4)
say r~area r~perimeter r~describe
signal on syntax
say 'abc'~describe
exit
syntax: say condition('d')
exit

::extension String
::method shout
  return translate(self)'!'
::method isPalindrome
  return self == reverse(self)
::method reverse                        /* replaces the built-in method */
  return 'reversed:' reverse(self)
::method squared
  return self * self

::extension Array inherit Describable
::method sum
  return self~inject(0, { :s :x | s + x })
::method average
  return self~sum / self~items
::method max
  return self~inject(self[1], { :m :x | max(m, x) })
::method join
  use arg sep
  return self~inject('', { use arg s, x; if s == '' then return x; return s || sep || x })

::extension Interval
::method sum
  return self~inject(0, { :s :x | s + x })

::extension Directory inherit Describable

::class Describable mixinclass Object
::method describe
  return 'I am' self~class~id

::class rect inherit Describable
::method init
  expose w h
  use arg w, h
::method area
  expose w h
  return w * h
::extension rect
::method perimeter
  expose w h
  return 2 * (w + h)
