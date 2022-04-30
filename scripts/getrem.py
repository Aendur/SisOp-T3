Python 3.10.0 (tags/v3.10.0:b494f59, Oct  4 2021, 19:00:18) [MSC v.1929 64 bit (AMD64)] on win32
Type "help", "copyright", "credits" or "license()" for more information.
def getrem(n, alloc):
    r1 = 371 * 5 * (1 + 1023 // alloc)
    r4 = n * 2 * (1 + 4095 // alloc)
    r8 = n * 3 * (1 + 8191 // alloc)
    rr = n * 1 * (1 +  511 // alloc)
    return r1 + r4 + r8 + rr

def hr(n, alloc):
    print("%10d %.1f" % (getrem(n, alloc)))

    
hr(1000, 2048)
Traceback (most recent call last):
  File "<pyshell#12>", line 1, in <module>
    hr(1000, 2048)
  File "<pyshell#11>", line 2, in hr
    print("%10d %.1f" % (getrem(n, alloc)))
TypeError: not enough arguments for format string
def hr(n, alloc):
    siz = getrem(n, alloc)
    print("%10d %.1f" % (siz, siz / 1024 / 1024))

    
hr(500, 2048)
     10355 0.0
hr(1000,2048)
     18855 0.0
def getrem(n, alloc):
    r1 = 371 * 5 * (1 + 1023 // alloc) * alloc
    r4 = n * 2 * (1 + 4095 // alloc) * alloc
    r8 = n * 3 * (1 + 8191 // alloc) * alloc
    rr = n * 1 * (1 +  511 // alloc) * alloc
    return r1 + r4 + r8 + rr

hr(500, 2048)
  21207040 20.2
hr(1000,2048)
  38615040 36.8
