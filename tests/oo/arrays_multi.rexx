/* arrays with more than one dimension */
m = .array~new(2, 3)                   /* a 2 x 3 matrix */
do i = 1 to 2
  do j = 1 to 3
    m[i, j] = i * 10 + j
  end
end
say 'dimension' m~dimension 'sizes' m~dimension(1) m~dimension(2) m~dimension(3),
    'dimensions' m~dimensions~toString(, 'x') 'size' m~size 'items' m~items
say 'm[2,3] =' m[2, 3] ' at(1,2) =' m~at(1, 2) ' hasIndex(2,2)' m~hasIndex(2, 2),
    'hasIndex(5,1)' m~hasIndex(5, 1) ' m[9,9] isNil' m[9, 9]~isNil
r = ''; do v over m; r = r v; end; say 'over:' r          /* row by row */
do with index ix item v over m
  if v // 2 = 0 then say '  even' v 'at' ix~toString(, ',')
end
m[3, 4] = 'grown'                      /* grows both dimensions */
say 'grown to' m~dimensions~toString(, 'x') 'm[1,1]' m[1,1] 'm[2,3]' m[2,3] 'm[3,4]' m[3,4]
say 'remove' m~remove(1, 1) 'items' m~items 'm[1,1] isNil' m[1,1]~isNil
say 'index of 23:' m~index(23)~toString(, ',')
c = m~copy; c[1, 2] = 'changed'; say 'copy' m[1,2] c[1,2] c~dimension
cube = .array~new                      /* an empty array takes the dimensions */
cube[2, 2, 2] = 'corner'               /* of its first item: 3 here */
say 'cube' cube~dimension cube~size cube[2,2,2] cube~allIndexes~items
s = m~supplier; s~next; say 'supplier index' s~index~toString(, ',') 'item' s~item
call t 'm[1] = 5'
call t 'x = m[1, 2, 3]'
call t 'm~append(9)'
call t 'm~sort'
call t 'v = .array~of(1,2)[1, 1]'
exit
t: parse arg code
  signal on syntax name bad
  interpret code
  say 'ran' code
  return
bad: say 'error' condition('D')
  return
