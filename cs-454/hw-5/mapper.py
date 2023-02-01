#!/usr/bin/env python
"""mapper"""

import sys

# itterate over each line in input
for line in sys.stdin:
    line = line.strip()
    word = line.split()

    # print data to stdout sepperated by tabs
    print(word[0]+'\t'+word[1]+'\t'+str(int(word[2]))+'\t'+word[3])
