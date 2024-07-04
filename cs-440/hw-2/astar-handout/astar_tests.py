import unittest
import astar
import subprocess
import time
import sys

class AStarTestCase(unittest.TestCase):

    def testAStar_onLittleGridProblem(self):
        gp = astar.LittleGridProblem()
        a = astar.AStar(gp)
        pth = a.search((0,0))
        self.assertEqual(10, pth.g, "Expected a path cost of 10 on the default grid")
        states = a.list_of_states(pth)
        self.assertEqual(11, len(states), "Expected path to contain 9 elements")
        print(a.reached)
        self.assertTrue((0,0) in a.reached, "Expected (0,0) in reached table")
        self.assertAlmostEqual(a.reached[(0,1)].g, 5.5, msg="Expected to reach (row 0, col 1) with cost 5.5")

    def testAStar_onSmallGridProblemNoHeuristic(self):
        grid = [[1,  10, 10, 10, 10],
                [1,  1,  10, 10, 10],
                [10, 1,  1,  1,  1],
                [10, 10, 1,  10, 1],
                [10, 10, 1,  10, 1]]
        gp = astar.GridProblem(grid, goaltest=lambda s: (s==(4,4)), hfn=lambda x: 0)
        a = astar.AStar(gp)
        pth = a.search((0,0))
        self.assertEqual(8, pth.g, "Expected a path cost of 8 on the default grid")
        states = a.list_of_states(pth)
        self.assertEqual(9, len(states), "Expected path to contain 9 elements")
        self.assertTrue((0,0) in a.reached, "Expected (0,0) on reached table")
        self.assertTrue((4,2) in a.reached, "Expected (4,2) on reached table")
        #print("Your reached table has: ", a.reached)


    def testAStar_Greedyness(self):
        grid = [[1,  10, 10, 1,  1,  1],
                [1,  1,  10, 1, 10,  1],
                [10, 1,  1,  1, 30,  1],
                [10, 10, 10, 1, 10,  1],
                [10, 10, 10, 2, 1,  1]]
        gp = astar.GridProblem(grid, hfn=lambda x: 0, goaltest=lambda x: x == (4,5))
        a = astar.AStar(gp)
        pth = a.search((0,0))
        print("PATH IS", a.list_of_states(pth), file=sys.stderr)
        self.assertEqual(10, pth.g, "Expected a path cost of 10 on this grid")
        states = a.list_of_states(pth)
        self.assertEqual(10, len(states), "Expected path to contain 10 elements")
        self.assertTrue((0,0) in a.reached, "Expected (0,0) on reached table")
        self.assertTrue((4,3) in a.reached, "Expected (4,3) on reached table")
        #print("Your reached table has: ", a.reached)


    def testAStar_on8x12png(self):
        ip = astar.ImageProblem('12x8.png', lambda x: x == (11,7), lambda s: abs(s[0]-11)+abs(s[1]-7))
        a = astar.AStar(ip)
        pth = a.search((0,0))
        self.assertEqual(15, pth.g, "Expected a path cost of 15 from UL-corner to LR the '12x8.png'")
        states = a.list_of_states(pth)
        self.assertFalse(('R', (1,0)) in states, "Didn't expect (1,0) on the path... path should have taken the diagonal")

    def testAStar_on8x12trickypng(self):
        #ip = astar.ImageProblem('12x8tricky.png', lambda x: x == (11,7), lambda s: abs(s[0]-11)+abs(s[1]-7))

        def h(s):
            wleft  = abs(s[0] - 11)
            hleft = abs(s[1] - 7)
            diag = min(wleft, hleft)
            remainder = max(wleft, hleft) - diag
            return diag + remainder

        ip = astar.ImageProblem('12x8tricky.png', lambda x: x == (11,7), lambda x: 0)
        a = astar.AStar(ip)
        pth = a.search((0,0))
        self.assertEqual("18.41", "%.2f"%pth.g, "Expected a path cost of 18.41 from UL-corner to LR the '12x8tricky.png'")
        states = a.list_of_states(pth)
        self.assertFalse(('R', (1,0)) in states, "Didn't expect (1,0) on the path... path should have taken the diagonal")

    def testAStar_on8x12trickypngWithNonAdmissibleHeuristic(self):
        #ip = astar.ImageProblem('12x8tricky.png', lambda x: x == (11,7), lambda s: abs(s[0]-11)+abs(s[1]-7))

        def h(s):
            wleft  = abs(s[0] - 11)
            hleft = abs(s[1] - 7)
            return wleft+hleft

        ip = astar.ImageProblem('12x8tricky.png', lambda x: x == (11,7), h)
        a = astar.AStar(ip)
        pth = a.search((0,0))
        self.assertEqual("19.41", "%.2f"%pth.g, "Expected to find a suboptimal path with my weird heuristic '12x8tricky.png'")
        states = a.list_of_states(pth)
        self.assertFalse(('R', (1,0)) in states, "Didn't expect (1,0) on the path... path should have taken the diagonal")

    def testAStar_asScriptOnGrid(self):
        out = subprocess.getoutput("python3 astar.py")
        lines = out.splitlines()

        # this can be a bit tricky to test since minor variations may have
        # different use of the reached table without affecting optimality and completeness.
        # However, current psudocode of R&N are clear about when states go into the reached table

        expected = "11 states on path to goal"
        self.assertEqual(expected, lines[-2], f"Expected 2nd to last line to read '{expected}'")
        expected = "cost is: 10.00"
        self.assertEqual(expected, lines[-1], f"Expected last line to read '{expected}'")
        self.assertTrue(len(lines) >= 15, "hmm. your ouput doesnt have enough lines for all the states...did you remove output?")
        path = "\n".join(lines[-16:-4])

        # On the old code base, the corrdinates were transposed, so deal with that here:
        newstylepath = "(None, (0, 0))\n('D', (1, 0))\n('R', (1, 1))\n('D', (2, 1))\n('R', (2, 2))\n('R', (2, 3))\n('R', (2, 4))\n('D', (3, 4))\n('D', (4, 4))\n('R', (4, 5))\n('R', (4, 6))"
        oldstylepath = "(None, (0, 0))\n('R', (1, 0))\n('D', (1, 1))\n('R', (2, 1))\n('D', (2, 2))\n('D', (2, 3))\n('D', (2, 4))\n('R', (3, 4))\n('R', (4, 4))\n('D', (4, 5))\n('D', (4, 6))"
        self.assertTrue( (path == newstylepath or path == oldstylepath) )

    def testAStar_asScriptOn12x8png(self):
        out = subprocess.getoutput("python3 astar.py 12x8.png")
        lines = out.splitlines()
        expected = "16 states on path to goal"
        self.assertEqual(expected, lines[-2], f"Expected 2nd to last line to read '{expected}'")
        expected = "cost is: 15.00"
        self.assertEqual(expected, lines[-1], f"Expected last line to read '{expected}'")
