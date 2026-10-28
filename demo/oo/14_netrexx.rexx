options netrexx
/* 14_netrexx.rexx - a NetRexx program run by Regina
 *
 * "options netrexx" (or a file name ending in .nrx) switches the NetRexx
 * notation on: classes with properties and methods, constructors named
 * like the class, Name(args) to create objects, obj.prop and obj.m(args),
 * loop, typed arguments (checked when the method is called).
 */
inv = Inventory()
inv.add(Item('bolt', 120, 0.05))
inv.add(Item('nut', 300, 0.02))
inv.add(Tool('wrench', 4, 12.50, 'steel'))
say inv.size() 'kinds of items, worth' inv.value().format(, 2)
loop it over inv.items
  say ' ' it.describe()
end
low = inv.below(10)
say 'low stock:' low
bolt = inv.find('bolt')
bolt.count = bolt.count - 100                 -- a property is set
say bolt.name 'now' bolt.count
say 'items created:' Item.created
say (bolt <= Item) (bolt <= Tool) (inv.find('wrench') <= Item)

class Item
  properties static
    created = 0
  properties
    name = String
    count = int
    price = 0
  method Item(aname, acount = int, aprice)
    name = aname; count = acount; price = aprice
    created = created + 1
  method value returns double
    return count * price
  method describe returns String
    return name.left(8) count.right(5) value().format(5, 2)

class Tool extends Item
  properties
    material
  method Tool(aname, acount = int, aprice, amaterial = 'iron')
    super(aname, acount, aprice)
    material = amaterial
  method describe returns String
    return super.describe() '('material')'

class Inventory
  properties
    items
  method Inventory
    items = .list~new
  method add(it)
    items.append(it)
  method size returns int
    return items.items()
  method value returns double
    total = 0
    loop it over items
      total = total + it.value()
    end
    return total
  method find(name)
    loop it over items
      if it.name = name then return it
    end
    return null
  method below(n = int) returns String
    names = ''
    loop it over items
      if it.count < n then names = names it.name
    end
    return names.strip()
