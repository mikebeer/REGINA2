/* 06_data_structures.rexx - building your own data structures
 *
 * A Stack as a subclass of Array, and a binary search tree made of Node
 * objects that refer to each other. .nil marks a missing child.
 */
st = .stack~new
st~push('a'); st~push('b'); st~push('c')
say 'stack top' st~peek', size' st~items
say 'popped' st~pop st~pop', left' st~items

tree = .tree~new
do w over 'mango apple peach banana cherry apple kiwi'~makeArray(' ')
  tree~insert(w)
end
say 'tree holds' tree~count 'distinct words'
say 'in order :' tree~inorder~toString(, ' ')
say 'has kiwi :' tree~contains('kiwi')
say 'has lemon:' tree~contains('lemon')
say 'depth    :' tree~depth
exit

::class Stack subclass Array           /* inherits APPEND, ITEMS, ... */
::method push
  use arg item
  self~append(item)
::method pop
  if self~items = 0 then return .nil
  return self~remove(self~last)
::method peek
  if self~items = 0 then return .nil
  return self[self~last]

::class Node
::attribute value
::attribute left
::attribute right
::method init
  expose value left right
  use arg value
  left = .nil; right = .nil

::class Tree
::method init
  expose root count
  root = .nil; count = 0
::attribute count get

::method insert
  expose root count
  use arg v
  if root~isNil then do
    root = .node~new(v); count = 1; return
  end
  n = root
  do forever
    select
      when v == n~value then return            /* already there */
      when v << n~value then do
        if n~left~isNil then do; n~left = .node~new(v); leave; end
        n = n~left
      end
      otherwise
        if n~right~isNil then do; n~right = .node~new(v); leave; end
        n = n~right
    end
  end
  count = count + 1

::method contains
  expose root
  use arg v
  n = root
  do while \n~isNil
    if v == n~value then return .true
    if v << n~value then n = n~left
    else n = n~right
  end
  return .false

::method inorder                      /* sorted items, recursively */
  expose root
  list = .array~new
  call walk root, list
  return list

::method depth
  expose root
  return height(root)

::routine walk
  use arg n, list
  if n~isNil then return
  call walk n~left, list
  list~append(n~value)
  call walk n~right, list

::routine height
  use arg n
  if n~isNil then return 0
  return 1 + max(height(n~left), height(n~right))
