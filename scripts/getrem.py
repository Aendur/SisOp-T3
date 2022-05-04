def getrem(n, alloc):
    r1 = 371 * 5 * (1 + 1023 // alloc) * alloc
    r4 = n * 2 * (1 + 4095 // alloc) * alloc
    r8 = n * 3 * (1 + 8191 // alloc) * alloc
    rr = n * 1 * (1 +  511 // alloc) * alloc
    return r1 + r4 + r8 + rr


def hr(n, alloc):
    siz = getrem(n, alloc)
    print("%10d %.1f" % (siz, siz / 1024 / 1024))

    
hr(1000, 2048)
hr(500, 2048)
hr(1000,2048)
hr(500, 2048)
hr(1000,2048)

