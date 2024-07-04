import re
import unittest
import tictactoe as ttt
import subprocess
from wsuvpyunitrunner import category, WSUVTestCase

class TicTacToe_ECTests(WSUVTestCase):


    @category('Extra Credit')
    def testMonteCarloCommandLine1(self):
        cmd = 'python3 tictactoe.py --state 102022110 --mc 10000'
        out = subprocess.check_output(cmd, shell=True)
        # get the last line...
        lastout = out.decode('ascii').splitlines()[-1]
        pattern = '(\d+) trials: 1 wins ([\d\.]+), -1 wins ([\d\.]+), stalemates ([\d\.]+)'
        expected_re = re.compile(pattern)
        self.assertIsNotNone(expected_re.match(lastout),
                             "Last line doesn't match expected pattern")

        grps = expected_re.match(lastout).groups()
        self.assertEquals(int(grps[0]), 10000)
        wins = [float(g) for g in grps[1:]]
        tolerance = .08
        expected = [.67, .33, 0.0]

        for w,e in zip(wins, expected):
            in_tolerance = (w <= (e + tolerance) and w >= (e - tolerance))
            self.assertTrue(in_tolerance,
                            "Monte-carlo results are outside of tolerance")
