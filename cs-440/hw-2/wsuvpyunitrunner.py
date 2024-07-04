import unittest
from astar_tests import AStarTestCase

class WSUVTestCase(unittest.TestCase):
    pass


def category(s):
    def wrapper(f):
        return f
    return wrapper


def main():
    t = AStarTestCase()
    t.testAStar_onLittleGridProblem()
    t.testAStar_onSmallGridProblemNoHeuristic()
    t.testAStar_Greedyness()
    t.testAStar_on8x12png()
    t.testAStar_on8x12trickypng()
    t.testAStar_on8x12trickypngWithNonAdmissibleHeuristic()
    t.testAStar_asScriptOnGrid()
    t.testAStar_asScriptOn12x8png()

if __name__ == "__main__":
    main()
