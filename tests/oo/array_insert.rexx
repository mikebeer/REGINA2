/* Array~insert and List~insert insert after the index, as in ooRexx */
a = .array~of('a', 'b', 'c')
say a~insert('x', 1) a~toString(, ',')
say a~insert('front', .nil) a~toString(, ',')
say a~insert('end') a~toString(, ',')
l = .list~of('one', 'three')
say l~insert('two', 1) l~toString(, ',')
