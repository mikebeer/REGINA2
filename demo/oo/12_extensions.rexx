/* 12_extensions.rexx - more collections, ::extension and ensure
 *
 * Interval, Set, Bag and SortedCollection, new methods for the built-in
 * classes String and Array with ::extension, a cleanup that always runs
 * with ensure, and a condition object.
 */
text = 'the quick brown fox jumps over the lazy dog the end'
words = text~makearray(' ')

/* a Bag counts, a Set keeps each word once */
counts = .bag~new
do w over words; counts~add(w); end
unique = .set~new
do w over words; unique~add(w); end
say 'words:' words~items 'different:' unique~items 'the:' counts~occurrencesOf('the')

/* a SortedCollection ordered by a block: longest words first */
longest = .sortedCollection~new({ :a :b | length(a) >= length(b) })
do w over unique; longest~add(w); end
say 'longest:' longest~section(1, 3)~makestring('line', ', ')

/* set operations */
vowels = .set~of('a', 'e', 'i', 'o', 'u')
letters = .set~new
do i over 1~to(length(text)); letters~add(substr(text, i, 1)); end
say 'vowels used:' letters~intersection(vowels)~items 'of' vowels~items

/* an Interval: the squares of the odd numbers below 10 */
say 'odd squares:' 1~to(9, 2)~collect({ :n | n * n })~join(' ')

/* new methods for built-in classes, see ::extension below */
say 'title case:' 'regina rexx with objects'~titleCase
say 'average word length:' format(words~collect({ :w | length(w) })~average, , 2)

/* ensure: the cleanup runs although the work fails */
signal on syntax
{ say 'opening the file'; call work }~ensure({ say 'closing the file' })
exit
syntax:
c = condition('O')
say 'error' c['CODE'] 'in line' c['POSITION']':' c['ERRORTEXT']
exit

work: return 1 / 0

::extension String
::method titleCase
  return self~makearray(' ')~collect({ :w | translate(left(w, 1)) || substr(w, 2) })~join(' ')

::extension Array
::method join
  use arg sep
  return self~inject('', { use arg s, x; if s == '' then return x; return s || sep || x })
::method average
  return self~inject(0, { :s :x | s + x }) / self~items
