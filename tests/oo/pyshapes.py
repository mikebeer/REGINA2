# a small Python module used by python_bridge.rexx
class Rect:
    def __init__(self, w, h):
        self.w = w
        self.h = h
    def area(self):
        return self.w * self.h
    def scaled(self, f=2):
        return Rect(self.w * f, self.h * f)
    def __repr__(self):
        return "Rect(%r, %r)" % (self.w, self.h)

def describe(*args, **kw):
    return "args=%r kw=%r" % (args, sorted(kw.items()))
