#!/usr/bin/env python3
#
# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.

from sys import argv

binfile = argv[1]
nwords = int(argv[2])

with open(binfile, "rb") as f:
    bindata = f.read()

assert len(bindata) < nwords                                # len() finds number of bytes
# assert len(bindata) % 4 == 0

for i in range(nwords):
    if i < len(bindata) // 1:
        w = bindata[1*i : 1*i+1]
        print("%02x" % (w[0]))
    else:
        print("%02x" % (0))