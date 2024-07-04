"""TicTacToe. A module for playing a simple game.

Ian Manix

"""
import sys

def int_input(state, mover):
    """A move function that asks for input from stdin..."""

    print("%s's turn...(0..%d)" % (TicTacToe.Chrs[mover], len(state)-1))
    # added error check for for cases of empty strings being entered by user
    try:
        return int(input())
    except ValueError:
        return -1


class TicTacToe():
    """A Class representing the game of TicTacToe.

    Each instance maintains the size of the board and it's current state.
    The instance also tracks whos turn it is.

    the move() method is used to manipulate the board state in a safe (legal)
    manner

    the show() method is used to display the board state in a human readable
    form

    the play() method is used to play a game with 'desired' moves supplied by
    an external function.
    """
    Column = 0
    Row = 1
    DiagonalRight = 2
    DiagonalLeft = 3
    StaleMate = 4
    Chrs = {0: ' ', 1: 'X', 2: 'O'}

    def __init__(self, n=3):
        """Create a n-by-n tic-tac-toe game. n=3 by default,
           but for full credit you should support arbitrary n>=2.

           If you need to use additional instance variables, you
           should initialize them here, and be sure to reinitialize
           them (if necessary) in reset()
        """

        self.n = n
        self.n2 = n**2
        self.reset()

    def reset(self, state=None):
        """Reset the game to the specified state, or to an empty board.
        A state is internally encoded as a list of elements in {0, 1, 2}.
        0 represents an empty space, 1 represents an 'X' (player 1), and 2
        represents an 'O' (player 2). The state is assumed to have an
        appropriate number of 'X's relative to the number of 'O's.

        The internal state has indicies 0...(n^2-1) which correspond to cells
        from the upper left corner (0) to the bottom right (n^2-1) running
        left-right and top-down
        """

        if state:
            n_x = sum([1 for i in state if i == 1])
            n_oh = sum([1 for i in state if i == 2])
            # ones (x's) go first

            assert n_x == n_oh or n_x == n_oh+1, "X's (1's) go first."
            assert len(state) == self.n2, "the specified state is not the correct length"
            # The game state is kept here as a list of values.
            # 0  indicates the space is unoccupied;
            # 1  indicates the space is occupied by Player 1 (X)
            # 2  indicates the space is occupied by Player 2 (O)
            self.board = list(state)
            self.turn = (n_x -  n_oh) + 1  # X's turn if moves are equal

        else:
            self.board = [0]*(self.n2)
            self.turn = 1

    def move(self, where):
        """_ Part 1: Implement This Method _

        Perform the current player's move at the specified location/index and
        change turns to the next player; where is an index into the board in
        the range 0..(n**2-1)

        If the specified index is a valid move, modify the board,
        change turns and return True.

        Return False if the specified index is unopen, or does not exist.

        Don't forget that for full credit, you need to deal with an
        arbitrary nxn size board, not simply 3x3.
        """

        xy = self.get_xy(where)

        ''' error if given impossible position'''
        if (where >= self.n2):
            print("Position "+str(xy[0])+", "+str(xy[1])+" dose not exist")
            return False

        ''' error if given illegal position'''
        if (self.board[where] != 0 ):
            print("Position "+str(xy[0])+", "+str(xy[1])+" already taken")
            return False

        ''' assumes self.turn is always 1 or 0'''
        self.board[where] = self.turn

        # swap turn
        if self.turn == 1:
            self.turn = 2
        else:
            self.turn = 1

        return True

    def show(self):
        """_ Part 2: Implement This Method _

        Display the board on stdout (e.g., using print())

        Simply write a method that displays the board in a logical and
        human-readable manner."""

        where = 0
        for i in range(self.n):
            row = str()
            for j in range(self.n):
                ''' build row, example expected row:
                    ` X |   | O `'''
                row = row + ' '
                if (self.board[where] == 1):
                    row = row + 'X'
                elif (self.board[where] == 2):
                    row = row + 'O'
                else:
                    row = row + ' '
                row = row + ' |'
                where += 1
            print(row[:-1])

            # skip on last row
            if (i!= self.n-1):
                print('-'*(len(row)-1))

        return

    def is_win(self):
        """_ Part 3: Implement This Method _

        Determine if the current board configuration is an end game.
        For a board of size n, a win requires one player to have n tokens
        in a line (vertical, horizontal or diagonal).

        For full credit, comment your code to indicate what each code block
        is trying to do.

        Returns:

         (TicTacToe.Column, ?, player): if player wins in column c
         (TicTacToe.Row, ?, player): if player wins in row r
         (TicTacToe.DiagonalRight, ?, player): if player wins via
           a diagonal starting in the upper-left corner and proceeding
           down and to the right
         (TicTacToe.DiagonalLeft, ?, player): if player wins via a
           diagonal in the starting in the upper-right corner and proceeding
           down and to the left
         (TicTacToe.StaleMate, 0, 0): if the game is a stalemate
         False: if the end state is not yet determined

         ? in the tuples above indicates the lowest value cell index
         involved in the win. Cell indices range from 0...(n^2-1) and
         go from the top left of the board to the bottom right.
        """
        # check for column win
        # check columns
        for i in range(self.n):
            # for every column in the board
            is_win_b = True
            test = self.board[i]
            test_val = i

            if (test == 0):
                # if empty in row, skip
                is_win_b = False
                continue

            for j in range(1, self.n):
                # test each element in column for full match
                if (self.board[i+(j*self.n)] != test):
                    # if any match fails, go to next column
                    is_win_b = False
                    break

            # win has been found, skip rest of board
            if is_win_b:
                break

        # if win, no need to continue
        if (is_win_b):
            return (TicTacToe.Column, test_val, test)

        # check rows
        for i in range(self.n):
            row = i * self.n
            is_win_b = True
            test = self.board[row]
            test_val = row

            if (test == 0):
                # if empty in row, skip
                is_win_b = False
                continue

            for j in range(1, self.n):
                if (self.board[row+j] != test):
                    is_win_b = False
                    break

            # win has been found, skip rest of board
            if is_win_b:
                break

        # if win, no need to continue
        if (is_win_b):
            return (TicTacToe.Row, test_val, test)

        # check diagonal
        # left->right
        is_win_b = True
        test = self.board[0]
        if test == 0:
            # if empty in row, skip
            is_win_b = False
        if is_win_b:
            for i in range(1,self.n):
                if (self.board[i*self.n + i] != test):
                    is_win_b = False
                    break

        # if win, no need to continue
        if is_win_b:
            return (TicTacToe.DiagonalRight, 0, test)

        # right<-left
        is_win_b = True
        test = self.board[self.n-1]
        if test == 0:
            # if empty in row, skip
            is_win_b = False
        if is_win_b:
            for i in range(2,self.n+1):
                if (self.board[i*self.n - i] != test):
                    is_win_b = False
                    break

        if (is_win_b):
            return (TicTacToe.DiagonalLeft, self.n-1, test)

        # Test for stalemate
        # only stalemate condition is if board is full
        for cell in self.board:
            if cell == 0:
                # no win con, game not over
                return False
        # no win con, all spaces filled
        return (TicTacToe.StaleMate, 0, 0)


    def describe_win(self, win):
        """Provides a text representation of an end-game state.

        win is a tuple:
         (reason code, cell location, player)
        """


        reason = {TicTacToe.Row: "Row", TicTacToe.Column: "Column",
                  TicTacToe.DiagonalLeft: "Diagonal Down and Left",
                  TicTacToe.DiagonalRight: "Diagonal Down and Right"}

        if win[0] == TicTacToe.StaleMate:
            return "StaleMate!"
        if win[0] == TicTacToe.DiagonalLeft:
            if win[1] == 0:
                where = "Upper Left"
            else:
                where = "Upper Right"
        else:
            where = "%d" % win[1]
        return "'%s' (%d) wins @ %s from cell %s" %(TicTacToe.Chrs[win[2]],
                                                    win[2], reason[win[0]],
                                                    win[1])

    def play(self, movefn=int_input, display=True, showwin=True):
        """_ Part 4: Implement This Method _

        Play the game of tictactoe!

        Arguments:
        movefn - a function that will provide possibly valid moves.
        display - show the game? (default: True)
        showwin - if True, explicitly indicate the game is over
                  and describe the win

        Play should work (roughly) as follows:
         - verify the game is not in an end state
         - display the game state (using show()) if the display flag is set
         - acquire the next 'intended' move from the movefn (see note below).
         - call the move method with 'intended' move
         - repeat steps above

         when an end state is reached:
         - print the state (if display flag is set) and
         - print 'Game Over!' along with a description of the win
           if showwin is True.

        the movefn should take two arguments:
          (1) the game state; and (2) the current player; because the internal
          state (self.board) is a list, you should pass a tuple to the movefn
          instead of directly passing self.board.  This ensures that the movefn
          can't actually manipulate the state of the board, it can only request
          that a move takes place -- it's up to the logic of the move() method to
          actually manipulate the game state/board.
        """
        # main game loop
        player = 1
        #self.turn = 0
        while True:

            win = self.is_win()

            if display:
                self.show()

            # if game is won
            if win != False:
                if showwin:
                    print("Game Over!")
                    print(self.describe_win(win))
                # end main loop, ending game
                break

            # loop until valid move has been given
            while True:
                int_move = movefn(tuple(self.board), self.turn)

                # if bad input
                if int_move == -1:
                    print("Please enter an int")
                    continue

                # if valid move break loop
                if self.move(int_move):
                    break
                # else keep looping

        # return win state for Monte-carlo function
        return win


    def get_state(self):
        """Get the state of the board as an immutable tuple"""
        return tuple(self.board)

    def get_xy(self, where):
        ''' Get the x,y of a given position'''
        x = int(where/self.n)
        y = where % self.n

        return (x, y)


def mc_mover(state, mover):
    ''' move function for TicTacToe.play()
    returns a random valid move from all possible moves 
    mover is unused, but taken as argument to maintain parity 
    with other movefn functions'''
    movelist = list()
    for i in range(len(state)):
        if state[i] == 0:
            movelist.append(i)

    return movelist.pop(random.randrange(0, len(movelist)))

def mc(state, trials, n, debug=False, seed=None):
    """_ Part 5 (Optional) : Implement This Method _

    Arguments:

     state   -- the initial state in which to start the game
     trials  -- the number of trials to run (games to play)
     n       -- the size of the board to make (n=3 in normal TicTacToe)
     debug   -- flag to enable more verbose printing


    Run a monte-carlo experiment in which we play the game using random
    moves. Here's how it should work:
      - create a function to generate with random moves (one that
         can be passed to the TicTacToe.play() method)
      - create a TicTacToe instance of board size n
      - loop for the number of trials and:
         - initialize the TicTacToe instance to the specified state
         - play the tictactoe game
         - track the outcome
      - return the distribution of outcomes as a tuple (see below).

    Monte-carlo experiments such as this are used to evaluate states in complex
    games such as chess and go.

    Return a 4-tuple of:
    (games played, % won by player-1, % won by player-2, % stalemates)


    """
    import random

    # generate list of legal moves from starting state
    

    display = False
    showwin = False

    if debug:
        print("In initial board state: "+str(state))

        # generate list of valid moves from given state
        # for debug purposes
        movelist = list()
        for i in range(len(state)):
            if state[i] == 0:
                movelist.append(i)

        print("Legal Moves: "+str(movelist))
        if seed:
            print("Seed: "+str(seed))
        else:
            print("No Seed")
        display = True
        showwin = True

    # results list, [num stalemates, num X wins, num O wins]
    results = [0, 0, 0]

    ttt = TicTacToe(n=n)

    for i in range(trials):
        # allows for set seed for debug purposes
        if seed != None:
            random.seed(seed + i)

        # setup initial board state
        ttt.reset(state=state)

        # loop until game ends
        end = False
        while not end:
            end = ttt.play(movefn=mc_mover, display=display, showwin=showwin)

        # increment results
        results[end[2]] += 1

    # stalemates moved from index 0 to index 2 to conform with expected output
    results_precent = (trials, results[1]/trials, results[2]/trials, results[0]/trials)
    return results_precent    


if __name__ == "__main__":
    import argparse
    import random
    parser = argparse.ArgumentParser()
    parser.add_argument("--play", action='store_true')
    parser.add_argument("--state",
                        help="initial state comprised of values in {0,1,2}")
    parser.add_argument("--mc", type=int, default=1000,
                        help="monte carlo trials; default=%{default}")
    parser.add_argument("-n", type=int, default=3,
                        help="board length,width; default=%{default}")
    args = parser.parse_args()

    if args.state:
        # At the command line state will come in as a string drawn
        # from {0,1,2}.
        assert len(args.state) == args.n**2, \
            "Expected string with %d elements" % (args.n**2)

        state = [int(z) for z in args.state]
        stateset = set(state)
        assert stateset.issubset(set([0, 1, 2])), \
            "Expected string with elements 0,1,2"
        state = tuple(state)
        print("State is:", state)
    else:
        state = tuple([0]*(args.n**2))


    if args.play:
        t = TicTacToe(args.n)
        t.reset(state)
        t.play(display=True)

    elif args.mc:
        (games, one, two, stale) = mc(state, args.mc, args.n)
        print("%d trials: 1 wins %.2f, "
              "-1 wins %.2f, stalemates %.2f" % (games, one, two, stale))
