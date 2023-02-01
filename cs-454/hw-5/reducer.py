#!/usr/bin/env python
"""reducer"""

from operator import itemgetter
import sys

current_word = None
current_count = 0
current_size = 0
word = None

for line in sys.stdin:
    line = line.strip()

    word, count, size, validity = line.split('\t')
    try:
        count = int(count)
        size = int(size)
    except ValueError:
        # ignore lines where count or size is not a num
        continue

    # ignore non-validsted lines
    if 'F' in validity:
        continue

    if current_word == word:
        current_count += count
        current_size += size
    else:
        # output word we're storing
        if current_word:
            print(current_word+'{size}\t'+str(current_size))
            print(current_word+'{visis}\t'+str(current_count))
        # update current wrod
        current_count = count
        current_word = word
        current_size = size

# print last word
if current_word == word:
    print(current_word+'{size}\t'+str(current_size))
    print(current_word+'{visis}\t'+str(current_count))
