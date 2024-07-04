from heapq import *
import time
import argparse
import PIL
import PIL.Image
from collections import namedtuple

# A SearchTree Node is a tuple of (f,g,h,state,parent)
# where the parent value is a reference to another SearchTree Node
# of the same form.  The root of the search tree is represents the
# initial state and has no parent.
# The line below creates a new *class* called "TNode" that has
# 5 slots that can be accessed with the names:
#  'f', 'g', 'h', 'state' and 'parent' respectively.
# Since TNodes are tuples (but with names), they are immutable
# and all values must be supplied at initialization.
# Take a look at the code below to see how these objects are
# created and how instance data is accessed.
TNode = namedtuple('TNode', ['f', 'g', 'h', 'state', 'parent', 'action'])


class SearchProblem():
    """An abstract class representing a search problem.

    This class defines the interface for a search problem.
    Note that unlike the Russell & Norvig 4th edition, this
    search problem has one method 'successors' that takes
    as input a state, and yields tuples indicating:
    (action, edge cost, next state).  This is the same information
    given by the 4th edition's actions(), result(), action_cost()
    methods """

    def successors(self, state):
        """Generates a sequence of (action, edge cost, next state)
        tuples for the specified state.  This is the same information
        given by the 4th edition's actions(), result(), action_cost()
        methods"""
        pass

    def is_goal(self, state):
        """returns True iff the specified state satisifies the goal condition"""
        pass

    def h(self, state):
        """computes the h value (heuristic 'distance') of a given state,
        returns 0 by default"""
        return 0


class GridProblem(SearchProblem):
    """A Grid Problem allows cardinal direction movement on an 2D grid.
    The grid is represented with a list of lists and edge weights are the
    average value of the two adjacent tiles"""

    def __init__(self, listoflists=None, goaltest=None, hfn=None):
        """Setup an instance of a GridProblem.
        States in a GridProblem are represented as (row, col) tuples
        and represent a particular location in the grid.

        Arguments:
            listoflists: a 2-d grid, represented as a list-of-lists. The
                         list of lists representation is a list (dimension
                         equal to the grid height) and each interior list has
                         a dimension equal to the grid width. Each value
                         represents the 'cost' of the tile.  Travelling
                         in in the cardinal direction (N,S,E,W) between
                         two tiles has a cost that is the average of
                         the starting and ending tile's cost.

            goaltest:  a function which takes a state and returns
                         True iff the state satisifies the goaltest,

            hfn:       a heuristic function which takes a state
                         and estimates the distance remaining to the goal
        """

        self.grid = listoflists
        self.is_goal = goaltest
        self.h = hfn

    def successors(self, state):
        """In a GridProblem, successor states are vertically
        or horizontally adjacent; diagonal movement is not allowed.
        The edge weight is the average of the two tiles' costs.

        The yield (used below) is similar to a return statement, but allows
        a function to generate multiple values that can be iterated
        through.  If you're unfamiliar with it, it is worth looking up.
        """

        # state is (<row>, <col>)
        if state[0] > 0:
            ns = (state[0]-1, state[1])
            yield ('U', (self.grid[state[0]][state[1]] +
                         self.grid[ns[0]][ns[1]]) / 2.0, ns)

        if state[0] < len(self.grid)-1:
            ns = (state[0]+1, state[1])
            yield ('D', (self.grid[state[0]][state[1]] +
                         self.grid[ns[0]][ns[1]]) / 2.0, ns)
        if state[1] > 0:
            ns = (state[0], state[1]-1)  # left
            yield ('L', (self.grid[state[0]][state[1]] +
                         self.grid[ns[0]][ns[1]]) / 2.0, ns)

        if state[1] < len(self.grid[0])-1:
            ns = (state[0], state[1]+1)  # right
            yield ('R', (self.grid[state[0]][state[1]] +
                         self.grid[ns[0]][ns[1]]) / 2.0, ns)


class LittleGridProblem(GridProblem):
    """A small grid problem, all set to go!"""

    def __init__(self):
        # note traveling down one tile from the upper left corner
        # costs (1+1)/2 = 1; traveling right from the upper left
        # corner costs (1+10)/2 = 5.5
        matrix = [[1,  10, 10, 10, 10, 5, 5],
                  [1,  1,  10, 10, 1,  5, 1],
                  [10, 1,  1,  1,  1,  5, 1],
                  [10, 10, 1,  10, 1,  5, 1],
                  [10, 10, 1,  10, 1,  1, 1]]

        goaltest = lambda x: x == (4,6)
        hfunc = lambda x: abs(x[0]-6) + abs(x[1]-4)

        # this is one way to call the 'super' class initializer...
        GridProblem.__init__(self, matrix, goaltest, hfunc)


class ImageProblem(SearchProblem):
    """An ImageProblem allows cardinal direction movement on an x-y grid.
    Here, the grid is represented with pixels from the image. A state
    is represented by an (x,y) tuple where 0 <= x < image.width and
    0 <= y < image.height. Edges connect states in cardinal directions,
    AND diagonals."""

    def __init__(self, imagepth, goaltest, hfn=None):
        self.img = PIL.Image.open(imagepth)
        self.is_goal = goaltest
        self.h = hfn

    def successors(self, state):
        """_ Part 1:  Implement This Method _

        Hint: Use self.img.getpixel(p) to get a tuple of RGB
        values; p should be an (x,y) coordinate tuple specifying the
        pixel location you wish to obtain RBG vaues for.

        the cost between two adjacent nodes should be calculated as:
         1 + sum( ((pixel value difference in channel)/32)**2 for each channel R,G,B )

        that is,
        (1) find the difference in each R,G,B channel,
        (2) divide those differences by 32 (to compress them a bit)
        (3) square the result (to make it positive and non-linear)
        (4) sum across the channels.

        so for two states whose R,G,B values are (100,100,100) and (100,116,36)
        the cost should be 5.25 = (1 + (0/32)**2 + (16/32)**2, (64/32)**2).

        Yields all successors of the specified state as
        tuples of: (<dir>, edgecost, nextstate)
        where <dir> is one of {'U', 'D', 'L', 'R', 'UR', 'UL', 'DR', 'DL'}

        @param {tuple} (x,y) state
        """

        width = self.img.width-1
        height = self.img.height-1
        successor_states = []

        left = False
        right = False
        top = False
        bottom = False

        # just done to make to following code more readable
        if state[1] > 0:
            top = True
        if state[1] < height:
            bottom = True
        if state[0] > 0:
            left = True
        if state[0] < width:
            right = True       

        # top row
        if top:
            if left:
                nxt = (state[0]-1, state[1]-1)
                tmp = ('UL', self.__calc_cost(state, nxt), nxt)
                successor_states.append(tmp)

            nxt = (state[0], state[1]-1)
            tmp = ('U', self.__calc_cost(state, nxt), nxt)
            successor_states.append(tmp)

            if right:
                nxt = (state[0]+1, state[1]-1)
                tmp = ('UR', self.__calc_cost(state, nxt), nxt)
                successor_states.append(tmp)

        # bottom row
        if bottom:
            if left:
                nxt = (state[0]-1, state[1]+1)
                tmp = ('DL', self.__calc_cost(state, nxt), nxt)
                successor_states.append(tmp)

            nxt = (state[0], state[1]+1)
            tmp = ('D', self.__calc_cost(state, nxt), nxt) 
            successor_states.append(tmp)

            if right:
                nxt = (state[0]+1, state[1]+1)
                tmp = ('DR', self.__calc_cost(state, nxt), nxt)  
                successor_states.append(tmp)

        # left and right leftovers
        if right:
            nxt = (state[0]+1, state[1])
            tmp = ('R', self.__calc_cost(state, nxt), nxt)
            successor_states.append(tmp)

        if left:
            nxt = (state[0]-1, state[1])
            tmp = ('L', self.__calc_cost(state, nxt), nxt)   
            successor_states.append(tmp) 

        return successor_states   


    def __calc_cost(self, state, successor):
        # calc cost to move between two states
        # successor helper method
        #
        # @param {tuple} (x,y) state
        # @param {tuple} (x,y) successor

        now = self.img.getpixel(state)
        nxt = self.img.getpixel(successor)

        cost = 1
        for i in range(3):
            cost += ((abs(now[i]-nxt[i]))/32)**2

        return cost



    def show_path(self, listofstates, pathfname='path.png'):
        """Given a list of states (each state specified as a tuple of
        (x,y) coordinates within the image, paint all states red and
        save the image."""

        imgcopy = self.img.copy()
        for (action, state) in listofstates:
            imgcopy.putpixel(state, (255, 0, 0))
        imgcopy.save(pathfname)


class AStar():

    def __init__(self, problem):
        """Initializer for an AStar search method.

        You must maintain the following instance variables:
          (1) self.reached:  a lookup table, with keys from the *state* space
                 and values corresponding to *TNode* instances

          (2) self.frontier:    a priority queue of TNode instances ordered by (g+h)

        """

        self.problem = problem
        self.reached = {}
        self.frontier = []

    def search(self, initialstate, costlimit=None, quiet=True):
        """_ Part 2: Implement This Method _

        Performs A* Search.
         (1) initializes reached table and frontier instance variables
         (2) uses self.problem to call:
              the successor function, heuristic function, and goal test
         (3) returns a search tree node (i.e., path), or False

        Arguments:
          costlimit - None (no limit) or a value such that the search
                       will quit when a node is removed from the frontier
                       with g > costlimit

          quiet - prints no output if this is True; the tests may assume
                    that you honor the quiet flag (i.e., that you don't
                    produce extraneous output)
          initialstate - (x,y)

        Returns a TNode constituting a path, or False if the search failed.
        """

        # set to None to turn optimization off
        # TNode = namedtuple('TNode', ['f', 'g', 'h', 'state', 'parent', 'action'])
        # f - g + h
        # g - cost of nodes leading to this node
        # h - cost of this node
        # tuples of: (<dir>, edgecost, nextstate)

        node = TNode(0, 0, self.problem.h(initialstate),
            initialstate, None, None)
        self.frontier.append(node)
        self.reached[initialstate] = node

        heapify(self.frontier)

        while(len(self.frontier) > 0):
            node = heappop(self.frontier)

            if (self.problem.is_goal(node.state)):
                return node

            for ent in self.problem.successors(node.state):
                tmp = TNode(
                    self.problem.h(ent[2]) + ent[1] + node.g,    # f
                    node.g + ent[1],                    # g
                    self.problem.h(ent[2]),             # h
                    ent[2], node, ent[0])
                # add node
                if tmp.state not in self.reached.keys(): 
                    self.reached[tmp.state] = tmp
                    heappush(self.frontier, tmp)
                # update node
                elif tmp.f < self.reached[tmp.state].f:
                    self.reached[tmp.state] = tmp
                    heappush(self.frontier, tmp)

        return False


    def list_of_states(self, treenode):
        """Given a TNode instance, create a list of states representing
        the path *from the root* of the tree to the specified TNode instance.
        This should be obtainable by following the parent references from the
        given TNode back to the root. If TNode's aren't linked together
        correctly to form a tree, this will almost certainly probably break.

        Returns a list of states, the first state should be an initial state,
        the last state should be the state represented by the specified TNode
        instance"""

        stack = []
        node = treenode
        while node:
            #print(node)
            (f, g, h, s, parent, a) = node
            stack.append(TNode(f, g, h, s, None, a))
            node = parent
        stack.reverse()
        return [(node.action, node.state) for node in stack]


if __name__ == "__main__":
    # You shouldn't need to modify this...

    parser = argparse.ArgumentParser()
    parser.add_argument('image', nargs="?")
    parser.add_argument('--verbose', action='store_true')
    parser.add_argument('-x', type=int, default=0,
                        help="initial x coordinate (default 0)")
    parser.add_argument('-y', type=int, default=0,
                        help="initial y coordinate (default 0)")
    parser.add_argument('-n', type=int,
                        help="show only first n states on the path",
                        default=-1)

    args = parser.parse_args()

    if args.image:
        image = PIL.Image.open(args.image)
        size = (image.width, image.height)

        # This heuristic always works!
        def h_fn(x): return 0

        def goal_fn(x): return x == (size[0] - 1, size[1] - 1)

        prob = ImageProblem(args.image, goal_fn, h_fn)
    else:
        prob = LittleGridProblem()

    a = AStar(prob)
    start = time.time()

    # WARNING: the unit tests will run your program from the command line
    # and look at the output.  Make sure your search obeys the 'quiet' flag.
    # and make sure you you don't change this when you submit.
    tnode = a.search((args.x, args.y), quiet=(not args.verbose))
    stop = time.time()
    states_on_path = a.list_of_states(tnode)
    left_to_print = args.n
    for s in states_on_path:
        if left_to_print == 0:
            break
        print(s)
        left_to_print -= 1

    print("Path found in %.2f seconds" % (stop-start))
    print("%d states in the reached dict" % len(a.reached))
    print("%d states on path to goal" % len(states_on_path))
    print("cost is: %.2f" % tnode.g)

    if args.image:
        prob.show_path(states_on_path)
