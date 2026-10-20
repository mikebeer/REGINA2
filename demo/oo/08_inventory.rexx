/* 08_inventory.rexx - a small application: stock keeping
 *
 * Products are objects, the inventory is a class holding a Directory of
 * products by article number. Orders change the stock; a report lists
 * the stock value, sorted, and the articles to reorder.
 */
data = 'A100 Screwdriver 4.50 120 50;',
       'A200 Hammer      12.90  15 20;',
       'B310 Drill       89.00   4  5;',
       'B320 Saw         24.00  30 10;',
       'C900 Toolbox     35.50   8  5'

inv = .inventory~new
do line over data~makeArray(';')
  parse var line id name price qty minimum .
  inv~add(.product~new(id, name, price, qty, minimum))
end

orders = .array~of('A200 10', 'B310 3', 'C900 9', 'X999 1', 'A100 25')
do o over orders
  parse var o id n
  say left(o, 10) '->' inv~order(id, n)
end
say
call report inv
exit

report: procedure
  use arg inv
  say 'Article Name          Qty    Value'
  do p over inv~byValue
    say left(p~id, 7) left(p~name, 12) right(p~qty, 5) right(format(p~value, , 2), 8)
  end
  say copies('-', 35)
  say left('Total', 25) right(format(inv~totalValue, , 2), 10)
  reorder = inv~below
  if reorder~items > 0 then
    say 'Reorder:' reorder~toString(, ', ')
  return

::class Product
::attribute id get
::attribute name get
::attribute price get
::attribute qty get
::attribute minimum get
::method init
  expose id name price qty minimum
  use arg id, name, price, qty, minimum
::method take                          /* returns .false if not enough */
  expose qty
  use arg n
  if n > qty then return .false
  qty = qty - n
  return .true
::method value
  expose qty price
  return qty * price
::method low
  expose qty minimum
  return qty < minimum

::class Inventory
::method init
  expose items
  items = .directory~new
::method add
  expose items
  use arg p
  items[p~id] = p
::method order
  expose items
  use arg id, n
  if \items~hasIndex(id) then return 'unknown article'
  p = items[id]
  if \p~take(n) then return 'only' p~qty 'in stock'
  return 'ok, left' p~qty
::method totalValue
  expose items
  t = 0
  do p over items~allItems
    t = t + p~value
  end
  return t
::method below                         /* names of articles to reorder */
  expose items
  list = .array~new
  do with index id item p over items
    if p~low then list~append(p~name)
  end
  return list
::method byValue                       /* products, highest value first */
  expose items
  keys = .array~new
  do with index id item p over items
    keys~append(right(format(p~value, , 2), 12) id)
  end
  keys~sort
  sorted = .array~new                  /* not "result": a message   */
  do i = keys~items to 1 by -1         /* instruction sets RESULT   */
    sorted~append(items[word(keys[i], 2)])
  end
  return sorted
