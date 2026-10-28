options levelg   -- CREXX notation (stage 3); "options levelb" works, too
namespace showcase expose asset

main: procedure = .int
  arg args = .string[]
  selected = .asset("log.txt")
  fallback = .asset("memo")
  say selected.describe()   /* file:log.txt:8 */
  say fallback.describe()   /* cache:memo:1 */
  sized = .asset.from_size(8)
  say sized.describe()
  other = .asset.from_size(3)
  say other.describe()
  say add(2, 3) typeof(sized) (sized is .asset) (sized is .fileasset)
  n = .int
  s = .string
  say 'n='n 's='s'.'
  item = .widget("gear", 4)
  say item.name() item.size() item.twice() total(3)
  v = .widget("cog", 2) as .named
  say typeof(v) (v is .measured)
  call check
  return 0

check: procedure
  signal on syntax
  w = .widget("x", "notanumber")
  say 'not reached'
  return
syntax:
  say 'error' rc':' condition('d')
  return

add: procedure = .int
  arg a = .int, b = .int
  return a + b

asset: interface
  *: factory
  arg spec = .string
  from_size: factory
  arg size = .int

  describe: method = .string
    return kind() || ":" || name() || ":" || size()

  kind: method = .string
  name: method = .string
  size: method = .int

fileasset: class implements .asset
  _name = .string
  _size = .int

  *: match
    arg spec = .string
    if spec = "log.txt" then return 100
    return 0

  *: factory
    arg spec = .string
    _name = spec
    _size = 8
    return

  from_size: match
    arg size = .int
    if size = 8 then return 50
    return 0

  from_size: factory
    arg size = .int
    _name = "sized-file"
    _size = size
    return

  kind: method = .string
    return "file"

  name: method = .string
    return _name

  size: method = .int
    return _size

cacheasset: class implements .asset
  _name = .string
  _size = .int

  *: factory
    arg spec = .string
    _name = spec
    _size = 1
    return

  from_size: factory
    arg size = .int
    _name = "cache-" || size
    _size = size
    return

  kind: method = .string
    return "cache"

  name: method = .string
    return _name

  size: method = .int
    return _size

named: interface
  *: factory
  arg label = .string, length = .int
  name: method = .string

measured: interface
  size: method = .int

widget: class implements .named .measured
  _label = .string
  _length = .int

  *: factory
    _label = label
    _length = length
    return

  name: method = .string
    return _label

  size: method = .int
    return _length

  twice: method = .int
    return size() * 2

total: procedure = .int
  arg n = .int
  t = 0
  do i = 1 to n
    t = t + i
  end
  return t
