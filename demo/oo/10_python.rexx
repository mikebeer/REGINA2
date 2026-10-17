/* 10_python.rexx - Python libraries from Rexx
 *
 * .Python~import loads a Python module; its functions and classes are
 * used with ~ like Rexx methods. Rexx numbers, strings, Arrays and
 * Directories are converted on the way in; Python numbers and strings
 * come back as Rexx strings, everything else as a PyObject.
 * The program analyses a small table of sales figures.
 */
::requires 'python'

stats = .python~import('statistics')
json = .python~import('json')
coll = .python~import('collections')

/* the data: one Rexx Directory per sale */
sales = .array~new
call sale 'Zurich', 'tea', 120
call sale 'Bern', 'coffee', 340
call sale 'Zurich', 'coffee', 210
call sale 'Basel', 'tea', 90
call sale 'Bern', 'tea', 150
call sale 'Zurich', 'cocoa', 60

amounts = .array~new
do s over sales
  amounts~append(s['amount'])
end
say 'sales     ' amounts~makestring('line', ' ')
say 'total     ' .python~sum(amounts)
say 'mean      ' stats~mean(amounts)
say 'median    ' stats~median(amounts)
say 'stdev     ' format(stats~stdev(amounts), , 2)

/* a Counter of the products, most common first */
products = .array~new
do s over sales
  products~append(s['product'])
end
do pair over coll~Counter(products)~most_common
  say 'product   ' left(pair~at(0), 8) pair~at(1)
end

/* totals per city in a Python dict, back as a Rexx stem */
totals = coll~defaultdict(.python~eval('int'))
do s over sales
  totals~put(totals~at(s['city']) + s['amount'], s['city'])
end
totals~toStem(t.)
say 'Zurich    ' t.zurich
say 'by city   ' json~dumps(.python~dict(totals), .python~kw('sort_keys', 1))

/* the sales as JSON text and back */
text = json~dumps(sales)
say 'json      ' left(text, 60)'...'
again = json~loads(text)~toRexx
say 'read back ' again~items 'sales, first in' again[1]['city']

/* a Python function, defined from Rexx */
.python~exec('def vat(x, rate=8.1):'                     || '0a'x ||,
             '    return round(x * (1 + rate / 100), 2)')
say 'with VAT  ' .python~vat(120) .python~vat(120, .python~kw('rate', 2.6))
exit

sale: procedure expose sales
  d = .directory~new
  d['city'] = arg(1); d['product'] = arg(2); d['amount'] = arg(3)
  sales~append(d)
  return
