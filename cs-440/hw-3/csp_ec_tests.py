import unittest
import csp
#
#  Hey CS 440 ;)
#  You can ignore this; it's here so you can
#  easily run these tests without having the
#  wsuvpyuntitest.py file.
#
try:
    # try to import the category annotation
    from wsuvpyunittest import category
except ImportError:
    # if import fails, make a no-op fall back.
    def category(name):
        def wrapper(obj):
            return obj
        return wrapper


FourSquare = """4
0; 1 2
1; 0 3
2; 0 3
3; 1 2
"""

FourSquareDiagonal = """4
0; 1 2
1; 0 2 3
2; 0 1 3
3; 1 2
"""

TenVertices2CC = """10
0; 1
1; 0 2 3 4
2; 1 3
3; 1 2
4; 1 5 7
5; 4 7
6; 7
7; 5 6 9
9; 7
"""

AUS = """7
0; 1, 3
1; 0, 2, 3
2; 1, 3, 4
3; 0, 1, 2, 4, 5
4; 2, 3, 5
5; 3, 4"""

FOUR_G = """6
0; 1, 2, 3, 4, 5
1; 0, 2, 3, 5
2; 0, 1, 5
3; 0, 1, 4
4; 0, 3, 5
5; 0, 1, 2, 3"""


@category("Extra Credit")
class CSP_EC_Tests(unittest.TestCase):


    def testCSPStats_2ColorsIsUnsolvable(self):
        graph = parse_graph_str(TenVertices2CC)
        c = csp.CSP(graph, makedomains(2, graph))
        s = c.solve(allstats=True, quiet=True)
        self.assertFalse(s['Solutions'])
        self.assertEqual(s['Rejected'], 1024,
                         "There are 1024 states/leaves in this space")

    def testCSPStats_2Colorsv2(self):
        graph = parse_graph_str(FourSquareDiagonal)
        c = csp.CSP(graph, makedomains(2, graph))
        s = c.solve(allstats=True, quiet=True)
        print(s)
        self.assertFalse(s['Solutions'])
        self.assertEqual(s['Rejected'], 16,
                         "There are 16 states/leaves in this space")

    def testCSPStats_2Colorsv3(self):
        graph = parse_graph_str(FourSquare)
        c = csp.CSP(graph, makedomains(2, graph))
        s = c.solve(allstats=True, quiet=True)
        print(s)
        self.assertTrue(s['Solutions'])
        self.assertLessEqual(s['Rejected'], 14, "14 leaves are invalid")

    def testCSPStats_2ColorsCount(self):
        graph = parse_graph_str(FourSquare)
        c = csp.CSP(graph, makedomains(2, graph))
        s = c.solve(allstats=True, count=True, quiet=False)
        print(s)
        self.assertTrue(s['Solutions'])
        self.assertEqual(s['SolutionCount'], 2)
        self.assertEqual(s['Rejected'], 14, "all 14 invalid leaves should be rejected")
        self.assertEqual(s['Unexplored'], 0)

def makedomains(ncolors, graph):
    """Return map from vertex id -> set of color codes"""
    colors = 'RGBCMYKOPASDF'
    if ncolors > len(colors):
        print("That's too many colors ;)")
        return None
    return {k: set(colors[:ncolors]) for k in graph}
def load_graph_file(fname):
    with open(fname, 'rt') as fin:
        graphstr = fin.read()
        return graphstr


def parse_graph_str(graphstr):
    lines = graphstr.splitlines()
    nv = int(lines[0].strip())
    graph = {i: {} for i in range(nv)}
    for line in lines[1:]:
        (v, e) = line.split(';')
        v = int(v.strip())
        e = [int(_) for _ in e.split()]
        graph[v] = e
    #pprint(self.graph)
    return graph

def parse_domains(domainstr):
    domains = {}
    for line in domainstr.splitlines():
        v,d = line.split(':')
        v = int(v.strip())
        d = set(d.strip())
        domains[v] = d
    return domains
