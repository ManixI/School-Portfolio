# HW-5 Mapper-Reducer

## Contributers
 - Ian Manix

## How To Run
using the following command will take the input file and output the result to out.dat
cat input.dat | python3 mapper.py | sort -k1,1 | python3 reducer.py >> out.dat
The #! is set to /usr/bin/env python as in the slides, so you can replace python3 with ./ if that is correct
on your machine (it's not on mine)
