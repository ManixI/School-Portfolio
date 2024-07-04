import unittest
import csp

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


class CSPTests(unittest.TestCase):

    def testCSP_4Colors(self):
        graph = parse_graph_str(TenVertices2CC)
        c = csp.CSP(graph, makedomains(4, graph))
        s = c.solve(quiet=True)
        print(s)
        self.assertTrue(s['Solutions'] is not None)

    def testCSP_3Colors(self):
        graph = parse_graph_str(TenVertices2CC)
        c = csp.CSP(graph, makedomains(3, graph))
        s = c.solve(quiet=True)
        self.assertTrue(s['Solutions'])

    def testCSP_2ColorsIsUnsolvable(self):
        graph = parse_graph_str(TenVertices2CC)
        c = csp.CSP(graph, makedomains(2, graph))
        s = c.solve(quiet=True)
        self.assertFalse(s['Solutions'])

    def testCSP_2Colorsv2(self):
        graph = parse_graph_str(FourSquareDiagonal)
        c = csp.CSP(graph, makedomains(2, graph))
        s = c.solve(quiet=True)
        print(s)
        self.assertFalse(s['Solutions'])

    def testCSP_2Colorsv3(self):
        graph = parse_graph_str(FourSquare)
        c = csp.CSP(graph, makedomains(2, graph))
        s = c.solve(quiet=True)
        print(s)
        self.assertTrue(s['Solutions'])

    def testCSP_2ColorsCount(self):
        graph = parse_graph_str(FourSquare)
        c = csp.CSP(graph, makedomains(2, graph))
        s = c.solve(count=True, quiet=False)
        print(s)
        self.assertTrue(s['Solutions'])
        self.assertTrue(['R', 'G', 'G', 'R'] in s['Solutions'] or
                        ['G', 'R', 'R', 'G'] in s['Solutions'])
        self.assertEqual(s['SolutionCount'], 2)

    def testCSP_2ColorsWantAll(self):
        graph = parse_graph_str(FourSquare)
        c = csp.CSP(graph, makedomains(2, graph))
        s = c.solve(solutions=2, count=True, quiet=False)
        print(s)
        self.assertTrue(s['Solutions'])
        self.assertTrue(['R', 'G', 'G', 'R'] in s['Solutions'])
        self.assertTrue(['G', 'R', 'R', 'G'] in s['Solutions'])
        self.assertEqual(s['SolutionCount'], 2)

    def testCSP_2ColorsWantTooMany(self):
        graph = parse_graph_str(FourSquare)
        c = csp.CSP(graph, makedomains(2, graph))
        s = c.solve(solutions=4, count=True, quiet=False)
        print(s)
        self.assertTrue(s['Solutions'])
        self.assertTrue(['R', 'G', 'G', 'R'] in s['Solutions'])
        self.assertTrue(['G', 'R', 'R', 'G'] in s['Solutions'])
        self.assertEqual(s['SolutionCount'], 2)


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
