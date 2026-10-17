# The Python interpreter inside Regina OO

This folder holds a copy of the C version of the Python 3 subset
interpreter from [mikebeer/python](https://github.com/mikebeer/python):

| Path | What |
|------|------|
| `py_all.c` | the interpreter, one strict C89 file (`c/py_all.c` upstream) |
| `pylib/`   | its pure-Python library: standard library, NumPy, pandas, SciPy, matplotlib (`pylib/` upstream, without `__pycache__`) |
| `UPSTREAM` | the upstream commit this copy was taken from |

`../pybridge.c` includes `py_all.c` and adds the C interface of
`../pybridge.h`; `../oo.c` builds the Rexx classes `.Python` and
`.PyObject` on it. The interpreter's language and library are documented
in the manual of the upstream repository.

## Changes against upstream

Two library modules assigned to `globals()[name]`, which the interpreter
does not keep (its `globals()` returns a copy); they use `exec` instead:

- `pylib/matplotlib/pyplot.py`: the module functions `plot`, `scatter`,
  `bar`, ... (`plt.plot(...)` failed with AttributeError)
- `pylib/http/client.py`: the HTTPStatus constants

## Updating

    tools/import-python.sh /path/to/mikebeer/python

copies `c/py_all.c` and `pylib/` again, applies the changes above and
records the commit in `UPSTREAM`. Then rebuild and run
`tests/run-regression.sh`.
